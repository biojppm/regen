import clang.cindex
from clang.cindex import CursorKind, TranslationUnit
import os.path
import asciitree  # must be version 0.2+
from collections import OrderedDict as odict
import subprocess
import tempfile
import ccsyspath
from shutil import which
import sys

from .util import logerr, dbg, cacheattr

clang_version = "3.8"
clang_options = (TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD
                 | TranslationUnit.PARSE_SKIP_FUNCTION_BODIES
                 # | TranslationUnit.PARSE_INCLUDE_BRIEF_COMMENTS_IN_CODE_COMPLETION
)

# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

def load_clang(libdir=None): #, version=clang_version):
    def _doit(libdir):
        version = clang_version
        def _dbg(*args, **kwargs): dbg("loading clang:", *args, **kwargs)
        _dbg("libdir=", libdir, "version=", version)
        _dbg("LD_LIBRARY_PATH=", os.environ["LD_LIBRARY_PATH"])
        _dbg("PATH=", os.environ["PATH"])
        if libdir is None:
            exe = find_llvm_config(version)
            _dbg("llvm-config=", exe)
            if exe is None:
                msg = "could not find a suitable llvm-config executable. Searched version {}"
                raise Exception(msg.format(version))
            libdir = subprocess.getoutput(exe + " --libdir")
            _dbg("libdir=", libdir)
        if libdir is not None:
            clang.cindex.Config.set_library_path(libdir)  # this doesn't work, at least in Ubuntu 16.04 x64
            libname = "libclang.so"  # FIXME for windows
            lib = os.path.join(libdir, libname)
            _dbg("lib=", lib)
            if not os.path.exists(lib):
                _dbg("lib not found!")
                raise Exception("lib not found: " + lib)
            assert os.path.exists(lib)
            clang.cindex.Config.set_library_file(lib)
        return lib
    return cacheattr(sys.modules[__name__], 'clang_loaded', lambda: _doit(libdir))


def find_llvm_config(version=clang_version):
    def _getit():
        def _dbg(*args, **kwargs): dbg("looking for llvm-config:", *args, **kwargs)
        _dbg("version", version)
        exe = which("llvm-config")
        _dbg("'llvm-config' in path?")
        if exe is None:
            _dbg("'llvm-config' not found")
        else:
            _dbg("'llvm-config' found:", exe)
            llvmc_version = subprocess.getoutput(exe + " --version")
            _dbg("'llvm-config' version:", llvmc_version)
            majmin = ".".join(llvmc_version.split(".")[0:2])
            ref = ".".join(version.split(".")[0:2])
            if majmin == ref:
                _dbg("'llvm-config' version ok")
                return exe
            else:
                _dbg("'llvm-config' version not ok")
                exe = None
        ver = "llvm-config-" + version
        _dbg(ver, "in path?")
        exe = which(ver)
        if exe is not None:
            _dbg("found", ver, ":", exe)
        else:
            _dbg("not found", ver, ":", exe)
        return exe
    return cacheattr(sys.modules[__name__], 'llvm_config', _getit)


def get_inc_path():
    def _getit():
        def _dbg(*args, **kwargs): dbg("clang include path:", *args, **kwargs)
        llvmc = find_llvm_config()
        bindir = subprocess.getoutput(llvmc + " --bindir")
        clangexe = os.path.join(bindir, 'clang++')
        _dbg("clang++:", clangexe)
        if not os.path.exists(clangexe):
            raise Exception("could not find clang++: " + clangexe)
        incpaths = ccsyspath.system_include_paths(clangexe)
        for p in incpaths:
            _dbg("clang++ include paths:", str(p))
        return ["-I {}".format(str(i, 'utf-8')) for i in incpaths]
    return cacheattr(sys.modules[__name__], 'clang_inc_path', _getit)


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
def parse_file(filename, args=[], options=clang_options):
    ip = get_inc_path()
    dbg("parse_file:", filename, ": args=", args)
    dbg("parse_file:", filename, ": inc_path=", ip)
    args += ip
    def _e(diags=None):
        msg = "{}: parse error: {}"
        msg = msg.format(filename, get_diagnostics_string(diags))
        if args:
            msg += "{}: args={}".format(filename, args)
        return msg
    idx = cacheattr(sys.modules[__name__], 'clang_idx',
                    lambda: clang.cindex.Index.create())
    try:
        if not os.path.exists(filename):
            raise Exception("file not found: " + filename)
        tu = idx.parse(path=filename, args=args + ip, options=options)
        if tu.diagnostics:
            logerr(_e(tu.diagnostics))
    except:
        if tu:
            raise Exception(_e(tu.diagnostics))
    if not tu:
        raise Exception(_e())
    return tu


def parse_source(source, args=[], options=clang_options):
    """@TODO is there a better way to accomplish this without writing a temporary file?"""
    f, n = tempfile.mkstemp(prefix='regen', suffix='.cpp')
    with open(n, "w") as f:
        if isinstance(source, str):
            f.write(source)
        elif isinstance(source, list):
            f.writelines(source)
        f.close()
    tu = parse_file(n, args, options)
    os.remove(n)
    return tu


def get_diagnostics_string(diags):
    s = ""
    if diags is not None:
        for d in diags:
            fl = fileline(d)
            s += '\n{0}: {1}: {2}'.format(fl, d.severity, d.spelling)
            if d.fixits is not None:
                for f in d.fixits:
                    s += '\n{0}:     {1}'.format(fl, str(f))
        s += "\n"
    return s


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
def same_trans_unit(node, trans_unit):
    return (node.location.file
            and node.location.file.name == trans_unit.cursor.displayname)


def file(node):
    return node.location.file


def line(node):
    return node.location.line


def fileline(node):
    l = node.location
    return str(l.file) + ":" + str(l.line)


def straddles(node, line):
    extent = node.extent
    ge = extent.start.line <= line
    le = extent.end.line >= line
    return ge and le


def get_node_children_in_translation_unit(node, trans_unit):
    return [c for c in node.get_children() if same_trans_unit(c, trans_unit)]


def find_nodes(root_node, node_kinds, node_names=None, trans_unit=None, descend=True):
    """
    Visit the tree starting at root_node, get all nodes of node_kind
    and (optionally) equal to node_name.
    """
    if not isinstance(node_kinds, list):
        node_kinds = [node_kinds]
    if (node_names is not None) and (not isinstance(node_names, list)):
        node_names = [node_names]
    def _get_node_children(node, l):
        for c in node.get_children():
            if trans_unit and not same_trans_unit(c, trans_unit):
                continue
            #print(c, c.location.file, c.location.line, c.kind, c.spelling)
            if c.kind in node_kinds:
                if (not node_names) or (c.spelling in node_names):
                    l.append(c)
            if descend:
                l = _get_node_children(c, l)
        return l
    return _get_node_children(root_node, [])


def find_function(trans_unit, function_name):
    nodes = find_nodes(trans_unit.cursor,
                       [clang.cindex.CursorKind.FUNCTION_DECL],
                       function_name, trans_unit)
    if len(nodes) == 0:
        return None
    else:
        assert len(nodes) == 1
        return nodes[0]


def find_macro_instantiations(trans_unit, macro_name):
    return find_nodes(trans_unit.cursor,
                      [CursorKind.MACRO_INSTANTIATION],
                      macro_name, trans_unit)


def find_node_with_offset(cursor, line_offset, column_offset):
    tu = cursor.translation_unit
    cl = cursor.location
    l = cl.line + line_offset
    c = cl.column + column_offset
    loc = clang.cindex.SourceLocation.from_position(tu, cl.file, l, c)
    ans = clang.cindex.Cursor.from_location(tu, loc)
    return ans


def get_tokens_string(tokens):
    s = ""
    for tk in tokens:
        if s:
            s += " "
        s += tk.spelling
    return s


def get_token_cursor(token, trans_unit):
    cl = token.location
    loc = clang.cindex.SourceLocation.from_position(trans_unit, cl.file, cl.line, cl.column)
    c = clang.cindex.Cursor.from_location(trans_unit, loc)
    return c


def find_enclosing_class_node(node):
    #n = find_node_with_offset(node, 0, -1) # TODO: HACK.......
    n = node.semantic_parent
    return n


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

def is_enum_class(enum_cursor):
    # TODO: HACK.......
    if enum_cursor.kind != CursorKind.ENUM_DECL:
        return False
    s = get_tokens_string(enum_cursor.get_tokens())
    well_is_it = (s[0:10] == 'enum class')
    return well_is_it


def underlying_type(enum_cursor):
    raise Exception("not implemented")


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

def is_template(cursor):
    k = cursor.kind
    return (k == CursorKind.FUNCTION_TEMPLATE
            or k == CursorKind.CLASS_TEMPLATE
            or k == CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION
    )


def find_template_parameters(cursor):
    # @TODO there's probably a better way to do this
    return find_nodes(cursor, [CursorKind.TEMPLATE_TYPE_PARAMETER,
                               CursorKind.TEMPLATE_NON_TYPE_PARAMETER,
                               CursorKind.TEMPLATE_TEMPLATE_PARAMETER],
                      descend=False)


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

def get_comment(cursor, skip_macro_call_above_line=False):
    prev = get_comment_prev_line(cursor, skip_macro_call_above_line)
    same = get_comment_same_line(cursor)
    c = prev + same
    return c


def get_comment_prev_line(cursor, skip_macro_call_above_line=False):
    line = cursor.location.line - 1
    if skip_macro_call_above_line:
        for c in cursor.translation_unit.cursor.get_children():
            if c.kind == CursorKind.MACRO_INSTANTIATION and straddles(c, line):
                line = c.location.line - 1
    l = get_comment_tokens(cursor.translation_unit, at_line=line)
    return "\n".join([i.spelling for i in l]) if l else ""


def get_comment_same_line(cursor):
    l = get_comment_tokens(cursor.translation_unit, at_line=cursor.location.line)
    return "\n".join([i.spelling for i in l]) if l else ""


def get_comment_tokens(trans_unit, at_line=None, outside_of_cursor=None):
    l = []
    for t in trans_unit.cursor.get_tokens():
        if at_line is None or straddles(t, at_line):
            if outside_of_cursor is None or outside_of_cursor:
                if t.kind == clang.cindex.TokenKind.COMMENT:
                    l.append(t)
    return l


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

def print_ast(trans_unit, start_node=None, with_tokens=False):
    def _get_node_children(node):
        return [c for c in node.get_children()
                if (c.location.file
                    and c.location.file.name == trans_unit.cursor.displayname)
        ]
    def _print_node(node):
        text = node.spelling or node.displayname
        kind = str(node.kind)[str(node.kind).index('.')+1:]
        fn = fileline(node)
        txt = '{} {} ({})'.format(kind, text, fn)
        if with_tokens:
            txt += "\n"
            txt += get_tokens_string(node.get_tokens())
        return txt
    if start_node is None:
        start_node = trans_unit.cursor
    print(asciitree.draw_tree(start_node, _get_node_children, _print_node))


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
class IncludeList:
    def __init__(self, file):
        self.file = os.path.abspath(file)
        self.incs = None

    def append(self, line, inc):
        il = IncludeList(inc)
        if self.incs is None:
            self.incs = odict()
        self.incs[int(line)] = il
        return il

    def printrec(self, istack=0, sep='  '):
        if self.incs is None:
            return
        for line, inc in self.incs.items():
            f = str(self.file).lstrip(" ").rstrip(" ")
            i = str(inc.file).lstrip(" ").rstrip(" ")
            print(istack * sep, "{0}:{1}: #include \"{2}\"".format(f, line, i))
            inc.printrec(istack + 1)


def get_include_tree(trans_unit):
    tree = IncludeList(trans_unit.cursor.displayname)
    s = [tree]
    prev_depth = 0
    for i in trans_unit.get_includes():
        if i.depth <= prev_depth:
            s = s[0:(i.depth - prev_depth - 1)]
        prev_depth = i.depth
        curr = s[-1].append(i.location.line, os.path.abspath(str(i.include)))
        s.append(curr)
    return tree


def print_includes(trans_unit):
    t = get_include_tree(trans_unit)
    t.printrec()
#    for i in trans_unit.get_includes():
#        l = i.location
#        l = os.path.abspath(str(l.file)) + ":" + str(l.line)# + ":" + str(l.column)
#        print("include:", i.depth, l, os.path.abspath(str(i.include)))
