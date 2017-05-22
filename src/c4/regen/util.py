import sys
import subprocess

debug_mode = True

hdr_ext = '.hpp'
src_ext = '.cpp'

hdr_exts = ['.h', '.hpp', '.hxx', '.hh', '.H', '.h++']
src_exts = ['.c', '.cpp', '.cxx', '.cc', '.C', '.c++']

# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
def ext(file_name):
    """returns the last extension of a file name"""
    return os.path.splitext(file_name)[1]


def is_hdr(file_name):
    """returns true if the given file name is a C/C++ header file (.h/.hpp and similar)"""
    return ext(file_name) in hdr_exts


def is_src(file_name):
    """returns true if the given file name is a C/C++ source file (.c/.cpp and similar)"""
    return ext(file_name) in src_exts


def inc_guard(header_name):
    """convert a header file name into an include guard"""
    return "_{0}_".format(re.sub(r'[./\\]','_', header_name.upper()))


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
def logerr(*args, **kwargs):
    print(*args, **kwargs, file=sys.stderr)


def dbg(*args, **kwargs):
    if debug_mode:
        print(*args, **kwargs, file=sys.stderr)


def in_windows():
    return sys.platform == "win32"


def in_unix():
    return sys.platform in ("linux", "linux2", "darwin")


def get_output(cmd):
    status, output = subprocess.getstatusoutput(cmd)
    if status != 0:
        msg = "command failed with status {}: {}\noutput was:\n{}"
        msg = msg.format(status, cmd, output)
        raise Exception(msg)
    return output


def cacheattr(obj, name, function):
    """add and cache an object member which is the result of a given function.
    This is for implementing lazy getters when the function call is expensive."""
    if hasattr(obj, name):
        val = getattr(obj, name)
    else:
        val = function()
        setattr(obj, name, val)
    return val


def splitesc_quoted(string, split_char, escape_char='\\', quote_chars='\'"'):
    """split a string at split_char, but respect (and preserve) all the
    characters inside a quote_chars pair (including escaped quote_chars and
    split_chars). split_char can also be escaped when outside of a
    quote_chars pair."""
    out = []
    i = 0
    l = len(string)
    prev = 0
    while i < l:
        is_escaped = (i > 0 and string[i - 1] == escape_char)
        c = string[i]
        # consume at once everything between quotes
        if c in quote_chars:
            j = i+1  # start counting only on the next position
            closes_quotes = False
            while j < l:
                d = string[j]
                is_escaped_j = (j > 0 and string[j - 1] == escape_char)
                if d == c and not is_escaped_j:  # found the matching quote
                    j += 1
                    closes_quotes = True
                    break
                j += 1
            # but defend against unbalanced quotes,
            # treating them as regular characters
            if not closes_quotes:
                i += 1
            else:
                s = string[prev:j]
                if s:
                    out.append(s)
                prev = j+1
                i = prev
        # when a split_char is found, append to the list
        elif c == split_char and not is_escaped:
            if i > 0 and i < l and i > prev:
                s = string[prev:i]
                if s:
                    out.append(s)
            prev = i+1
            i += 1
        # this is a regular character, just go on scanning
        else:
            i += 1
    # if there are still unread characters, append them as well
    if prev < l:
        s = string[prev:l]
        if s:
            out.append(s)
    return out
