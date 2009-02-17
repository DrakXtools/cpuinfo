#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Python Bindings for CPUInfo
#
# Copyright (C) 2009 Per Øyvind Karlsen <peroyvind@mandriva.org>
# CPUInfo Copyright (C) 2007-2008  Gwenole Beauchesne
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
import sys, os, subprocess
from warnings import warn
from setuptools import setup, Extension

descr = "Python bindings for CPUInfo"
long_descr = """PyCpuinfo provides a python interface for the cpuinfo library"""
version = '0.1'
version_define = [('VERSION', '"%s"' % version)]

c_files = ['CPUInfo.c']

compile_args = ['-std=gnu99']
warnflags = ['-Wall', '-Wextra', '-pedantic']
compile_args.extend(warnflags)
link_args = ['-lcpuinfo']

extens=[Extension('CPUInfo', c_files, extra_compile_args=compile_args, extra_link_args=link_args, define_macros=version_define)]

setup(
    name = "pycpuinfo",
    version = version,
    description = descr,
    author = "Per Øyvind Karlsen",
    author_email = "peroyvind@mandriva.org",
    url = "https://proyvind.net",
    license = 'GPL 2',
    keywords = "cpu info",
    long_description = long_descr,
    platforms = sys.platform,
    classifiers = [
        'Development Status :: 4 - Beta',
        'Programming Language :: Python',
        'Topic :: Software Development :: Libraries :: Python Modules',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: GNU General Public License (GPL)',
        'Operating System :: POSIX'
    ],
    ext_modules = extens,
)

sys.exit(0)
