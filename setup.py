# -*- python -*-

# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modified
# versions of this software, you first contact the authors at
# oof_manager@nist.gov. 

## Usage:

#  In the top oofcanvas directory (the one containing this file) type
#  something like this:
#    python setup.py install     # installs in the default location
#    python setup.py install --prefix=/some/other/place
#    python setup.py [build [--debug]] install --prefix ...

###############################

# Required version numbers of required external libraries.  These
# aren't used explicitly in this file, but they are used in the DIR.py
# files that are execfile'd here.

GTK_VERSION = "3.22.0"
MAGICK_VERSION = "6.0"
CAIROMM_VERSION = "1.12" # Don't know what the earliest acceptable version is.
PANGO_VERSION = "1.40"
PANGOCAIRO_VERSION = "1.40"
PYGOBJECT_VERSION = "3.26"

# The make_dist script edits the following line when a distribution is
# built.  Don't change it by hand.  On the release branch,
# "(unreleased)" is replaced by the version number.
# TODO GTK3: Make that happen.  Temporarily set to 0.0.0 for testing.
version_from_make_dist = "0.0.0"  #"(unreleased)"

###############################

# TODO

# Despite using os.path.join and os.path.relpath and other portable
# path manipulations, there are many places in which this script
# assumes that the path separator is "/".  That should be fixed if
# this is ever ported to a non-unix system.

###############################

import distutils.core
from distutils.command import build
from distutils.command import build_ext
from distutils.command import build_py
from distutils.command import clean
from distutils.command import build_scripts
from distutils.command import install_data
from distutils import errors
from distutils import log
from distutils.dir_util import remove_tree, mkpath
from distutils.util import convert_path
from distutils.sysconfig import get_config_var

log.set_verbosity(2)

## oof2installlib redefines the distutils install_lib command so that
## it runs install_name_tool on Macs.  This doesn't seem to be
## necessary.  If library files can't be found at run time, try
## reinstating oof2installlib by uncommenting it here and where it's
## used, below.
import oof2installlib

import shlib # adds build_shlib and install_shlib to the distutils command set
from shlib import build_shlib

from oof2setuputils import run_swig, find_file, extend_path

import os
import shlex
import stat
import string
import sys
import subprocess
import tempfile
import time
from types import *

# Tell distutils that .C is a C++ file suffix.
from distutils.ccompiler import CCompiler
CCompiler.language_map['.C'] = 'c++'

# TODO GTK3: Why are these defined here, but SWIGDIR, etc are not?
# They should all be in one place.
DIRFILE = "DIR.py"                      # subdirectory manifest files
SWIGCFILEEXT = 'cmodule.C'              # suffix for swig output files
SRCDIR = "oofcanvas"                    # top source directory

##############

# readDIRs() walks through the source directory looking for
# DIR.py files, reads the files, and fills in the CLibInfo objects.
# CLibInfo categorize all of the source files except for the python
# files for pure python modules.

# DIR.py files contain the following python variables.  All are optional.

# dirname: The name of the directory. It's actually not used.

# clib: the name of the library to be built from the C and C++ files
# in the directory.  The library will be called lib<name>.<suffix>,
# where suffix is system dependent.  More than one DIR.py file can use
# the same name.

# cfiles: a list of the names of all of the C and C++ files in the
# directory that need to be compiled to form lib<name>.

# hfiles: a list of the names of the header files that go with the C
# and C++ files.

# swigfiles: a list of the names of the swig input files in the
# directory.  Swig-generated C++ code will be compiled but *not*
# included in lib<name>.  Each swig input file will create a separate
# python-loadable module which will *link* to lib<name>.

# swigpyfiles: a list of the names of python files that are included
# in swig output files.

# clib_order: an integer specifying the order in which the libraries
# must be built.  Later libraries may link to earlier ones.  This
# linking is done by setting clib.externalLibs in the set_clib_flags
# function in a DIR.py file.

# set_clib_flags: a function which may be called to set compilation
# and linker flags for building the library.  Its argument is a
# CLibInfo object.  The includeDirs, externalLibDirs, and externalLibs
# members of the object may be modified by set_clib_flags.

# subdirs: a list of subdirectories that should be processed.  Each
# subdirectory must have its own DIR.py file.

allCLibs = {}
purepyfiles = []

def getCLibInfo(name):
    try:
        return allCLibs[name]
    except KeyError:
        clib = allCLibs[name] = CLibInfo(name)
        return clib
                
class CLibInfo:
    def __init__(self, name):
        allCLibs[name] = self
        self.libname = name
        self.dirdata = {'cfiles': [],   # *.[Cc] -- c and c++ source code
                        'hfiles': [],   # *.h    -- c and c++ header files
                        'swigfiles': [], # *.swg  -- swig source code
                        'swigpyfiles': [], # *.spy  -- python included in swig
                        }
        self.pkgs = set()          # packages to run pkg-config on
        self.externalLibs = []
        self.externalLibDirs = []

        # cwd is put into the include path here because SRCDIR is
        # 'oofcanvas' and in the C++ code the include statements are
        # in the form #include "oofcanvas/xyz.h" The same header files
        # are installed in "DESTDIR/oofcanvas/xyz.h", and users will
        # include them using the *same* "#include "oofcanvas/xyz.h".
        # This lets use use the header files unchanged when building
        # oofcanvas and when using it. 
        self.includeDirs = [os.getcwd()]
        
        self.extra_link_args = []
        self.extra_compile_args = []
        self.extensionObjs = None
        self.ordering = None

    def add_pkg(self, pkg):
        self.pkgs.add(pkg)

    def run_pkg_config(self):
        # Running pkg-config on all of the packages at the same time
        # reduces redundancy in the resulting compiler argument
        # list. This is called after all of the DIR.py files have been
        # read, so that self.pkgs contains all of the third party
        # packages that will be used.
        if not self.pkgs:
            return
        # Run pkg-config --cflags.
        cmd = "pkg-config --cflags %s" % string.join(self.pkgs)
        log.info("%s: %s", self.libname, cmd)
        f = os.popen(cmd, 'r')
        for line in f.readlines():
            for flag in line.split():
                if flag[:2] == '-I':
                    self.includeDirs.append(flag[2:])
                else:
                    self.extra_compile_args.append(flag)
        # Run pkg-config --libs.
        cmd = "pkg-config --libs %s" % string.join(self.pkgs)
        log.info("%s: %s", self.libname, cmd)
        f = os.popen(cmd, 'r')
        for line in f.readlines():
            for flag in line.split():
                if flag[:2] == '-l':
                    self.externalLibs.append(flag[2:])
                elif flag[:2] == '-L':
                    self.externalLibDirs.append(flag[2:])
                else:
                    self.extra_link_args.append(flag)

    # Parse the file lists in a DIR.py file.  The file has been read
    # already, and its key,list pairs are in dirdict.  Only the data
    # relevant to CLibInfo is dealt with here.  The rest is handled
    # by readDIRs().
    def extractData(self, srcdir, dirdict):
        for key in self.dirdata.keys():
            try:
                value = dirdict[key]
                del dirdict[key]
            except KeyError:
                pass
            else:
                for filename in value:
                    self.dirdata[key].append(os.path.join(srcdir, filename))
        try:
            flagFunc = dirdict['set_clib_flags']
            del dirdict['set_clib_flags']
        except KeyError:
            pass
        else:
            flagFunc(self)

            
        try:
            self.ordering = dirdict['clib_order']
            del dirdict['clib_order']
        except KeyError:
            pass

    def get_extensions(self):
        if self.extensionObjs is None:
            self.extensionObjs = []
            for swigfile in self.dirdata['swigfiles']:
                # The file name is of the form "./SRCDIR/dirs/something.swg".
                # Strip the "./SRCDIR" and the suffix.
                basename = os.path.splitext(
                    os.path.relpath(swigfile, SRCDIR))[0]
                modulename = os.path.splitext(basename + SWIGCFILEEXT)[0]
                sourcename = os.path.join(swigroot, basename+SWIGCFILEEXT)
                
                extension = distutils.core.Extension(
                    # name = os.path.join(PROGNAME,"oofcanvaslib", SWIGINSTALLDIR,
                    #                     modulename),
                    name = os.path.join(PROGNAME, modulename),
                    language = 'c++',
                    sources = [sourcename],
                    define_macros = platform['macros'],
                    extra_compile_args = self.extra_compile_args + \
                        platform['extra_compile_args'],
                    include_dirs = self.includeDirs + platform['incdirs'],
                    library_dirs = self.externalLibDirs + platform['libdirs'],
                    libraries = [fixLibName(self.libname)] + self.externalLibs,
                                                        # + platform['libs'],
                    extra_link_args = self.extra_link_args + \
                        platform['extra_link_args']
                    )

                self.extensionObjs.append(extension)
        return self.extensionObjs

    def get_shlib(self):
        if self.dirdata['cfiles']:
            return build_shlib.SharedLibrary(
                self.libname,
                sources=self.dirdata['cfiles'],
                extra_compile_args=platform['extra_compile_args']+self.extra_compile_args,
                include_dirs=self.includeDirs + platform['incdirs'],
                libraries=self.externalLibs,# + platform['libs'],
                library_dirs=self.externalLibDirs +
                platform['libdirs'],
                extra_link_args=platform['extra_link_args'])

    # Find all directories containing at least one swig input file.  These
    # are used to create the swigged python packages.  This is done by
    # traversing the DIR.py files, so that random leftover .swg files in
    # strange places don't create packages, and so that modules can be
    # included conditionally by HAVE_XXX tests in DIR.py files.
    def find_swig_pkgs(self):
        pkgs = set()
        for swigfile in self.dirdata['swigfiles']:
            pkgs.add(os.path.split(swigfile)[0])
        # pkgs is a set of dirs containing swig files, relative to
        # the main OOF2 dir, eg, "./SRCDIR/common".
        # Convert it to a list of dirs relative to swigroot
        swigpkgs = []
        for pkg in pkgs:
            relpath = os.path.relpath(pkg, SRCDIR)
            relocated = os.path.normpath(
                os.path.join(PROGNAME, SWIGDIR, relpath))
            pkgname = relocated.replace('/', '.')
            swigpkgs.append(pkgname)
        return swigpkgs

# end class CLibInfo

def moduleSort(moduleA, moduleB):
    if moduleA.ordering is not None:
        if moduleB.ordering is not None:
            return cmp(moduleA.ordering, moduleB.ordering)
        return -1
    else:                               # moduleA.ordering is None
        if moduleB.ordering is not None:
            return 1
        return cmp(moduleA.name, moduleB.name)

def allFiles(key):
    hierlist = [lib.dirdata[key] for lib in allCLibs.values()]
    flatlist = []
    for sublist in hierlist:
        flatlist.extend(sublist)
    return flatlist


def readDIRs(srcdir):
    dirfile = os.path.join(srcdir, DIRFILE)
    if os.path.exists(dirfile):
        log.info("loading %s", dirfile)
        # dirfile defines variables whose names are the same as the
        # ModuleInfo.dirdata keys.  The variables contain lists of
        # file names.
        localdict = {}
        execfile(dirfile, globals(), localdict)
        # Now the variables and functions defined in dirfile are in localdict.
        try:
            dirname = localdict['dirname']
            del localdict['dirname']
        except KeyError:
            pass
        
        try:
            clib = localdict['clib']
            del localdict['clib']
        except KeyError:
            pass
        else:
            clibinfo = getCLibInfo(clib)
            clibinfo.extractData(srcdir, localdict)
            
        try:
            pyfiles = localdict['pyfiles']
            del localdict['pyfiles']
        except KeyError:
            pass
        else:
            for filename in pyfiles:
                purepyfiles.append(os.path.join(srcdir, filename))

        # dirfile also contains a list of subdirectories to process.
        try:
            subdirs = localdict['subdirs']
            del localdict['subdirs']
        except KeyError:
            pass
        else:
            # At this point, all args in localdict should have been processed.
            if len(localdict) > 0:
                log.warn("WARNING: unrecognized values %s in %s",
                         localdict.keys(), dirfile)
            for subdir in subdirs:
                readDIRs(os.path.join(srcdir, subdir))

##########

# Find all python packages and subpackages in a directory by looking
# for __init__.py files.

def find_pkgs():
    pkglist = []
    os.path.walk(SRCDIR, _find_pkgs, pkglist)
    return pkglist

def _find_pkgs(pkglist, dirname, subdirs):
    if os.path.exists(os.path.join(dirname, '__init__.py')):
        pkglist.append(dirname)

##########

def swig_clibs(dry_run, force, debug, build_temp, with_swig=None):
    # First make sure that swig has been built.
    if with_swig is None:
        ## TODO 3.1: swig is installed inside the distutils
        ## build/temp* directory to avoid conflicts if oof is being
        ## built for multiple architectures on a shared file system.
        ## However, swig's .o file and other intermediate files
        ## (parser.cxx, parser.h, config.log, Makefiles, etc) are
        ## still in OOFSWIG/SWIG.  They'll have to be removed manually
        ## before building on a different architecture.  It would be
        ## better if they were in build/temp* too, but that might
        ## require modifying the Makefile.
        swigsrcdir = os.path.abspath('OOFSWIG')
        swigbuilddir = os.path.join(os.path.abspath(build_temp), 'swig-build')
        if not os.path.exists(swigbuilddir):
            os.mkdir(swigbuilddir)
        swigexec = os.path.join(swigbuilddir, 'bin', 'swig')
        if not os.path.exists(swigexec):
            log.info("Building swig")
            status = os.system(
                'cd %s && ./configure --prefix=%s && make && make install' 
                % (swigsrcdir, swigbuilddir))
            if status:
                sys.exit(status)
    else:
        swigexec = with_swig
    srcdir = os.path.abspath(SRCDIR)
    extra_args = platform['extra_swig_args']
    if debug:
        extra_args.append('-DDEBUG')
    if USEMAGICK:
        extra_args.append('-DOOFCANVAS_USE_IMAGEMAGICK')
    for clib in allCLibs.values():
        for swigfile in clib.dirdata['swigfiles']:
            # run_swig requires a src dir and an input file path
            # relative to it.  The '+1' in the following line strips
            # off a '/', so that sfile doesn't look like an absolute
            # path.
            sfile = os.path.abspath(swigfile)[len(srcdir)+1:]
            run_swig(srcdir=SRCDIR, swigfile=sfile, destdir=swigroot,
                     cext=SWIGCFILEEXT,
                     dry_run=dry_run,
                     extra_args=extra_args,
                     force=force,
                     with_swig=swigexec,
                     )

##########

# Get a file's modification time.  The time is returned as an integer.
# All we care about is that the integers are properly ordered.

def modification_time(phile):
    return os.stat(phile)[stat.ST_MTIME]

#########

# If we're building with python-dbg, the shared libraries that it
# builds will have a "_d" added to their names, and we need to
# know that in order to link to them.  SHLIB_EXT is either ".so"
# or "_d.so".  Unfortunately, the quotation marks are included.

_sfx = get_config_var("SHLIB_EXT").split('.')[0]
if _sfx[0] == '"':
    _sfx = _sfx[1:]

if _sfx:
    log.debug("Library suffix is %s", _sfx)

def fixLibName(libname):
    return libname + _sfx

def addOOFlibs(clib, *libnames):
    for libname in libnames:
        clib.externalLibs.append(fixLibName(libname))

#########

# Define subclasses of the distutils build_ext and build_shlib class.
# We need subclasses so that makedepend can be run.  oof_build_xxxx
# contains the routines that are being added to both build_ext and
# build_shlib.

_dependencies_checked = 0
class oof_build_xxxx:
    def check_header(self, headername):
        # Check that a c++ header file exists on the system.
        log.info("Testing for %s", headername)
        tmpfiled, tmpfilename = tempfile.mkstemp(suffix='.C')
        tmpfile = os.fdopen(tmpfiled, 'w')
        print >> tmpfile, """\
        #include %s
        int main(int, char**) { return 1; }
        """ % headername
        tmpfile.flush()
        try:
            try:
                ofiles = self.compiler.compile(
                    [tmpfilename],
                    extra_postargs=platform['extra_compile_args']
                    )
            except errors.CompileError:
                return 0
            ofile = ofiles[0]
            dir = os.path.split(ofile)[0]
            os.remove(ofiles[0])
            if dir:
                try:
                    os.removedirs(dir)
                except:
                    pass
            return 1
        finally:
            os.remove(tmpfilename)

    def find_dependencies(self):
        # distutils doesn't provide a makedepend-like facility, so we
        # have to do it ourselves.  makedepend is deprecated, so we
        # use "gcc -MM" and hope that gcc is available.  This routine
        # runs "gcc -MM" and constructs a dictionary listing the
        # dependencies of each .o file and each swig-generated .C
        # file.

        # TODO: Check for the existence of gcc and makedepend and use
        # the one that's available.

        # depdict[file] is a list of sources that the file depends
        # upon.
        depdict = {}

        # Run "gcc -MM" on the C++ files to detect dependencies.  "gcc
        # -MM" only prints the file name of the target, not its path
        # relative to the build directory, so we have to use the -MT
        # flag to specify the target.  That means that we can't
        # process more than one C++ file at a time.
        log.info("Finding dependencies for C++ files.")
        for phile in allFiles('cfiles'):
            ## Hack Alert.  We don't know the full paths to some of
            ## the system header files at this point.  The -MM flag is
            ## supposed to tell gcc to ignore the system headers, but
            ## apparently some versions still want to be able to find
            ## them, and fail when they don't.  So we use -MG, which
            ## tells gcc to add missing headers to the dependency
            ## list, and then we weed them out later.  At least this
            ## way, the "missing" headers don't cause errors.
            cmd = 'g++ -std=c++11 -MM -MG -MT %(target)s -I%(srcdir)s -I%(builddir)s -I%(buildsrc)s %(file)s' \
              % {'file' : phile,
                 'target': os.path.splitext(phile)[0] + ".o",
                 'builddir' : self.build_temp,
                 'buildsrc' : os.path.join(self.build_temp, SRCDIR),
                 'srcdir' : SRCDIR
                 }
            proc = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE,
                                    bufsize=4096)
            stdoutdata, stderrdata = proc.communicate()
            if stderrdata:
                log.error("Command failed: %s", cmd)
                log.error("%s", stderrdata)
                sys.exit(1)
            if not stdoutdata:
                log.error("Command failed, no output: %s", cmd)
                sys.exit(1)
            # stdoutdata is a multiline string.  The first substring
            # is the name of the target file, followed by a colon.
            # The remaining substrings are the source files that the
            # target depends on, but there can also be line
            # continuation characters (backslashes) which must be
            # ignored.  It's even possible that the *first* line is
            # blank except for a backslash.
            files = [f for f in stdoutdata.split() if f != "\\"]
            target = files[0][:-1] # omit the colon
            realtarget = os.path.normpath(os.path.join(self.build_temp, target))
            for source in files[1:]:
                ## See Hack Alert, above.  Missing header files will
                ## all be outside of our directory hierarchy, so we
                ## just ignore any dependency that doesn't begin with
                ## SRCDIR.
                if (source.startswith(SRCDIR+'/') or
                    source.startswith(self.build_temp)):
                    depdict.setdefault(realtarget, []).append(source)

        # .C and.py files in the SWIG directory depend on those in the
        # SRCDIR directory.  Run gcc -MM on the swig source files.
        log.info("Finding dependencies for .swg files.")
        for phile in allFiles('swigfiles'):
            cmd = 'g++ -std=c++11 -MM -MG -MT %(target)s -x c++ -I. -I%(srcdir)s -I%(builddir)s %(file)s'\
              % {'file' : phile,
                 'target': os.path.splitext(phile)[0] + '.o',
                 'srcdir' : SRCDIR,
                 'builddir' : self.build_temp
              }
            proc = subprocess.Popen(shlex.split(cmd), 
                                    stdout=subprocess.PIPE, bufsize=4096)
            stdoutdata, stderrdata = proc.communicate()
            if stderrdata:
                log.error("Command failed: %s", cmd)
                log.error("%s", stderrdata)
                sys.exit(1)
            files = [f for f in stdoutdata.split() if f != "\\"]
            target = files[0][:-1]
            targetbase = os.path.splitext(target)[0]
            # On some systems, target begins with SRCDIR.  On
            # others, it begins with ./SRCDIR.  Arrgh.  This
            # strips off either one.
            targetbase = targetbase.split(SRCDIR+"/", 1)[1]
            targetc = os.path.normpath(
                os.path.join(swigroot, targetbase + SWIGCFILEEXT))
            targetpy = os.path.normpath(
                os.path.join(swigroot, targetbase + '.py'))
            for source in files[1:]:
                if (source.startswith(SRCDIR+'/') or
                    source.startswith(self.build_temp)):
                    depdict.setdefault(targetc, []).append(source)
                    depdict.setdefault(targetpy,[]).append(source)

        ## Debugging:
        # def dumpdepdict(filename, depdict):
        #     print >> sys.stderr, "Dumping dependency information to", filename
        #     f = file(filename, "w")
        #     keys = depdict.keys()
        #     keys.sort()
        #     for target in keys:
        #         print >> f, target
        #         sources = depdict[target]
        #         sources.sort()
        #         for source in sources:
        #             print >> f, "   ", source
        #     f.close()
        # dumpdepdict("depdict", depdict)

        # Add in the implicit dependencies on the .swg files.
        for phile in allFiles('swigfiles'):
            # phile is ./SRCDIR/dir/whatver.swg
            relfile = os.path.relpath(phile, SRCDIR) # dir/whatever.swg
            base = os.path.splitext(relfile)[0]      # dir/whatever
            cfile = os.path.normpath(os.path.join(swigroot,
                                                  base+SWIGCFILEEXT))
            pyfile = os.path.normpath(os.path.join(swigroot, base+'.py'))
            depdict.setdefault(cfile, []).append(phile)
            depdict.setdefault(pyfile, []).append(phile)
        # Add in the implicit dependencies on the .spy files.
        for underpyfile in allFiles('swigpyfiles'):
            relpath = os.path.relpath(underpyfile, './'+SRCDIR)
            relocated = os.path.normpath(os.path.join(swigroot, relpath))
            # Replace .spy with .py
            pyfile = os.path.splitext(relocated)[0] + ".py"
            depdict.setdefault(pyfile, []).append(underpyfile)

        return depdict
    

    # Remove out-of-date target files.  We have to do this because
    # distutils for Python 2.5 and earlier checks the dates of the .C
    # and .o files, but doesn't check for any included .h files, so it
    # doesn't rebuild enough.  For 2.6 and later, it doesn't check
    # anything, and it rebuilds far too much. To fix that, we
    # monkeypatch _setup_compile from Python 2.5 as well as remove the
    # out of date target files.

    def clean_targets(self, depdict):
        outofdate = False
        if not self.dry_run:
            for target, sources in depdict.items():
                if os.path.exists(target):
                    targettime = modification_time(target)
                    sourcetime = max([modification_time(x) for x in sources])
                    if sourcetime > targettime:
                        os.remove(target)
                        log.info(
                            "clean_targets: Removed out-of-date target %s" ,
                            target)
                        outofdate = True
                else:
                    outofdate = True
        if outofdate:
            ## TODO: Remove the .so file.  I can't figure out how to
            ## find its name at this point, though.
            pass

    def clean_dependencies(self):
        global _dependencies_checked
        if not _dependencies_checked:
            # Read dependencies from a file unless MAKEDEPEND has been
            # defined by providing the --makedepend command line
            # option, or if the file doesn't exist.
            depfilename = os.path.join(self.build_temp, 'depend')
            if not MAKEDEPEND and os.path.exists(depfilename):
                locals = {}
                log.info("Loading dependencies from %s", depfilename)
                execfile(depfilename, globals(), locals)
                depdict = locals['depdict']
            else:
                depdict = self.find_dependencies()
                log.info("Saving dependencies in %s", depfilename)
                mkpath(self.build_temp)
                depfile = open(depfilename, "w")
                print >> depfile, "depdict=", depdict
                depfile.close()
            self.clean_targets(depdict)
            _dependencies_checked = True


# This does the swigging.
class oof_build_ext(build_ext.build_ext, oof_build_xxxx):
    description = "build the python extension modules for OOF2"
    user_options = build_ext.build_ext.user_options + [
        ('with-swig=', None, "specify the swig executable")
    ]
    boolean_options = build_ext.build_ext.boolean_options
    
    def initialize_options(self):
        self.with_swig = None
        build_ext.build_ext.initialize_options(self)
    def finalize_options(self):
        self.set_undefined_options('build', ('with_swig', 'with_swig'))
        build_ext.build_ext.finalize_options(self)
    # build_extensions is called by build_ext.run().
    def build_extensions(self):
        self.compiler.add_include_dir(os.path.join(self.build_temp, SRCDIR))
        self.compiler.add_include_dir(SRCDIR)
        self.compiler.add_library_dir(self.build_lib)

        if self.debug:
            self.compiler.define_macro('DEBUG')
            # self.compiler.define_macro('Py_DEBUG')
            self.compiler.undefine_macro('NDEBUG')
        # Make the pkg-config file.
        self.make_pkgconfig()

        # Run makedepend
        self.clean_dependencies()
        # Generate swigged .C and .py files
        swig_clibs(self.dry_run, self.force, self.debug, self.build_temp,
                   self.with_swig)
                                          
        # Build the swig extensions by calling the distutils base
        # class function
        build_ext.build_ext.build_extensions(self)

    def make_pkgconfig(self):
        # We don't actually know enough to write the whole
        # oofcanvas.pc file here, but some of the information we need
        # is only available now.  In particular, the values of the
        # build args are not be available to the install command if
        # build and install aren't run at the same time.  So
        # oofcanvas.pc is written with placeholders that will be
        # replaced when it's installed by the (modified) install_data
        # command.

        cflags = []
        reqs = ["gtk+-3.0 >= %s" % GTK_VERSION]
        if USEMAGICK:
            cflags.append("-DOOFCANVAS_USE_IMAGEMAGICK")
            reqs.append("Magick++ >= %s" % MAGICK_VERSION)
        if BUILDPYTHONAPI:
            cflags.append("-DOOFCANVAS_USE_PYTHON")
        cfgfilename = os.path.normpath(os.path.join(self.build_temp,
                                                    'oofcanvas.pc'))

        cfgfile = open(cfgfilename, "w")
        print >> cfgfile, \
"""prefix=$PREFIX
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: oofcanvas
Description: A gtk compatible canvas class for use in C++ and Python
Version: %(version)s
URL: http://www.ctcms.nist.gov/oof/oofcanvas
Requires.private: cairomm-1.0 >= %(cairover)s, pango >= %(pangover)s, pangocairo >= %(pangocairover)s
Requires: %(reqs)s
Cflags: -I${includedir} %(cflgs)s
Libs: -L${libdir} -loofcanvas%(lsfx)s -loofcanvasGUI%(lsfx)s
""" % dict(cflgs=" ".join(cflags),
           version=version_from_make_dist,
           cairover=CAIROMM_VERSION,
           pangover=PANGO_VERSION,
           pangocairover=PANGOCAIRO_VERSION,
           gtkvers=GTK_VERSION,
           reqs=", ".join(reqs),
           lsfx=_sfx
           )
        
        cfgfile.close()

class oof_build_shlib(build_shlib.build_shlib, oof_build_xxxx):
    user_options = build_shlib.build_shlib.user_options + [
        ('with-swig=', None, "non-standard swig executable"),
        ]
    def initialize_options(self):
        self.with_swig = None
        build_shlib.build_shlib.initialize_options(self)
    def finalize_options(self):
        self.set_undefined_options('build',
                                   ('with_swig', 'with_swig'),
                                   ('libraries', 'libraries'),
                                   ('library_dirs', 'library_dirs'))
        build_shlib.build_shlib.finalize_options(self)
    def build_libraries(self, libraries):
        # self.make_oofconfig()
        self.clean_dependencies()
        self.compiler.add_include_dir(os.path.join(self.build_temp, SRCDIR))
        self.compiler.add_include_dir(SRCDIR)
        if self.debug:
            self.compiler.define_macro('DEBUG')

        build_shlib.build_shlib.build_libraries(self, libraries)

class oof_build(build.build):
    sep_by = " (separated by '%s')" % os.pathsep
    user_options = build.build.user_options + [
        ('with-swig=', None, "non-standard swig executable"),
        ('libraries=', None, 'external libraries to link with'),
        ('library-dirs=', None,
         "directories to search for external libraries" + sep_by),
    ]
    def initialize_options(self):
        self.libraries = None
        self.library_dirs = None
        self.with_swig = None
        build.build.initialize_options(self)


###################################################

# Modify the build_py command so that python files created by swig and
# living in SWIGDIR are installed back in the non-swig part of the
# hierarchy.  OOF2 keeps them separate, but there's no need to do that
# here, and it just makes importing messy.

class oof_build_py(build_py.build_py):

    def build_module (self, module, module_file, package):
        if type(package) is StringType:
            package = string.split(package, '.')
        elif type(package) not in (ListType, TupleType):
            raise TypeError, \
                  "'package' must be a string (dot-separated), list, or tuple"

        # Now put the module source file into the "build" area -- this is
        # easy, we just copy it somewhere under self.build_lib (the build
        # directory for Python source).
        # [Comment from distutils, not oofcanvas]
        outfile = self.get_module_outfile(self.build_lib, package, module)

        # The next line is the only one that is different from
        # original distutils. 
        outfile = os.path.normpath(outfile.replace(SWIGDIR, ""))
        
        dir = os.path.dirname(outfile)
        self.mkpath(dir)
        return self.copy_file(module_file, outfile, preserve_mode=0)

###################################################

# Modify install_data so that it can process files through a filter,
# and replaces $BUILDTEMP in files names with the actual build temp
# directory name.

class oof_install_data(install_data.install_data):
    def run(self):
        # Get the name of the temp directory, so that files created
        # there can be installed.
        buildcmd = self.get_finalized_command("build")
        buildtemp = buildcmd.build_temp
        
        self.mkpath(self.install_dir)
        for f in self.data_files:
            if isinstance(f, str):
                # it's a simple file, so copy it
                f = convert_path(f)
                f.replace("$BUILDTEMP", buildtemp)
                if self.warn_dir:
                    self.warn("setup script did not provide a directory for "
                              "'%s' -- installing right in '%s'" %
                              (f, self.install_dir))
                (out, _) = self.copy_file(f, self.install_dir)
                self.outfiles.append(out)
            else:
                # it's a tuple with path to install to and a list of
                # files, and an optional filter.
                destdir = convert_path(f[0])
                if len(f) == 3:
                    filt = f[2]
                else:
                    filt = None
                    
                if not os.path.isabs(destdir):
                    destdir = os.path.join(self.install_dir, destdir)
                elif self.root:
                    destdir = change_root(self.root, destdir)
                self.mkpath(destdir)

                if f[1] == []:
                    # If there are no files listed, the user must be
                    # trying to create an empty directory, so add the
                    # directory to the list of output files.
                    self.outfiles.append(destdir)
                else:
                    # Copy files, adding them to the list of output
                    # files.  When filtering files, use a temp
                    # directory instead of individual temp files,
                    # because mkstemp creates files that aren't
                    # readable by all.
                    tmpdir = tempfile.mkdtemp()
                    try:
                        for data in f[1]:
                            data = convert_path(data)
                            data = data.replace("$BUILDTEMP", buildtemp)
                            if filt:
                                fname = os.path.split(data)[1] # data file name
                                # Filter the data file to temp directory
                                fulltempname = os.path.join(tmpdir, fname)
                                tmpfile = open(fulltempname, "w")
                                filt(self, data, tmpfile)
                                tmpfile.close()
                                # Copy filtered file to destination
                                dest = os.path.join(destdir, fname)
                                (out, _) = self.copy_file(fulltempname, dest)
                                os.remove(fulltempname)
                            else:
                                (out, _) = self.copy_file(data, destdir)
                            self.outfiles.append(out)
                    finally:
                        os.rmdir(tmpdir)

# pkgconfigfilt is the filter function that is used to put the correct
# prefix into oofcanvas.pc.

def pkgconfigfilt(installcmd, datafile, destinationfile):
    prefix = installcmd.get_finalized_command("install").prefix
    realprefix = os.path.normpath(os.path.expanduser(prefix))
    f = open(datafile, "r")
    for line in f.readlines():
        print >> destinationfile, line.replace("$PREFIX", realprefix),

###################################################

class oof_clean(clean.clean):
    user_options = clean.clean.user_options + [
        ('most', 'm', 'remove libraries and scripts, but not binary dist.'),
        ('swig', None, 'remove swig output files')]
    boolean_options = clean.clean.boolean_options + ['most', 'swig']

    def initialize_options(self):
        clean.clean.initialize_options(self)
        self.most = None
        self.swig = None
        
    def run(self):
        if self.most and not self.all:
            for d in [self.build_lib, self.build_scripts]:
                if os.path.exists(d):
                    remove_tree(d,dry_run=self.dry_run)
                else:
                    log.warn("'%s' does not exist -- can't clean it.", d)
        if self.swig and os.path.exists(swigroot):
            remove_tree(swigroot, dry_run=self.dry_run)
            swigsrcdir = os.path.abspath('OOFSWIG')
            log.info("Cleaning swig")
            status = os.system('cd %s && make clean' % swigsrcdir)
            if status:
                sys.exit(status)
        clean.clean.run(self)
    
###################################################

def set_dirs():
    global swigroot, datadir, includedir
    swigroot = os.path.join(SRCDIR, SWIGDIR)
    # Splitting and reassembling paths makes them portable to systems
    # that don't use '/' as the path separator.  (Why are we worrying
    # about this?  We're certainly not consistent about it.)
    datadir = os.path.join(*DATADIR.split('/'))
    includedir = os.path.join(*INCLUDEDIR.split('/'))

def get_global_args():
    # The --enable-xxxx flags in the command line have to be obtained
    # *before* we parse the DIR.py files, and the DIR.py files must be
    # parsed before calling distutils.core.setup (because the list of
    # source files comes from DIR.py).  But distutils.core.setup handles
    # the command line arguments, so we have to look for and remove the
    # --enable-xxxx flags here.

    # TODO? Add  --enable-gui (--disable-gui?)

    global MAKEDEPEND, BUILDPYTHONAPI, USEMAGICK, PORTDIR
    MAKEDEPEND = _get_oof_arg('--makedepend')
    BUILDPYTHONAPI = _get_oof_arg('--pythonAPI')
    USEMAGICK = _get_oof_arg('--magick')
    PORTDIR = _get_oof_arg('--port-dir', '/opt/local')

    # The following determine some secondary installation directories.
    # They will be created within the main installation directory
    # specified by --prefix.
    ## TODO: These shouldn't be defined here.  Put them with DIRFILE, etc
    global DATADIR, PROGNAME, SWIGDIR, INCLUDEDIR
    DATADIR = "share/oofcanvas"
    INCLUDEDIR = "include/oofcanvas"
    PROGNAME = "oofcanvas"      # was OOFNAME in oof2
    SWIGDIR = "SWIG"            # root dir for swig output, inside SRCDIR


def _get_oof_arg(arg, default=None):
    # Search for an argument which begins like "arg" -- if found,
    # return the trailing portion if any, or 1 if none, and remove the
    # argument from sys.argv.
    for s in sys.argv:
        splits = s.split('=')
        if splits[0] == arg:
            sys.argv.remove(s)
            if len(splits) > 1:         # found an =
                return splits[1]
            return 1                    # just a plain arg
    return default                      # didn't find arg
        
platform = {}

def set_platform_values():
    ## Set platform-specific flags that don't specifically depend on
    ## OOF2 stuff.  They're stored in a dictionary just to keep things
    ## tidy.
    platform['extra_compile_args'] = []
    platform['macros'] = []
    platform['libdirs'] = []
    platform['incdirs'] = [get_config_var('INCLUDEPY')]
    platform['extra_link_args'] = []
    platform['extra_swig_args'] = []

    if os.path.exists('/usr/local/lib'):
        platform['libdirs'].append('/usr/local/lib')
    # if os.path.exists('/usr/site/lib'):
    #     platform['libdirs'].append('/usr/site/lib')
    if os.path.exists('/usr/site/include'):
        platform['incdirs'].append('/usr/site/include')

    if sys.platform == 'darwin':
        platform['extra_link_args'].append('-headerpad_max_install_names')
        # If we're using macports, the pkgconfig files for the python
        # modules aren't in the standard location.
        if os.path.exists('/opt'):
            ## TODO: Having to encode such a long path here seems
            ## wrong.  If and when pkgconfig acquires a more robust
            ## way of finding its files, use it.
            global PORTDIR
            pkgpath = os.path.join(PORTDIR, "Library/Frameworks/Python.framework/Versions/%d.%d/lib/pkgconfig/" % (sys.version_info[0], sys.version_info[1]))
            log.info("Adding %s", pkgpath)
            extend_path("PKG_CONFIG_PATH", pkgpath)
        # Enable C++11
        platform['extra_compile_args'].append('-Wno-c++11-extensions')
        platform['extra_compile_args'].append('-std=c++11')
        # After upgrading to High Sierra, deprecated register warnings
        # started coming from the python header files.
        platform['extra_compile_args'].append('-Wno-deprecated-register')
        if 'clang' in get_config_var('CC'):
            # If we're using clang, we want to suppress some warnings
            # about oddities in swig-generated code:
            platform['extra_compile_args'].append('-Wno-self-assign')
            
    elif sys.platform.startswith('linux'):
        # add -std=c++11 option to use c++11 standard
        platform['extra_compile_args'].append('-std=c++11')

    ## TODO: netbsd options may be out of date.  C++11 should be
    ## enabled.
    elif sys.platform[:6] == 'netbsd':
        platform['libdirs'].append('/usr/pkg/lib')


#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# In Python 2.6 and 2.7, distutils doesn't check for an existing .o
# file, and recompiles everything whether it needs to or not.  Since
# we remove all of the out-of-date .o files, we don't want to
# recompile all of them.  Here we monkeypatch the relevant method from
# the Python 2.5 distutils.

from distutils.dep_util import newer_pairwise, newer_group
from distutils.ccompiler import gen_preprocess_options
def _setup_compile(self, outdir, macros, incdirs, sources, depends,
                   extra):
    """Process arguments and decide which source files to compile.

    Merges _fix_compile_args() and _prep_compile().
    """
    if outdir is None:
        outdir = self.output_dir
    elif type(outdir) is not StringType:
        raise TypeError, "'output_dir' must be a string or None"

    if macros is None:
        macros = self.macros
    elif type(macros) is ListType:
        macros = macros + (self.macros or [])
    else:
        raise TypeError, "'macros' (if supplied) must be a list of tuples"

    if incdirs is None:
        incdirs = self.include_dirs
    elif type(incdirs) in (ListType, TupleType):
        incdirs = list(incdirs) + (self.include_dirs or [])
    else:
        raise TypeError, \
              "'include_dirs' (if supplied) must be a list of strings"
    if extra is None:
        extra = []

    # Get the list of expected output (object) files
    objects = self.object_filenames(sources,
                                    strip_dir=0,
                                    output_dir=outdir)
    assert len(objects) == len(sources)

    # XXX should redo this code to eliminate skip_source entirely.
    # XXX instead create build and issue skip messages inline

    if self.force:
        skip_source = {}            # rebuild everything
        for source in sources:
            skip_source[source] = 0
    elif depends is None:
        # If depends is None, figure out which source files we
        # have to recompile according to a simplistic check. We
        # just compare the source and object file, no deep
        # dependency checking involving header files.
        skip_source = {}            # rebuild everything
        for source in sources:      # no wait, rebuild nothing
            skip_source[source] = 1

        n_sources, n_objects = newer_pairwise(sources, objects)
        for source in n_sources:    # no really, only rebuild what's
            skip_source[source] = 0 # out-of-date
    else:
        # If depends is a list of files, then do a different
        # simplistic check.  Assume that each object depends on
        # its source and all files in the depends list.
        skip_source = {}
        # L contains all the depends plus a spot at the end for a
        # particular source file
        L = depends[:] + [None]
        for i in range(len(objects)):
            source = sources[i]
            L[-1] = source
            if newer_group(L, objects[i]):
                skip_source[source] = 0
            else:
                skip_source[source] = 1

    pp_opts = gen_preprocess_options(macros, incdirs)

    build = {}
    for i in range(len(sources)):
        src = sources[i]
        obj = objects[i]
        ext = os.path.splitext(src)[1]
        self.mkpath(os.path.dirname(obj))
        if skip_source[src]:
            log.debug("skipping %s (%s up-to-date)", src, obj)
        else:
            build[obj] = src, ext

    return macros, objects, extra, pp_opts, build
CCompiler._setup_compile = _setup_compile

# End of monkeypatch 

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

if __name__ == '__main__':
    get_global_args()
    set_dirs()
    set_platform_values()

    readDIRs('.')                       # Gather data from the DIR.py files.
    
    # Get the data to build the C++ extension modules.
    clibraries = allCLibs.values()
    clibraries.sort(moduleSort)
    extensions = []
    shlibs = []
    for clib in clibraries:
        clib.run_pkg_config()
        extensions.extend(clib.get_extensions())
        shlib = clib.get_shlib()
        if shlib is not None:
            shlibs.append(shlib)

    # Construct the list of pure python packages. If a subdirectory has an
    # __init__.py file, then all the .py files in the subdirectory will
    # form a package.

    # find non-swigged files
    pkg_list = set()
    pkgs = find_pkgs()          # ['SRCDIR', 'SRCDIR/common', ...]
    swigdir = os.path.join(SRCDIR, SWIGDIR)
    for pkg in pkgs:
        if not pkg.startswith(swigdir):
            pkgname = PROGNAME + '.' + os.path.relpath(pkg, SRCDIR).replace('/', '.')
            pkg_list.add(pkgname)

    # Ask each CLibInfo object for the swigged python modules it
    # creates.

    for clib in allCLibs.values():
        pkg_list.update(clib.find_swig_pkgs())

    # Make sure that intermediate directories are in the package list.
    allpkgs = set()
    for pkg in pkg_list:
        comps = pkg.split('.')  # components of the package path
        if comps[0] == PROGNAME:
            p = comps[0]
            # Add components one by one to the base component and add
            # the resulting path to the package list.
            for pp in comps[1:]:
                p = '.'.join([p, pp])
                allpkgs.add(p)
        else:
            allpkgs.add(pkg)

    # # Find example files that have to be installed.
    # examplefiles = []
    # for dirpath, dirnames, filenames in os.walk('examples'):
    #     if filenames:
    #         examplefiles.append(
    #             (os.path.join(datadir, dirpath), # installation dir
    #              [os.path.join(dirpath, phile) for phile in filenames
    #               if not phile.endswith('~') and
    #               os.path.isfile(os.path.join(dirpath, phile))]))

    datafiles = [
        # "$BUILDTEMP" is replaced by the modified install_data
        # command.  pkgconfigfilt replaces $PREFIX in oofcanvas.pc
        # with the actual prefix.
        (os.path.join("lib", "pkgconfig"), ['$BUILDTEMP/oofcanvas.pc'],
         pkgconfigfilt)
    ]
    
    # Get header files from CLibInfo objects.  These are the header
    # files that need to be installed in <prefix>/include/oofcanvas,
    # for OOFCanvas users.  Headers that are only used when building
    # OOFCanvas don't need to be listed.
    for clib in allCLibs.values():
        hfiles = clib.dirdata['hfiles']
        if hfiles:
            # tuple containing installation dir and list of files
            datafiles.append((includedir, hfiles))
        
    setupargs = dict(
        name = PROGNAME,
        version = version_from_make_dist,
        description = "A 2D canvas for use in Gtk, from NIST.",
        author = 'The NIST OOF Team',
        author_email = 'oof_manager@nist.gov',
        #url = "http://www.ctcms.nist.gov/oof/oof2/",
        # If more scripts are added here, change oof_build_scripts too.
        #scripts = ['oof2', 'oof3d'],
        cmdclass = {"build" : oof_build,
                    "build_ext" : oof_build_ext,
                    "build_py" : oof_build_py,
                    "build_shlib": oof_build_shlib,
                    # "build_scripts" : oof_build_scripts,
                    ## See comment about oof2installlib above.
                    "install_lib": oof2installlib.oof_install_lib,
                    "install_data": oof_install_data,
                    "clean" : oof_clean},
        packages = allpkgs,
        package_dir = {PROGNAME:SRCDIR},
        shlibs = shlibs,
        ext_modules = extensions,
        data_files = datafiles,
        )

    #options = dict(build = dict(plat_name = distutils.util.get_platform()))
    #setupargs['options'] = options

    distutils.core.setup(**setupargs)


