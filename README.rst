regen
=====

|license|

`regen` is a python3 package providing C/C++ reflection and source-code
generation. It uses the AST provided by LLVM's libclang, which is processed
by user-provided source-code generators to automatically generate C/C++
source code. This is done via a `template engine (jinja2)
<http://jinja.pocoo.org/`_ for generating the code. Examples of application
are maintenance-free enum strings, object-tree serialization or property
systems with arbitrary per-property annotations.

This is a very fresh pre-alpha project. It is buggy and its interface will change.

Quick examples
--------------

Enum stringification
^^^^^^^^^^^^^^^^^^^^

Consider a C++ header, let's name it `myenum.h`::

    #pragma once

    C4_ENUM()
    typedef enum {
      FOO,
      BAR,
      BAZ
    } MyEnum;

Now use the following generation code which specifies the templates, saved as
`regen.py`::

    import c4.regen as regen

    egen = regen.EnumGenerator(
        # extract enums tagged with this macro
        tag="C4_ENUM",
        # header preamble
        hdrp="""\
    #include "enum_pairs.h"
    """
        # template for code in header files
        hdr="""\
    template<> const EnumPairs< {{enum.type}} > enum_pairs();
    """,
        # template for code in source files
        src="""\
    template<> const EnumPairs< {{enum.type}} > enum_pairs()
    {
        static const EnumAndName< {{enum.type}} > vals[] = {
            {% for e in enum.symbols %}
            { {{e.name}}, "{{e.name}}"},
            {% endfor %}
        };
        EnumPairs< {{enum.type}} > r(vals);
        return r;
    }
    """
    )
    writer = regen.ChunkWriterGenFile()

    #------------------------------------------------------------------------------
    if __name__ == "__main__":
        regen.run(writer, egen, [])

Now generate the code::

    python regen.py myenum.h

The command above generates `myenum.gen.h`...::

    #ifndef _MYENUM_GEN_H_
    #define _MYENUM_GEN_H_

    #include "enum_pairs.h"
    #include "myenum.h"

    template<> const EnumPairs< MyEnum > enum_pairs();
    #endif // _MYENUM_GEN_H_

and `myenum.gen.cpp`::

    #include "myenum.gen.h"

    template<> const EnumPairs< MyEnum > enum_pairs()
    {
        static const EnumAndName< MyEnum > vals[] = {
            { FOO, "FOO"},
            { BAR, "BAR"},
            { BAZ, "BAZ"},
            { BOD, "BOD"},
        };
        EnumPairs< MyEnum > r(vals);
        return r;
    }


Running
-------

`regen` uses `libclang-py3 <https://pypi.python.org/pypi/libclang-py3>`_,
which is a python wrapper for the libclang library. The current version of
libclang-py3 requires libclang 3.9. You may need to alter ``LD_LIBRARY_PATH``
so that libclang can be found. For example::

    LD_LIBRARY_PATH=$(llvm-config-3.9 --libdir) python regen.py myenum.h

(This version dependency needs to be fixed).


Installing
----------

From PyPi
^^^^^^^^^

Installation is easy with the Python package repository. This will install
pip along with its dependencies::
    pip install regen

From source
^^^^^^^^^^^
To install from source::
    git clone https://github.com/biojppm/regen.git
    cd regen
    pip install .

For development
^^^^^^^^^^^^^^^
Setting up regen for development is easy::
    git clone https://github.com/biojppm/regen.git
    cd regen
    pip install -r requirements_dev.txt
    pip install -e .

License
-------
cmany is permissively licensed under the `MIT license`_.

.. _MIT license: LICENSE.txt

.. |license| image:: https://img.shields.io/badge/License-MIT-yellow.svg
   :alt: License: MIT
   :target: https://opensource.org/licenses/MIT

.. http://stackoverflow.com/questions/10870719/inline-code-highlighting-in-restructuredtext
