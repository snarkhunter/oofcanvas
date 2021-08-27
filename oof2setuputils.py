# -*- python -*-


# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modified
# versions of this software, you first contact the authors at
# oof_manager@nist.gov.

# This file contains routines used by the main OOF2 setup.py script.
# These routines are also used by oof2extutils.py, which is used by
# the authors of external OOF2 extensions.

import os, sys, string, stat

# Check for packages that use pkg-config for their options.  Include
# their compiler and linker flags if they're found, and complain if
# they're not.

## TODO: Each set_clib_flags function may call pkg_check, and
## therefore pkg-config multiple times, which may concatenate
## duplicate copies of the compilation arguments.  The pkg-config
## calls should be combined.

def pkg_check(package, version, clib=None):
    if check_exec('pkg-config'):
        if os.system("pkg-config --atleast-version=%s %s" % (version, package)):
            print "Can't find %s! Version %s or later required" % (package,
                                                                   version)
            sys.exit()
        if clib:
            # This will cause pkg-config --cflags and pkg-config
            # --libs to be called later on for this package.
            clib.add_pkg(package)
    else:
        print "Can't find pkg-config!"
        sys.exit()
    


# Check that an executable file exists in the environment's PATH.

pathdirs = os.getenv("PATH").split(":")

def check_exec(xfile):          # is xfile an executable file in the path?
    for dir in pathdirs:
        try:
            if os.path.exists(dir):
                files = os.listdir(dir)
                if xfile in files:
                    fullname = os.path.join(dir, xfile)
                    if os.access(fullname, os.X_OK):
#                         print >> sys.stderr, "Found", xfile, "as", fullname
                        return 1
        except: # Ignore permission errors
            pass
    if sys.platform == 'cygwin' and not xfile.endswith('.exe'):
        return check_exec(xfile + '.exe')
    print "Warning!", xfile, "not found!"

# See if a given file exists in a list of directories.  Return the
# directory containing the file, or None if the file can't be found.

def find_file(filename, dirs):
    for dir in dirs:
        if os.path.exists(os.path.join(dir, filename)):
            return dir

# Add dirs to the given path environment variable.

def extend_path(envpath, *dirs):
    try:
        pathstring = os.environ[envpath]
    except KeyError:
        path = []
    else:
        path = pathstring.split(':')
    # prepend directories to the path
    path[0:0] = [d for d in dirs if os.path.exists(d)]
    if path:
        os.environ[envpath] = ':'.join(path)
    
###################

SWIG = 'swig'                           # must be swig 1.1 build 883
SWIGARGS = ["-shadow", "-dnone", "-python", "-c++", "-c"]

def run_swig(srcdir, swigfile, destdir, cext="_.C", include_dirs=[],
             dry_run=False, force=False, with_swig=None, extra_args=[]):

    # srcdir is a directory.  swigfile is the name of the input swig
    # file, specfied *relative* to srcdir.  The output files will go
    # into destdir.  If swigfile has subdirectories as part of its
    # name, then those subdirectories will be created in destdir.  For
    # example, if srcdir is "A/B", swigfile is "C/swig.swg", and
    # destdir is "E/F", then the file "A/B/C/swig.swg" will be swigged
    # to create "E/F/C/swig_.C" and "E/F/C/swig.py".

    # cext is the extension to use on the output C++ file.

    swig = with_swig or SWIG

    abssrcdir = os.path.abspath(srcdir)
    destdir = os.path.abspath(destdir)
    relpath = os.path.split(swigfile)[0]
    infile = os.path.join(abssrcdir, swigfile)
    indir = os.path.join(abssrcdir, relpath)
    outdir = os.path.join(destdir, relpath)

    swigfilename = os.path.split(swigfile)[1]
    basename = os.path.splitext(swigfilename)[0]
    cfile = os.path.join(destdir, relpath, basename + cext)
    pyfile = os.path.join(destdir, relpath, basename + ".py")

    infiletime = os.stat(infile)[stat.ST_MTIME]
    uptodate = os.path.exists(cfile) and os.path.exists(pyfile) and \
               (os.stat(cfile)[stat.ST_MTIME] > infiletime) and \
               (os.stat(pyfile)[stat.ST_MTIME] > infiletime)

    if force or not uptodate:

        # Make sure that outdir exists.
        if not os.path.exists(outdir):
            os.makedirs(outdir)

        # Make sure that all directories from destdir to outdir
        # contain __init__.py.
        destdepth = len(destdir.split(os.sep))
        outsplit = outdir.split(os.sep)[destdepth:]
        # In OOFCanvas we want the __init__.py files in the
        # subdirectories to automatically import everything in the
        # module.  We can't simply create __init__.py files in the
        # source directory, because then distutils will think that the
        # source directories are pure python modules and will install
        # them.  That would actually be ok, since there are no python
        # files in the source directories other than __init__.py,
        # except that we *also* need to have __init__.py files in the
        # swig output directories, and those would get installed after
        # the __init__.py files in the source directories, clobbering
        # them.  So here we create the __init__.py files on the fly in
        # the swig output directories.
        #
        # OOF2 doesn't have this problem because the swig generated
        # python files and the pure python files are installed in
        # separate directories.
        #
        # This is a hack because this is the wrong place to be
        # deciding what the content of the __init__.py files should
        # be.  It assumes that no modifications to OOFCanvas will
        # create pure python modules.  It assumes that a swig output
        # directory contains a single python file, whose base name is
        # the same as the name of the directory.
        #
        # Probably the correct thing to do is to specify the contents
        # of __init__.py in DIR.py.
        addInitPy(destdir, "from %s import *" % srcdir)
        odir = destdir
        for subdir in outsplit:
            odir = os.path.join(odir, subdir)
            addInitPy(odir, "from %s import *" % subdir)

        incls = [os.path.abspath(dir) for dir in include_dirs]
        if abssrcdir not in incls:
            incls.append(abssrcdir)
        if indir not in incls:
            incls.append(indir)
        
        # The following command cds to the source directory and runs
        # swig.  It uses absolute path names for all of the
        # directories so that we don't have to worry about the effect
        # of the cd.  The cd is necessary because the swig
        # 'pragma(python) include' lines don't seem to respect the -I
        # flags.
        
        cmd = 'cd %(indir)s; %(swig)s %(swigargs)s %(includes)s -module %(module)s -o %(cfile)s %(infile)s' \
              %  {'indir' : indir,
                  'swig': swig,
                  'swigargs' : string.join(SWIGARGS+extra_args, " "),
                  'includes' : string.join(['-I'+file for file in incls], ' '),
                  'module' : basename,
                  'cfile' : cfile,
                  'infile' : infile}
        print cmd
        if not dry_run:
            os.system(cmd)

    return dict(basename=basename,
                indir=indir,
                outdir=outdir,
                cfile=cfile
                )


def addInitPy(subdir, initcode=None):
    initpy = os.path.join(subdir, '__init__.py')
    if not os.path.exists(initpy):
        initfile = open(initpy, 'w')
        print >> initfile, "# This file was created automatically by setup.py."
        print >> initfile, "# See run_swig() in oof2setuputils.py for an explanation."
        if initcode:
            print >> initfile, initcode
        initfile.close()
