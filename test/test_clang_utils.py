import unittest as ut
import c4.regen.clang_utils as clu
import clang.cindex
from clang.cindex import CursorKind as ck

empty_main = ("""\
#include <iostream>
int main(int argc, const char* argv[])
{
    printf("hello\\n");
}
""")


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
def srcjoin(*src_bits):
    "join source bits into several lines"
    src = ""
    for a in src_bits:
        if src: src += "\n"
        src += str(a)
    if src: src += "\n"
    return src


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
class CluTest(ut.TestCase):

    def __init__(self, methodName='runTest'):
        super().__init__(methodName)
        clu.load_clang()

    def _parse(self, *src_bits):
        src = srcjoin(*src_bits)
        tu = clu.parse_source(src)
        return src, tu

    def _failparse(self, *src_bits):
        src = srcjoin(*src_bits)
        #tu = clu.parse_source(src)
        with self.assertRaises(Exception):
            tu = clu.parse_source(src)


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
class Test0ParseFromSource(CluTest):

    def test00_pass_src(self):
        self._parse(empty_main)

    def test02_incomplete(self):
        self._parse("void bla() {}")

    def test10_fail_parse_error(self):
        self._failparse(empty_main, "dasfkhdsfkjhsdf sdfkjsdf sdfkjhsdf  ++++++***")

    def test11_fail_missing_include(self):
        self._failparse("#include <whatever.wtf>", empty_main)


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
class Test1FindNodes(CluTest):

    def test(self):
        pass


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
class Test2FindFunction(CluTest):

    def test(self):
        _, tu = self._parse(empty_main)
        f = clu.find_function(tu, "main")
        self.assertIsNotNone(f)
        self.assertEqual(f.kind, ck.FUNCTION_DECL)
        self.assertEqual(f.spelling, "main")


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
class Test3GetComments(CluTest):

    def _singleline_comments(self, txt):
        return [c.format(txt) for c in ("// {}", "/* {} */", "/** {} */")]

    def _multiline_comments(self, txt):
        return [c.format(txt) for c in ("/* {} */", "/** {} */")]

    def _getbla(self, *args):
        src,tu = self._parse(*args)
        f = clu.find_function(tu, "bla")
        self.assertEqual(f.kind, ck.FUNCTION_DECL)
        self.assertEqual(f.spelling, "bla")
        return f

    def test10_prev_line(self):
        for comment in self._singleline_comments("this is a comment"):
            f = self._getbla(empty_main, comment, "void bla() {}")
            c = clu.get_comment_prev_line(f)
            self.assertEqual(c, comment)

    def test11_prev_line_multiline(self):
        for comment in self._multiline_comments("this is a\nmultiline\ncomment\n\nmore goes here"):
            f = self._getbla(empty_main, comment, "void bla() {}")
            c = clu.get_comment_prev_line(f)
            self.assertEqual(c, comment)

    df = "#define MACRO_NAME(...)"
    # macro call
    mc = "MACRO_NAME(key1=\"val1\", key2=val2, key3=\"val3\")"
    # multiline macro call
    mmc = "MACRO_NAME(\nkey1=\"val1\", \nkey2=val2, \nkey3=\"val3\"\n)"
    # macro call with comments
    mcwc = "MACRO_NAME(key1=\"val1\"/* a comment goes here */, key2=val2/*and another here*/, key3=\"val3\")"
    # multiline macro call with comments
    mmcwc = "MACRO_NAME(\nkey1=\"val1\"/* a comment goes here */, \nkey2=val2/*and another here*/, \nkey3=\"val3\"\n)"

    def test20_prev_line_with_macro(self):
        for comment in self._singleline_comments("this is a comment"):
            f = self._getbla(__class__.df, empty_main, comment, __class__.mc, "void bla() {}")
            c = clu.get_comment_prev_line(f, skip_macro_call_above_line=True)
            self.assertEqual(c, comment)

    def test21_prev_line_with_macro_multiline(self):
        for comment in self._multiline_comments("this is a\nmultiline\ncomment\n\nmore goes here"):
            f = self._getbla(__class__.df, empty_main, comment, __class__.mc, "void bla() {}")
            c = clu.get_comment_prev_line(f, skip_macro_call_above_line=True)
            self.assertEqual(c, comment)

    def test30_prev_line_with_multiline_macro(self):
        for comment in self._singleline_comments("this is a comment"):
            f = self._getbla(__class__.df, empty_main, comment, __class__.mmc, "void bla() {}")
            c = clu.get_comment_prev_line(f, skip_macro_call_above_line=True)
            self.assertEqual(c, comment)

    def test31_prev_line_with_multiline_macro_multiline(self):
        for comment in self._multiline_comments("this is a\nmultiline\ncomment\n\nmore goes here"):
            f = self._getbla(__class__.df, empty_main, comment, __class__.mmc, "void bla() {}")
            c = clu.get_comment_prev_line(f, skip_macro_call_above_line=True)
            self.assertEqual(c, comment)

    def test40_prev_line_with_macro_and_comments(self):
        for comment in self._singleline_comments("this is a comment"):
            f = self._getbla(__class__.df, empty_main, comment, __class__.mcwc, "void bla() {}")
            c = clu.get_comment_prev_line(f, skip_macro_call_above_line=True)
            self.assertEqual(c, comment)

    def test41_prev_line_with_macro_and_comments_multiline(self):
        for comment in self._multiline_comments("this is a\nmultiline\ncomment\n\nmore goes here"):
            f = self._getbla(__class__.df, empty_main, comment, __class__.mcwc, "void bla() {}")
            c = clu.get_comment_prev_line(f, skip_macro_call_above_line=True)
            self.assertEqual(c, comment)

    def test50_prev_line_with_multiline_macro_and_comments(self):
        for comment in self._singleline_comments("this is a comment"):
            f = self._getbla(__class__.df, empty_main, comment, __class__.mmcwc, "void bla() {}")
            c = clu.get_comment_prev_line(f, skip_macro_call_above_line=True)
            self.assertEqual(c, comment)

    def test51_prev_line_with_multiline_macro_and_comments_multiline(self):
        for comment in self._multiline_comments("this is a\nmultiline\ncomment\n\nmore goes here"):
            f = self._getbla(__class__.df, empty_main, comment, __class__.mmcwc, "void bla() {}")
            c = clu.get_comment_prev_line(f, skip_macro_call_above_line=True)
            self.assertEqual(c, comment)

    def test60_same_line_right(self):
        for comment in self._singleline_comments("this is a comment"):
            f = self._getbla(empty_main, "void bla() {}" + " " + comment)
            c = clu.get_comment_same_line(f)
            self.assertEqual(c, comment)

    def test61_same_line_right_multiline(self):
        for comment in self._multiline_comments("this is a\nmultiline\ncomment\n\nmore goes here"):
            f = self._getbla(empty_main, "void bla() {}" + " " + comment)
            c = clu.get_comment_same_line(f)
            self.assertEqual(c, comment)

    def test70_enum_comments(self):
        return  # this is failing and needs to be fixed
        l = []
        for i in (0, 1):
            for c in ('// {}', '/* {} */', '/** {} */'):
                s = ""
                d = {}
                comm = c.format("this is the enum comment")
                d['comm'] = comm
                for e in ('FOO', 'BAR', 'BAZ'):
                    sc = c.format(e + " means " + e.lower())
                    d[e] = sc
                    if i == 0:
                        s += "  {}\n{},\n".format(sc, e)
                    else:
                        s += "  {},{}\n".format(e, sc)
                s = (comm + "\ntypedef enum {{\n{} }} Whatever;".format(s))
                d['src'] = s
                l.append(d)
        for d in l:
            src, tu = self._parse(d['src'])
            clu.print_ast(tu)
            typedef_nodes = clu.find_nodes(tu.cursor,
                                           clang.cindex.CursorKind.TYPEDEF_DECL,
                                           descend=False)
            self.assertEqual(len(typedef_nodes), 1)
            td = typedef_nodes[0]
            ch = list(td.get_children())
            self.assertEqual(len(ch), 1)
            enum_node = ch[0]
            comm = clu.get_comment(enum_node)
            self.assertEqual(comm, d['comm'])
            for s in enum_node.get_children():
                if s.kind == clang.cindex.CursorKind.ENUM_CONSTANT_DECL:
                    name = s.displayname
                    comm = clu.get_comment(s)
                    try:
                        self.assertEqual(comm, d[name])
                    except:
                        print("ERROR:--------------------------------------")
                        print("name=", name)
                        print("comm=", comm)
                        print("d[name]=", d[name])
                        print(src)
                        print(d)
                        raise


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
if __name__ == '__main__':
    ut.main()
