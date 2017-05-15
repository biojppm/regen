from . import clang_utils as clu
from clang.cindex import TokenKind as tkk, CursorKind as ck
from collections import OrderedDict as odict
import jinja2 as jj2
import re
import os.path

from . import util

# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

hdr_ext = '.hpp'
src_ext = '.cpp'

hdr_exts = ['.h', '.hpp', '.hxx', '.hh', '.H', '.h++']
src_exts = ['.c', '.cpp', '.cxx', '.cc', '.C', '.c++']

tpl_env = jj2.Environment(trim_blocks=True, lstrip_blocks=True)

chunk_intro = "/** {{generator}}: auto-generated from {{originator}} */\n"
chunk_outro = ""


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


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

class CodeEntity:

    def __init__(self, ast_node=None):
        self.ast_node = ast_node

    @property
    def file(self):
        assert self.ast_node is not None
        return clu.file(self.ast_node)

    @property
    def line(self):
        assert self.ast_node is not None
        return clu.line(self.ast_node)

    @property
    def fileline(self):
        assert self.ast_node is not None
        return clu.fileline(self.ast_node)

    @property
    def comment(self):
        if not hasattr(self, '_comment'):
            self._comment = clu.get_comment(self.ast_node)
        return self._comment

    def __str__(self):
        return self.fileline


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

class Annotation(CodeEntity):

    def __init__(self, ast_node):
        super().__init__(ast_node)
        tokens = list(ast_node.get_tokens())
        self.macro = tokens[0].spelling
        self.cursor = ast_node
        self.extent = ast_node.extent
        self.names = []
        # skip initial and final tokens
        # eg in MACRO_NAME(...) skip MACRO_NAME, (, ), and the next token
        # which for some reason is being spat out by cursor.get_tokens()
        tokens = tokens[2:-2]
        if not tokens:
            return
        i = 0
        while i < len(tokens):
            tk = tokens[i]
            if (tk.kind in (tkk.KEYWORD, tkk.IDENTIFIER)):
                n = tk.spelling
                v = None
                if (i+1) < len(tokens) and tokens[i+1].spelling == '=':
                    v = tokens[i+2].spelling
                    i += 2
                self.add(AnnotationValue(n, v))
            i += 1

    def add(self, annot_or_name, or_value=None):
        if isinstance(annot_or_name, str):
            annot_or_name = AnnotationValue(annot_or_name)
        assert not self.contains(annot_or_name.name)
        self.names.append(annot_or_name.name)
        setattr(self, annot_or_name.name, annot_or_name)

    def contains(self, name):
        if name not in self.names:
            return False
        return (getattr(self, name).val is not None)

    @property
    def empty(self):
        return (len(self.names) == 0)

    def __str__(self):
        s = ""
        for an in self.names:
            s += str(getattr(self, an)) + ","
        return "{" + s[0:-2] + "}"

# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

class AnnotationValue:

    def __init__(self, n, v=None):
        self.name = n
        self.val = v if (v is not None) else True

    def __str__(self):
        s = ""
        s += self.name
        if self.val is not None and self.val != True:
            s += "=" + str(self.val)
        return s

# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

class Var(CodeEntity):

    def __init__(self, ast_node):
        super().__init__(ast_node)
        #print(ast_node.kind, ast_node.displayname, fileline(ast_node), ast_node.type)
        self.name = ast_node.displayname
        self.type_name = self.ast_node.type.spelling
        self.type_class = None

    def __str__(self):
        s = self.fileline + str(self.type_name)
        return s

# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

class Member(Var):

    def __init__(self, cursor):
        super().__init__(cursor)
        self.class_cursor = cursor.semantic_parent#find_enclosing_class_node(cursor)
        self.class_name = self.class_cursor.displayname
        self.class_obj = None
        self.scoped_name = self.class_name + "::" + self.name
        #print(self)

    def __str__(self):
        s = clu.fileline(self.ast_node) + ": C4_MEMBER: "
        s += str(self.type_name) + " " + self.scoped_name
        return s


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
class Prop(Member):

    def __init__(self, annotation_cursor):
        # node on next line, same column
        cursor = clu.find_node_with_offset(annotation_cursor, 1, 0)
        super().__init__(cursor)
        self.annotation = Annotation(annotation_cursor)
        #print(self)

    def __str__(self):
        s = clu.fileline(self.annotation.ast_node) + ": C4_PROPERTY: "
        s += str(self.type_name) + " " + self.scoped_name
        if not self.annotation.empty: s += str(self.annotation)
        return s

    def resolve_class(self, classes):
        for c in classes:
            if c.name == self.class_name:
                self.class_obj = c
                c.add_prop(self)
            if c.name == self.type_name:
                self.type_class = c
        if self.class_obj == None:
            raise Exception(str(self) + ": could not find class!!!")


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
class Class(CodeEntity):

    def __init__(self, annotation_cursor):
        self.macro = Annotation(annotation_cursor)
        self.class_cursor = clu.find_enclosing_class_node(annotation_cursor)
        super().__init__(self.class_cursor)
        self.name = self.class_cursor.displayname
        self.is_template = False
        self.tpl_info = None
        self.tpl_params = ""
        if clu.is_template(self.class_cursor):
            self.is_template = True
            self.tpl_info = TplInfo(self.class_cursor)
            self.tpl_params = self.tpl_info.params_string
        self.props = []
        self.members = []
        for tk in self.class_cursor.get_tokens():
            m = None
            if tk.kind == tkk.IDENTIFIER:
                if tk.spelling == "C4_PROPERTY":
                    c = clu.get_token_cursor(tk, self.class_cursor.translation_unit)
                    m = Prop(c)
                    self.props.append(m)
                elif tk.cursor:
                    if tk.cursor.kind in (ck.FIELD_DECL, ck.VAR_DECL):
                        noprop = (self.find_property(tk.cursor.displayname) is None)
                        nomemb = (self.find_member(tk.cursor.displayname) is None)
                        if noprop and nomemb:
                            for c in self.class_cursor.get_children():
                                if (c.kind in (ck.FIELD_DECL, ck.VAR_DECL)
                                    and c.displayname == tk.spelling):
                                    m = Member(c)
                                    #print(c.displayname, c.type.spelling)
            if m is not None:
                self.members.append(m)
        #print(self)#, self.props, self.members)
        self.ctx = self._ctx()


    def _ctx(self):
        c = self
        ctx = {
            'type':self.name,
            'tpl_params':self.tpl_params,
            'members':[],
            'props':[],
        }
        for p in self.props:
            d = {
                'type':p.type_name,
                'name':p.name
            }
            ctx['props'].append(d)
        for m in self.members:
            d = {
                'type':m.type_name,
                'name':m.name
            }
            ctx['members'].append(d)
        return ctx

    def find_property(self, prop_name):
        for p in self.props:
            if p.name == prop_name:
                return p
        return None

    def find_member(self, member_name):
        for p in self.members:
            if p.name == member_name:
                return p
        return None

    def __str__(self):
        s = clu.fileline(self.macro.cursor) + ": C4_CLASS: " + self.name
        if not self.macro.empty: s += str(self.macro)
        return s

    def add_prop(self, p):
        self.props.append(p)


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

class Enum(CodeEntity):

    def __init__(self, macro_cursor):
        self.macro = Annotation(macro_cursor)
        self.enum_cursor = self._find_enum_node(macro_cursor)
        self.underlying_type = self.enum_cursor.enum_type.spelling
        super().__init__(self.enum_cursor)
        #
        # is this enum a C++1x enum class?
        self.is_class = clu.is_enum_class(self.enum_cursor)
        self.class_name = self.enum_cursor.displayname if self.is_class else ""
        #
        # is this enum nested in a class?
        enclosing_class = clu.find_enclosing_class_node(macro_cursor)
        if enclosing_class is not None and enclosing_class.kind == ck.INVALID_FILE:
            enclosing_class = None
        self.enclosing_class_cursor = enclosing_class
        self.enclosing_class_name = enclosing_class.displayname if enclosing_class is not None else ""
        #
        # load the symbols
        self.symbols = []
        for c in self.enum_cursor.get_children():
            if c.kind == ck.ENUM_CONSTANT_DECL:
                self.symbols.append(EnumSymbol(c))

    @property
    def ctx(self):
        if hasattr(self, '_ctx'):
            return self._ctx
        cn = self.class_name + "::" if self.class_name else ""
        enccn = (self.enclosing_class_name + "::") if self.enclosing_class_name else ""
        ename = enccn + self.enum_name
        self._ctx = {
            'enum':{
                'type': ename,
                'underlying_type': self.underlying_type,
                'comment': self.comment,
                'is_class': self.is_class,
                'enclosing_class': enccn,
                'enclosing_class_len': len(enccn),
                'ast_node': self.enum_cursor,
                'symbols': [
                    {
                        'name': cn + s.name,
                        'value': s.value,
                        'comment': s.comment,
                        'ast_node': s.ast_node,
                    } for s in self.symbols
                ]
            }
        }
        return self._ctx

    def __str__(self):
        sc = self.enclosing_class_cursor
        e = self.enum_cursor
        s = clu.fileline(self.macro.cursor) + ": C4_ENUM: "
        if sc: s += (sc.displayname or sc.spelling)+"::"
        s += self.enum_name
        #s += ": " + str(list(self.symbols.keys()))
        #s += "\n" + self.gen_code()
        return s

    def _find_enum_node(self, macro_node):
        # get the enum cursor
        cenum = clu.find_node_with_offset(macro_node, line_offset=1, column_offset=5) # @TODO HACK
        self.enum_name = cenum.displayname
        # when enums are written like typedef enum {...} EnumType
        # we need to get the type from the typedef cursor,
        # and then the enum from below it
        self.typedef_cursor = None
        if cenum.kind in (ck.NO_DECL_FOUND, ck.CLASS_DECL):
            next_line = 1
            while True:
                next_line += 1
                cenum = clu.find_node_with_offset(macro_node, line_offset=next_line, column_offset=5) # @TODO HACK
                if cenum.kind == ck.TYPEDEF_DECL:
                    self.enum_name = cenum.displayname
                    self.typedef_cursor = cenum
                    cenum = list(cenum.get_children())[0]
                    assert cenum.kind == ck.ENUM_DECL
                    break
                if next_line > 1000000:
                    raise Exception("could not find an enum node for enum ")
        return cenum

    @staticmethod
    def _check_sym_len(s):
        if len(s) > 255:
            raise Exception("symbol length must be under 256 characters. Got {0} in {1}".format(len(s), s))

# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

class EnumSymbol(CodeEntity):

    def __init__(self, symcursor):
        super().__init__(symcursor)
        self.name = symcursor.displayname
        self.value = symcursor.enum_value

# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

class TplInfo:

    "gathers the collection of parameters of a C++ template"

    def __init__(self, cursor):
        self.cursor = cursor
        self.params = []
        for n in clu.find_template_parameters(cursor):
            p = TplParam(n)
            self.params.append(p)
        s = ""
        for p in self.params:
            s += ", " if len(s) > 0 else ""
            s += str(p)
        self.params_string = s

# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------


class TplParam:

    def __init__(self, cursor):
        self.cursor = cursor
        self.tpl_info = None
        if self.cursor.kind == ck.TEMPLATE_TEMPLATE_PARAMETER:
            self.tpl_info = TplInfo(self.cursor)

    def __str__(self):
        c = self.cursor
        k = c.kind
        dn = (" " + c.displayname) if c.displayname else ""
        if k == ck.TEMPLATE_TYPE_PARAMETER:
            return "class" + dn
        elif k == ck.TEMPLATE_NON_TYPE_PARAMETER:
            return c.type.spelling + dn
        elif k == ck.TEMPLATE_TEMPLATE_PARAMETER:
            return "template<" + self.tpl_info.params_string + ">" + dn


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

class CodeChunk:
    """
    represents a chunk of auto-generated C/C++ source code, separated into
    declarations, source-file definitions and inline header-file definitions
    """
    intro_tpl = tpl_env.from_string(chunk_intro)
    outro_tpl = tpl_env.from_string(chunk_outro)

    def __init__(self, generator, originator, hdr=None, src=None, inl=None):
        """
        :param generator: the generator that created this chunk
        :param originator: the code entity that prompted creation of this chunk
        :param hdr: C/C++ declarations
        :param src: C/C++ definitions
        :param inl: C/C++ inline definitions (should be placed into headers)
        """
        self.generator = generator
        self.originator = originator
        self.ctx = {'generator':self.generator.name, 'originator':str(self.originator)}
        self.hdr = self._fmt(hdr)
        self.src = self._fmt(src)
        self.inl = self._fmt(inl)

    def _fmt(self, c):
        if not c: return ""
        i = CodeChunk.intro_tpl.render(self.ctx)
        o = CodeChunk.outro_tpl.render(self.ctx)
        return i + c + o

    @staticmethod
    def join(chunk_iterable, inline_in_header=True):
        """
        join a collection of CodeChunk objects into a string
        :param chunk_iterable: an iterable object with the collection of CodeChunk objects
        :param inline_in_header: whether inline definitions should be put in the header string
        :return: a tuple containing the strings for the header, for the
        inline definitions and for the source code definitions
        """
        if inline_in_header:
            hdr = "\n".join([c.hdr + c.inl for c in chunk_iterable])
            inl = ""
        else:
            hdr = "\n".join([c.hdr for c in chunk_iterable])
            inl = "\n".join([c.inl for c in chunk_iterable])
        src = "\n".join([c.src for c in chunk_iterable])
        return hdr, inl, src


# -------------------------------------------------------------------------------
# -------------------------------------------------------------------------------
# -------------------------------------------------------------------------------

class BaseGenerator:

    def __init__(self, **kwargs):
        self.name = kwargs.get('name', '')
        self.hdr = tpl_env.from_string(kwargs.get('hdr', ''))
        self.src = tpl_env.from_string(kwargs.get('src', ''))
        self.inl = tpl_env.from_string(kwargs.get('inl', ''))

    def _gen(self, originator, ctx):
        hdr = self.hdr.render(ctx)
        src = self.src.render(ctx)
        inl = self.inl.render(ctx)
        return CodeChunk(self, originator, hdr, src, inl)


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

class ClassGenerator(BaseGenerator):

    def __init__(self, **kwargs):
        kwargs['name'] = kwargs.get('name', 'ClassGenerator')
        super().__init__(**kwargs)

    def gen_code(self, c4class):
        ctx = c4class.ctx
        return self._gen(c4class, ctx)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

class EnumGenerator(BaseGenerator):

    def __init__(self, **kwargs):
        kwargs['name'] = kwargs.get('name', 'enum')
        super().__init__(**kwargs)

    def gen_code(self, c4enum):
        ctx = c4enum.ctx
        return self._gen(c4enum, ctx)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------


class ChunkWriterStdOut:
    """
    Write the generated source code chunks into stdout.
    """
    name = 'stdout'

    def write(self, source_file, chunk_iterable, mark=None):
        hdr, inl, src = CodeChunk.join(chunk_iterable)
        print(hdr)
        print()
        print(inl)
        print()
        print(src)

    def outfiles(self, source_file):
        """
        get the list of files output by this writer
        :param source_file: the source file
        :return: the list of files
        """
        return None


class ChunkWriterSameFile:
    """
    Write the generated source code chunks into the exact same source file
    where they are originated from.
    """
    name = 'same_file'

    def write(self, source_file, chunk_iterable, mark=None):
        cl = __class__
        mark = mark if mark is not None else cl.mark
        hdr, inl, src = CodeChunk.join(chunk_iterable)
        pump_to_file(source_file.filename, hdr+inl+src, mark, cl.begin, cl.end)

    def outfiles(self, source_file):
        """
        get the list of files output by this writer
        :param source_file: the source file
        :return: the list of files
        """
        return [source_file.filename]

    mark = "// regen:GENERATED"
    begin = mark + ":(BEGIN). DO NOT EDIT THE BLOCK BELOW. WILL BE OVERWRITTEN!\n"
    end = mark + ":(END). DO NOT EDIT THE BLOCK ABOVE. WILL BE OVERWRITTEN!\n"


class ChunkWriterGenFile:
    """
    Write the generated source code chunks into newly generated source files
    """
    name = 'gen_file'

    def __init__(self, tpl_hdr=None, tpl_src=None, tpl_inl=None):
        self.tpl_hdr = tpl_hdr if tpl_hdr is not None else ChunkWriterGenFile.tpl_hdr
        self.tpl_src = tpl_src if tpl_src is not None else ChunkWriterGenFile.tpl_src
        self.tpl_inl = tpl_inl if tpl_inl is not None else ChunkWriterGenFile.tpl_inl
        self.inl_to_hdr = self.tpl_inl != None and self.tpl_inl != ""

    def write(self, source_file, chunk_iterable):
        c = __class__
        hdr, inl, src = CodeChunk.join(chunk_iterable, inline_in_header=self.inl_to_hdr)
        ctx = {
            'notice': c.notice,
            'hdr': hdr,
            'inl': inl,
            'src': src,
            'filename': source_file.filename,
            'hdr_name': source_file.name_hdr,
            'src_name': source_file.name_src,
            'hdr_name_gen': source_file.name_hdr_gen,
            'inc_guard': inc_guard(source_file.name_hdr),
            'inc_guard_gen': inc_guard(source_file.name_hdr_gen)
        }
        if hdr: hdr = tpl_env.from_string(c.tpl_hdr).render(ctx)
        if src: src = tpl_env.from_string(c.tpl_src).render(ctx)
        if inl: inl = tpl_env.from_string(c.tpl_inl).render(ctx)
        if hdr: open(source_file.name_hdr_gen, "w").write(hdr)
        if src: open(source_file.name_src_gen, "w").write(src)
        if inl: open(source_file.name_inl_gen, "w").write(inl)

    def outfiles(self, source_file):
        """
        get the list of files output by this writer
        :param source_file: the source file
        :return: the list of files
        """
        return [source_file.name_hdr_gen, source_file.name_src_gen]

    notice = """// GENERATED AUTOMATICALLY. DO NOT EDIT THIS FILE: IT WILL BE OVERWRITTEN."""
    tpl_hdr = ("""\
{{notice}}

#ifndef {{inc_guard_gen}}
#define {{inc_guard_gen}}

#include "{{hdr_name}}"

{{hdr}}
{{inl}}

#endif // {{inc_guard_gen}}
""")

    tpl_src = ("""\
{{notice}}

#include "{{hdr_name_gen}}"

{{src}}
""")

    tpl_inl = ""


writers = [
    ChunkWriterSameFile,
    ChunkWriterGenFile,
    ChunkWriterStdOut
]


def resolve_writer(name):
    for w in writers:
        if w.name == name:
            return w
    msg = "{}: writer not found. Choose one of {}"
    msg = msg.format(name, ",".join(cl.name for cl in writers))
    raise Exception(msg)
    return None


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

class SourceFile:

    def __init__(self, filename=None):
        assert filename is not None and filename != ""
        self.filename = filename
        #
        spl = os.path.splitext(self.filename)
        #
        self.is_hdr = spl[1] == hdr_ext[1:]
        self.name_hdr = spl[0] + hdr_ext
        self.name_hdr_gen = spl[0] + '.gen' + hdr_ext
        #
        self.is_src = spl[1] == src_ext[1:]
        self.name_src = spl[0] + src_ext
        self.name_src_gen = spl[0] + '.gen' + src_ext

    def parse(self, parse_args=[]):
        self.trans_unit = clu.parse_file(self.filename, parse_args)

    def extract(self):
        tu = self.trans_unit
        self.enums = [Enum(e) for e in clu.find_macro_instantiations(tu, "C4_ENUM")]
        self.classes = [Class(c) for c in clu.find_macro_instantiations(tu, "C4_CLASS")]
        #self.props = [Prop(p) for p in find_macro_instantiations(tu, "C4_PROPERTY")]
        #for p in self.props:
        #    p.resolve_class(self.classes)

    def gen_code(self, writer, egen=None, cgens=[]):
        """
        :param writer: an instance of a chunk writer
        :param egen: an instance of an enum generator
        :param cgens: an iterable of class generators
        """
        self.gen_chunks(egen, cgens)
        writer.write(self, self.chunks)

    def gen_chunks(self, enum_generator=None, class_generators=[]):
        chunks = []
        if enum_generator:
            for e in self.enums:
                ch = enum_generator.gen_code(e)
                chunks.append(ch)
        if class_generators:
            for c in self.classes:
                for g in class_generators:
                    ch = g.gen_code(c)
                    chunks.append(ch)
        # sort the chunks by line (assume all from the same trans_unit)
        self.chunks = sorted(chunks, key=lambda ch: ch.originator.line)
        return self.chunks

    def write(self, writer):
        writer.write(self, self.chunks)

    def print_ast(self):
        clu.print_ast(self.trans_unit)

    def print_includes(self):
        clu.print_includes(self.trans_unit)


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

def run(writer, enum_generator=None, class_generators=None, in_args=None, clang_version=None):
    opts, args = handle_args(in_args, clang_version)
    #
    clu.load_clang(opts.clang_libdir)  #, opts.clang_version)
    #
    source_files = []
    for a in args:
        sf = SourceFile(a)
        sf.parse(opts.clang_args)
        source_files.append(sf)
    #
    if opts.writer:
        writer = resolve_writer(opts.writer)()
    if opts.show_writer_types:
        fieldlen = 0
        for w in writers:
            fieldlen = max(fieldlen, len(w.name))
        fieldlen += 4
        for w in writers:
            print(("{:"+str(fieldlen)+"} {}").format(w.name, w.__doc__))

    if opts.gen_code:
        for f in source_files:
            f.extract()
            f.gen_code(writer, enum_generator, class_generators)
    if opts.show_all or opts.show_hdr or opts.show_src:
        if opts.show_hdr and opts.show_src:
            opts.show_all = True
        outfiles = set()
        for f in source_files:
            f.extract()
            ch = f.gen_chunks(enum_generator, class_generators)
            if ch:
                for o in writer.outfiles(f):
                    if opts.show_all:
                        outfiles.add(o)
                    elif opts.show_hdr and is_hdr(o):
                        outfiles.add(o)
                    elif opts.show_src and is_src(o):
                        outfiles.add(o)
        for f in outfiles:
            print(f)
    if opts.show_ast:
        for f in source_files:
            f.print_ast()
    if opts.show_includes:
        for f in source_files:
            f.print_includes()


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

def handle_args(in_args=None, clang_version=None):
    from optparse import OptionParser, OptionGroup
    parser = OptionParser("usage: %prog [options] {filename(s)}")
    parser.disable_interspersed_args()  # https://docs.python.org/3/library/optparse.html#optparse.OptionParser.disable_interspersed_args
    #
    cmds = OptionGroup(parser, "Available commands")
    cmds.add_option("--gen-code", action="store_true", default=False,
                    help="generate source code (this is the default)")
    cmds.add_option("--show-hdr", action="store_true", default=False,
                    help="show header files to be generated or changed")
    cmds.add_option("--show-src", action="store_true", default=False,
                    help="show source files to be generated or changed")
    cmds.add_option("--show-all", action="store_true", default=False,
                    help="show header+source files to be generated or changed")
    cmds.add_option("--show-ast", action="store_true", default=False,
                    help="show the abstract syntax tree of the input file(s)")
    cmds.add_option("--show-includes", action="store_true", default=False,
                    help="show include tree of the input file(s)")
    parser.add_option_group(cmds)
    #
    wargs = OptionGroup(parser, "Fine-tuning")
    wargs.add_option("--writer", type=str, default=None,
                     help="""Override the writer type.
                     Available types: """ +
                     ",".join([cl.name for cl in writers]))
    wargs.add_option("--show-writer-types", action="store_true", default=False,
                     help="""Show quick help on the available writer types.""")
    #
    clo = OptionGroup(parser, "clang options")
    clo.add_option('-a', '--clang-args', default="",
                   help="arguments to be passed to clang, eg --clang-args='-std=c++11'")
    #clo.add_option("--clang-version", default=clang_version if clang_version else clu.version_default,
    #                help="specify a clang version number for finding the clang library. [default: %default]")
    #clo.add_option("--clang-version-fallback", default=",".join(clu.version_fallback),
    #                help="specify a sequence of comma-separated version numbers to fall back on if libclang is not found in the original clang-version. [default: %default]")
    clo.add_option("--clang-libdir", default=None,
                    help="directly specify the directory where libclang is located. This will bypass the clang-version lookup. [default: %default]")
    parser.add_option_group(clo)
    #
    if in_args:
        opts, args = parser.parse_args(in_args)
    else:
        opts, args = parser.parse_args()
        if len(args) == 0:
            parser.error('invalid number of arguments')
    #
    # FIXME setup mutually exclusive options
    if opts.show_all or opts.show_hdr or opts.show_src or opts.show_ast or opts.show_includes:
        opts.gen_code = False
    #
    opts.clang_args = util.splitesc_quoted(opts.clang_args, ' ')
    #
    return opts, args


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

def splitlines(s):
    """splits the string into a list of lines, retaining the newline character.
    s.split("\n") cannot be used because it removes the newline character."""
    assert isinstance(s, str)
    l = []
    b = 0
    for i,c in enumerate(s):
        if c == '\n':
            l.append(s[b:(i+1)])
            b = i+1
    if b != len(s):
        l.append(s[b:])
    return l


def pump_to_file(outfile, contents, mark, begin_tag=None, end_tag=None):
    f = open(outfile, "r")
    lines = f.readlines()
    f.close()
    outlines = pump(contents, lines, is_hdr(outfile), mark, begin_tag, end_tag)
    f = open(outfile, "w")
    f.writelines(outlines)
    f.close()


def pump(contents_to_insert, lines, lines_are_from_header, mark, begin_tag=None, end_tag=None):
    if isinstance(lines, str): lines = splitlines(lines)
    if not isinstance(contents_to_insert, str): contents_to_insert = "\n".join(contents_to_insert)
    if mark[-1] != "\n": mark = mark + "\n"
    if begin_tag is None: begin_tag = mark
    if end_tag is None: end_tag = mark
    if begin_tag[-1] != "\n": begin_tag += "\n"
    if end_tag[-1] != "\n": end_tag += "\n"
    #
    first_line, num_lines = 0, 0
    if mark != begin_tag:
        for i, l in enumerate(lines):
            if l == mark:
                first_line, num_lines = i, 1
                break
    else:
        assert begin_tag == end_tag
    if num_lines == 0:
        first_line, num_lines = lookup_line_range_unique(begin_tag, end_tag, lines)
        if num_lines == 0:
            if lines_are_from_header:
                # If it is a header, be sure to pump _inside_ the include guards.
                # (We're assuming that the file has include guards.)
                f, n = lookup_line_range_nested(["#ifndef", "#ifdef"], "#endif", lines)
                assert n > 0
                first_line, num_lines = (f+n-1), 0
            else:
                # if it's not a header, just append to the end
                first_line, num_lines = len(lines), 0
    # we're set
    if contents_to_insert != "" and contents_to_insert[-1] == "\n":
        ins = [begin_tag, contents_to_insert, end_tag]
    else:
        ins = [begin_tag, contents_to_insert, "\n", end_tag]
    outlines = (lines[0:first_line] + ins + lines[(first_line + num_lines):])
    return outlines


def lookup_line_range_unique(begin_tag, end_tag, lines):
    """
    assumes there's at most a single occurrence of a begin_tag...end_tag sequence
    """
    if begin_tag[-1] != "\n": begin_tag = begin_tag + "\n"
    if end_tag[-1] != "\n": end_tag = end_tag + "\n"
    if isinstance(lines, str): lines = splitlines(lines)
    on, first, num = False, 0, 0
    for i, l in enumerate(lines):
        if not on:
            if l == begin_tag or re.match(begin_tag, l):
                on = True
                first = i
        else:
            if l == end_tag or re.match(end_tag, l):
                on = False
                num = i+1 - first
                break
    if on and num == 0:
        num = 1
    return first, num


def lookup_line_range_nested(begin_tag, end_tag, lines):
    """can handle nested occurrences of begin_tag...end_tag sequences"""
    if begin_tag[-1] == "\n": begin_tag = begin_tag[0:-1]
    if end_tag[-1] == "\n": end_tag = end_tag[0:-1]
    if isinstance(begin_tag, list): begin_tag = "|".join(begin_tag)
    if isinstance(end_tag, list): end_tag = "|".join(end_tag)
    if isinstance(lines, str): lines = splitlines(lines)
    on,first,num = 0,-1,0
    for i,l in enumerate(lines):
        #print(i, '"'+l.strip("\n")+'"', begin_tag, end_tag)
        if re.match(begin_tag, l):
            if on == 0 and first == -1:
                first = i
            on += 1
        elif on > 0 and re.match(end_tag, l):
            on -= 1
            if on == 0:
                num = i+1 - first
                break
    if first == -1: first = 0
    return first, num
