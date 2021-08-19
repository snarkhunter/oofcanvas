# -*- python -*-

# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modified
# versions of this software, you first contact the authors at
# oof_manager@nist.gov. 

# Tools for building OOF2 extensions.  These are *not* used for
# building OOF2 itself, because they rely on oof2config.py, which is
# created when OOF2 is built.

import oof2config         # must be imported before shlib and oof2setuputils
import oof2setuputils
import shlib                            # our own distutils command
import os.path

import sys

# Tell distutils that .C is a C++ file suffix
from distutils.ccompiler import CCompiler
CCompiler.language_map['.C'] = 'c++'

def _addlistarg(kwargs, name, listval):
    # Augment or set a list-valued keyword argument.
    try:
        kwargs[name].extend(listval)
    except KeyError:
        kwargs[name] = listval[:]

# Define an Extension class that automatically picks up all of the
# info from oof2config.

import distutils.core

class Extension(distutils.core.Extension):
    def __init__(self, name, sources, include_dirs, libraries, **kwargs):

        if 'language' not in kwargs.keys():
            kwargs['language'] = 'c++'

        # include_dirs and libraries are *required* here, although
        # they're optional for distutils.core.Extension.  That's
        # because an external OOF2 extension always needs to be able
        # to find its SharedLibrary.
        include_dirs = include_dirs + oof2config.include_dirs
        libraries = libraries + oof2config.libraries

        _addlistarg(kwargs, 'extra_compile_args', oof2config.extra_compile_args)
        _addlistarg(kwargs, 'library_dirs', oof2config.library_dirs)
        _addlistarg(kwargs, 'extra_link_args', oof2config.extra_link_args)

        distutils.core.Extension.__init__(self, name, sources,
                                          include_dirs=include_dirs,
                                          libraries=libraries,
                                          **kwargs)

# Define a SharedLibrary class that automatically picks up all of the
# info from oof2config.

from shlib import build_shlib

class SharedLibrary(build_shlib.SharedLibrary):
    def __init__(self, name, sources, language='c++', **kwargs):
##        if 'language' not in kwargs.keys():
##            kwargs['language'] = 'c++'
        _addlistarg(kwargs, 'include_dirs', oof2config.include_dirs)
        _addlistarg(kwargs, 'libraries', oof2config.libraries)
        _addlistarg(kwargs, 'library_dirs', oof2config.library_dirs)

        build_shlib.SharedLibrary.__init__(self, name, sources, **kwargs)

# Redefine run_swig so that it automatically picks up information from
# oof2config.

def run_swig(srcdir, swigfile, destdir,
             cext="_wrap.C", include_dirs=[], dry_run=False, force=False,
             with_swig=None):
    # TODO: There's a problem with extensions, they need access
    # to SWIG, but the build process builds SWIG in the build-temp
    # directory, to accommodate different architectures in the
    # same build tree.  Extensions do have access to the "root"
    # of the source tree, through oofcconfig.py, maybe they all
    # just need to build their own swig executables, the way
    # the main setup.py does?
    return oof2setuputils.run_swig(srcdir, swigfile, destdir, cext,
                                   include_dirs + oof2config.swig_include,
                                   dry_run, force, with_swig)



# Combination utility function that runs swig *and* creates an
# Extension object.  kwargs can contain any arguments allowed by Extension.

def get_swig_ext(srcdir, srcfile, destdir, cext="_wrap.C", include_dirs=[],
                 libraries=[], dry_run=False, force=False,
                 with_swig=None, **kwargs):

    print >> sys.stderr, "*** OOF2extutils.get_swig_ext ***"
    print >> sys.stderr, oof2config.root
    
    swigstatus = run_swig(srcdir, srcfile, destdir, cext, include_dirs,
                          dry_run, force, with_swig)

    # convert the swig output directory to a package name
    outdir = swigstatus['outdir']       # is an abspath
    here = os.path.abspath('.')
    # This fails if here=="/".  Let's not worry about that case.
    pkgpath =  outdir[len(here)+1:]
    if pkgpath[-1] == os.path.sep:
        pkgpath = pkgpath[:-1]
    pkgpath = pkgpath.replace(os.path.sep, '.')

    # return an Extension object and the Python package name
    return (
        Extension(name = swigstatus['basename']+'c',
                  sources = [swigstatus['cfile']],
                  include_dirs = [swigstatus['indir']] + include_dirs,
                  libraries = libraries,
                  **kwargs
                  ),
        pkgpath
        )
           
