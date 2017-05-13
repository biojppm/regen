import c4.regen as regen
import unittest as ut
import re


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
class Test0FilenameUtils(ut.TestCase):

    def test_ext(self):
        self.assertEqual(regen.ext('filename.hpp'), '.hpp')
        self.assertEqual(regen.ext('filename.bla.hpp'), '.hpp')

    def test_is_hdr_src(self):
        for e in ('.h','.hpp','.hxx','.hh','.H','.h++'):
            self.assertTrue(regen.is_hdr('filename' + e))
            self.assertFalse(regen.is_src('filename' + e))
        for e in ('.c','.cpp','.cxx','.cc','.C','.c++'):
            self.assertFalse(regen.is_hdr('filename' + e))
            self.assertTrue(regen.is_src('filename' + e))

    def test_inc_guard(self):
        self.assertEqual(regen.inc_guard('filename.hpp'), '_FILENAME_HPP_')
        self.assertEqual(regen.inc_guard('path/to/filename.hpp'), '_PATH_TO_FILENAME_HPP_')
        self.assertEqual(regen.inc_guard('path\\to\\filename.hpp'), '_PATH_TO_FILENAME_HPP_')


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
class Test1Splitlines(ut.TestCase):

    def test(self):
        self.assertEqual(regen.splitlines("line1"), ["line1"])
        self.assertEqual(regen.splitlines("line1\n"), ["line1\n"])
        self.assertEqual(regen.splitlines("line1\n\n"), ["line1\n", "\n"])
        self.assertEqual(regen.splitlines("line1\n\n\n"), ["line1\n", "\n", "\n"])
        self.assertEqual(regen.splitlines("line1\n\n\n\n"), ["line1\n", "\n", "\n", "\n"])
        self.assertEqual(regen.splitlines("line1\nline2"), ["line1\n", "line2"])
        self.assertEqual(regen.splitlines("line1\nline2\n"), ["line1\n", "line2\n"])
        self.assertEqual(regen.splitlines("line1\n\nline2\n"), ["line1\n", "\n", "line2\n"])
        self.assertEqual(regen.splitlines("line1\n\nline2\n\n"), ["line1\n", "\n", "line2\n", "\n"])


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
class Test2Pump(ut.TestCase):

    def test0_unique(self):
        c = PumpTestCases
        t = c.tag
        bt = c.btag
        et = c.etag
        ct = c.content
        #
        self._t(c.in0,   c.out0 , ct, t, t, 0, 0, False)
        self._t(c.in1b,  c.out1b, ct, t, t, 0, 1, False)
        self._t(c.in1,   c.out1 , ct, t, t, 1, 1, False)
        self._t(c.in1e,  c.out1e, ct, t, t, 2, 1, False)
        self._t(c.in2b,  c.out2b, ct, t, t, 0, 2, False)
        self._t(c.in2,   c.out2 , ct, t, t, 1, 2, False)
        self._t(c.in2e,  c.out2e, ct, t, t, 2, 2, False)
        #
        self._t(c.in2db, c.out2db, ct, bt, et, 0, 2, False)
        self._t(c.in2d,  c.out2d , ct, bt, et, 1, 2, False)
        self._t(c.in2de, c.out2de, ct, bt, et, 2, 2, False)

    def test1_nested(self):
        c = PumpTestCases
        t = c.tag
        bt = c.btag
        et = c.etag
        ct = c.content
        #
        self._t(c.nested_in0,   c.nested_out0 , ct, t, t, 0, 0, True)
        self._t(c.nested_in1b,  c.nested_out1b, ct, t, t, 2, 1, True)
        self._t(c.nested_in1e,  c.nested_out1e, ct, t, t, 8, 1, True)
        self._t(c.nested_in2b,  c.nested_out2b, ct, t, t, 2, 2, True)
        self._t(c.nested_in2e,  c.nested_out2e, ct, t, t, 8, 2, True)
        self._t(c.nested_in2db, c.nested_out2db, ct, bt, et, 2, 2, True)
        self._t(c.nested_in2de, c.nested_out2de, ct, bt, et, 8, 2, True)

    def _t(self, which, which_out, ct, bt, et, fval, nval, is_header):
        f, n = regen.lookup_line_range_unique(bt, et, which)
        self.assertEqual(f, fval)
        self.assertEqual(n, nval)
        #
        f, n = regen.lookup_line_range_unique(bt + "\n", et + "\n", which)
        self.assertEqual(f, fval)
        self.assertEqual(n, nval)
        #
        outlines = regen.pump(PumpTestCases.content, which, is_header, PumpTestCases.tag, bt, et)
        outlines = "".join(outlines)
        assert bt[-1] != "\n" and et[-1] != "\n"
        bt += "\n"
        et += "\n"
        reflines = re.sub(r'____content____\n', bt + ct + et, which_out)

        # print("\n")
        # print(">>>>>>\n", outlines, "<<<<<<", sep="")
        # print(">>>>>>\n", reflines, "<<<<<<", sep="")
        self.assertEqual(outlines, reflines)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

class PumpTestCases:

    content = ("""\
This is a multi-line thing.
If you don't like it, well,
you can do something else with it.
""")

    tag = "// tag"

    # input with no tags
    in0 = ("""\
line1
line2
""")
    out0 = ("""\
line1
line2
____content____
""")
    nested_in0 = ("""\
#ifndef BLA
#define BLA
#   ifndef FOO
#       ifdef BAR
#       endif
#   endif
#   ifdef BAZ
#   endif
#endif // !BLA
""")
    nested_out0 = ("""\
#ifndef BLA
#define BLA
#   ifndef FOO
#       ifdef BAR
#       endif
#   endif
#   ifdef BAZ
#   endif
____content____
#endif // !BLA
""")

    # input only with single tag
    in1 = ("""\
line1
// tag
line2
""")
    out1 = ("""\
line1
____content____
line2
""")
    in1b = ("""\
// tag
line2
line3
""")
    out1b = ("""\
____content____
line2
line3
""")
    in1e = ("""\
line1
line2
// tag
""")
    out1e = ("""\
line1
line2
____content____
""")
    nested_in1b = ("""\
#ifndef BLA
#define BLA
// tag
#   ifndef FOO
#       ifdef BAR
#       endif
#   endif
#   ifdef BAZ
#   endif
#endif // !BLA
""")
    nested_out1b = ("""\
#ifndef BLA
#define BLA
____content____
#   ifndef FOO
#       ifdef BAR
#       endif
#   endif
#   ifdef BAZ
#   endif
#endif // !BLA
""")
    nested_in1e = ("""\
#ifndef BLA
#define BLA
#   ifndef FOO
#       ifdef BAR
#       endif
#   endif
#   ifdef BAZ
#   endif
// tag
#endif // !BLA
""")
    nested_out1e = ("""\
#ifndef BLA
#define BLA
#   ifndef FOO
#       ifdef BAR
#       endif
#   endif
#   ifdef BAZ
#   endif
____content____
#endif // !BLA
""")

    # input with begin+end tag
    in2 = ("""\
line1
// tag
// tag
line4
""")
    out2 = ("""\
line1
____content____
line4
""")
    in2b = ("""\
// tag
// tag
line3
line4
""")
    out2b = ("""\
____content____
line3
line4
""")
    in2e = ("""\
line1
line2
// tag
// tag
""")
    out2e = ("""\
line1
line2
____content____
""")
    nested_in2b = ("""\
#ifndef BLA
#define BLA
// tag
// tag
#   ifndef FOO
#       ifdef BAR
#       endif
#   endif
#   ifdef BAZ
#   endif
#endif // !BLA
""")
    nested_out2b = ("""\
#ifndef BLA
#define BLA
____content____
#   ifndef FOO
#       ifdef BAR
#       endif
#   endif
#   ifdef BAZ
#   endif
#endif // !BLA
""")
    nested_in2e = ("""\
#ifndef BLA
#define BLA
#   ifndef FOO
#       ifdef BAR
#       endif
#   endif
#   ifdef BAZ
#   endif
// tag
// tag
#endif // !BLA
""")
    nested_out2e = ("""\
#ifndef BLA
#define BLA
#   ifndef FOO
#       ifdef BAR
#       endif
#   endif
#   ifdef BAZ
#   endif
____content____
#endif // !BLA
""")

    btag = tag + ": BEGIN"
    etag = tag + ": END"
    # input with different begin+end tag
    in2d = ("""\
line1
// tag: BEGIN
// tag: END
line3
""")
    out2d = ("""\
line1
____content____
line3
""")
    in2db = (
"""// tag: BEGIN
// tag: END
line3
line4
""")
    out2db = ("""\
____content____
line3
line4
""")
    in2de = ("""\
line1
line2
// tag: BEGIN
// tag: END
""")
    out2de = ("""\
line1
line2
____content____
""")
    nested_in2db = ("""\
#ifndef BLA
#define BLA
// tag: BEGIN
// tag: END
#   ifndef FOO
#       ifdef BAR
#       endif
#   endif
#   ifdef BAZ
#   endif
#endif // !BLA
""")
    nested_out2db = ("""\
#ifndef BLA
#define BLA
____content____
#   ifndef FOO
#       ifdef BAR
#       endif
#   endif
#   ifdef BAZ
#   endif
#endif // !BLA
""")
    nested_in2de = ("""\
#ifndef BLA
#define BLA
#   ifndef FOO
#       ifdef BAR
#       endif
#   endif
#   ifdef BAZ
#   endif
// tag: BEGIN
// tag: END
#endif // !BLA
""")
    nested_out2de = (
"""#ifndef BLA
#define BLA
#   ifndef FOO
#       ifdef BAR
#       endif
#   endif
#   ifdef BAZ
#   endif
____content____
#endif // !BLA
""")


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
if __name__ == '__main__':
    ut.main()
