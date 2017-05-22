
===========  ===========
 |pypi|       |license|
===========  ===========

regen
=====

`regen` is a python3 package providing C/C++ reflection and source-code
generation. It uses the AST provided by LLVM's libclang, which is processed
by user-provided source-code generators to automatically generate C/C++
source code. This is done via a `template engine (jinja2)
<http://jinja.pocoo.org/docs/2.9/templates/>`_ for generating the
code. Examples of application are maintenance-free enum strings, object-tree
serialization or property systems with arbitrary per-property annotations.

This is a very fresh pre-alpha project. It is buggy and its interface will
change.

This is a very fresh pre-alpha project. It is buggy and its interface will change.

Quick examples
--------------

Enum stringification
^^^^^^^^^^^^^^^^^^^^

Consider a C++ header, let's name it `myenum.h`:

.. code:: c++

    #pragma once

    C4_ENUM()
    typedef enum {
        FOO,
        BAR,
        BAZ
    } MyEnum;

Now use the following python code for parsing and generating, saved as
`regen.py`:

.. code:: python

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

Now generate the code:

.. code:: bash

    python regen.py myenum.h

The command above generates `myenum.gen.h`:

.. code:: c++

    #ifndef _MYENUM_GEN_H_
    #define _MYENUM_GEN_H_

    #include "enum_pairs.h"
    #include "myenum.h"

    template<> const EnumPairs< MyEnum > enum_pairs();
    #endif // _MYENUM_GEN_H_

and also `myenum.gen.cpp`:

.. code:: c++

    #include "myenum.gen.h"

    template<> const EnumPairs< MyEnum > enum_pairs()
    {
        static const EnumAndName< MyEnum > vals[] = {
            { FOO, "FOO"},
            { BAR, "BAR"},
            { BAZ, "BAZ"},
        };
        EnumPairs< MyEnum > r(vals);
        return r;
    }


Running
-------

Finding libclang
^^^^^^^^^^^^^^^^
`regen` uses `libclang-py3 <https://pypi.python.org/pypi/libclang-py3>`_,
which is a python wrapper for the libclang library. The current version of
libclang-py3 requires libclang 3.8. regen tries to find libclang 3.8 by
querying ``llvm-config --libdir`` (if ``llvm-config --version`` reports 3.8)
or ``llvm-config-3.8 --libdir`` if the first fails. If this also fails, then
you can still use the option ``--clang-libdir``.

(This version dependency needs to be fixed; this will probably be done by
using different branches).


Installing
----------

From PyPi
^^^^^^^^^

Installation is easy with the Python package repository. This will install
pip along with its dependencies::

    pip install regen

From source
^^^^^^^^^^^
.. code:: bash

    git clone https://github.com/biojppm/regen.git
    cd regen
    pip install .

For development
^^^^^^^^^^^^^^^

Setting up regen for development is easy:

.. code:: bash

    git clone https://github.com/biojppm/regen.git
    cd regen
    pip install -r requirements_dev.txt
    pip install -e .

***Windows notes***. For easier understanding (and less development work),
the examples rely extensively on symbolic link files. This works as expected
in Unix and Mac, but symbolic links were only recently introduced in
Windows. Git already allows you to use symbolic links in Windows, but the
process is convoluted. You must first `enable symlinks in windows
<https://github.com/git-for-windows/git/wiki/Symbolic-Links>`_, and you need
to pass an option to clone, such that the files are really symbolic
links. The clone command thus needs to be ``git clone -c core.symlinks=true
https://github.com/biojppm/regen.git``.


License
-------
cmany is permissively licensed under the `MIT license`_.

.. _MIT license: LICENSE.txt

.. |pypi| image:: https://img.shields.io/pypi/v/regen.svg
      :alt: Version
      :target: https://pypi.python.org/pypi/regen/

.. |license| image:: https://img.shields.io/badge/License-MIT-yellow.svg
   :alt: License: MIT
   :target: https://opensource.org/licenses/MIT
