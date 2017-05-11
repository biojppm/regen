#!/usr/bin/env python
# -*- coding: utf-8 -*-

from setuptools import setup, find_packages
import sys
import os


if sys.version_info < (3, 0):
    msg = 'regen requires at least Python 3.0. Current version is {}. Sorry.'
    sys.exit(msg.format(sys.version_info))


def readreqs(*rnames):
    def _skipcomment(line):
        return line if (line and not line.startswith('--')
                        and not line.startswith('#')) else ""
    with open(os.path.join(os.path.dirname(__file__), *rnames)) as f:
        l = [_skipcomment(line.strip()) for line in f]
    return l


setup(name="regen",
      version="0.1.0",
      description="C/C++ source code reflection and code generation",
      url="https://github.com/biojppm/regen",
      download_url="https://github.com/biojppm/regen",
      license="License :: OSI Approved :: MIT License",
      classifiers=[
          "License :: OSI Approved :: MIT License",
          "Intended Audience :: Developers",
          "Development Status :: 2 - Pre-Alpha",
          "Programming Language :: Python :: 3",
          "Programming Language :: C",
          "Programming Language :: C++",
          "Topic :: Software Development :: Code Generators"
          "Topic :: Software Development :: Libraries :: Python Modules",
          "Topic :: Utilities",
      ],
      keywords=["c", "c++", "reflection", "code-generation", "libclang"],
      author="Joao Paulo Magalhaes",
      author_email="dev@jpmag.me",
      zip_safe=False,
      namespace_packages=['c4'],
      packages=find_packages('src'),
      package_dir={'': 'src'},
      # entry_points={'console_scripts': ['cmany=c4.regen.main:regen_main'], },
      install_requires=readreqs('requirements.txt'),
      tests_require=readreqs('requirements_test.txt'),
)
