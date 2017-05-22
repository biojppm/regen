
===========  ===========
 |pypi|       |license|
===========  ===========

regen
=====

`regen` is a python3 package providing C/C++ reflection and source-code
generation. You provide `your own code generation templates
<http://jinja.pocoo.org/docs/2.9/templates/>`_ , and have full control over
where the generated code goes. A flexible system of annotations is used, so
that you can pass meta-values to your code generation templates.

`regen` is ideal to be used as a pre-build step for your C/C++ project, but
it can also be used separately on a file by file basis. Examples of
application are object-tree iteration utilities, or property systems with
arbitrary per-property annotations, or maintenance-free enum strings.

This is a very fresh pre-alpha project. It is buggy and its interface will
change.

How it works
------------

`regen` receives two inputs: your C/C++ source code and the code generation
templates. The code generation templates are written for Python's `jinja2
template engine <http://jinja.pocoo.org/docs/2.9/templates/>`_. `regen` uses
`libclang <http://clang.llvm.org/>`_ to parse your C/C++ code and produce an
Abstract Syntax Tree (AST). This tree is processed to extract the
features, which are then passed to each generator.

You can see `regen` being used in the project's `examples folder`_.


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

libclang on windows
^^^^^^^^^^^^^^^^^^^

`libclang is hard to use on windows
<https://www.reddit.com/r/cpp/comments/50x2ee/how_to_get_clang_to_work/>`_,
but it is useable.
While its rough edges are rounded out by the clang developers, we need to
deal with its windows problems:

* The official installer for version 3.8.1 on the LLVM site `is broken with
  VS2015 Update 3
  <http://lists.llvm.org/pipermail/cfe-dev/2016-June/049748.html>`_, so it
  won't work out of the box when the C++ library is used.
* clang 3.9.1 needs to be run with the Visual Studio developer environment,
  or it will cause a linker error (no kernel32).

To deal with this, ``regen`` has a `clang build project`_, which downloads
the clang source, patches it as needed, and installs it. You can use it like
this:

.. code:: bash

    cd regen/tools/clang-build
    mkdir build
    cd build
    cmake -DCLANG_VERSION=3.8.1 ..
    cmake --build --config Release .

You can compile several versions at once. For example, to compile versions
3.8.1, 3.9.1 and 4.0.0, you can configure with this command instead:
``cmake -DCLANG_VERSION="3.8.1;3.9.1;4.0.0" ..``.

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

***Windows notes***. The examples rely extensively on symbolic link
files. This works as expected in Unix and Mac, but symbolic links were only
recently introduced in Windows. Git already allows you to use symbolic links
in Windows, but the process is convoluted. Before cloning the repo, you must
first `enable symlinks in windows
<https://github.com/git-for-windows/git/wiki/Symbolic-Links>`_. Then you
need to pass an option to clone to ensure that the files are really symbolic
links. The clone command thus needs to be::

.. code:: bash
   git clone -c core.symlinks=true https://github.com/biojppm/regen.git


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

.. _examples folder: examples
.. _clang build project: tools/clang-build/CMakeLists.txt
