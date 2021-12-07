# -*- python -*-

# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modified
# versions of this software, you first contact the authors at
# oof_manager@nist.gov.

## Distutils command to install the shared libraries built by the
## build_shlib command.  Most of this code was cribbed from the other
## install_* commands.  Perhaps install_shlib should be derived from
## one of them.

## install_shlib is inserted into the distutils command structure by
## running setup_shlib.py.

import os
import sys
from distutils.core import Command
from distutils.util import convert_path
from distutils import log
from distutils.errors import DistutilsExecError

class install_shlib(Command):
    description = "install shared libs used by extension modules"

    user_options = [
        ('install-dir=', 'd', "directory to install to"),
        ('build-dir=', 'b', "build directory (where to install from)"),
        ('skip-build', None, "skip the build steps")
        ]
    boolean_options = ['skip-build']

    def initialize_options(self):
        self.install_dir = None
        self.build_dir = None
        self.outfiles = []
        self.shlibs = self.distribution.shlibs
        self.skip_build = None

    def finalize_options(self):
        self.set_undefined_options('install',
                                   ('install_shlib', 'install_dir'),
                                   ('skip_build', 'skip_build'))
        self.set_undefined_options('build_shlib',
                                   ('build_shlib', 'build_dir'))

    def run(self):
        self.build()
        self.install()
        
    def build(self):
        if not self.skip_build:
            if self.distribution.has_shared_libraries():
                self.run_command('build_shlib')

    def install(self):
        if os.path.isdir(self.build_dir):
            # outfiles contains the paths to the shared library files.
            # They're of the form <root>/<prefix>/lib/libXXXXXX.dylib
            # (or .so).  <root> is the value of the install command's
            # --root arg, and is expected to be DESTDIR
            # (https://www.gnu.org/prep/standards/html_node/DESTDIR.html).
            # BUT the files will be eventually installed into
            # <prefix>/lib/libXXXXXX.dylib so the shared library ID
            # stored in the file on Macs needs to be corrected.

            outfiles = self.copy_tree(self.build_dir, self.install_dir)

            # On macOS, we have to run install_name_tool here, since
            # dylibs contain info about their own location and the
            # locations of the libraries they link to.  The
            # alternative is to force users to set DYLD_LIBRARY_PATH.
            if sys.platform == "darwin":
                prefix = os.path.expanduser(
                    self.get_finalized_command("install").prefix)
                
                root = self.get_finalized_command("install").root
                if root:
                    relinstall_dir = os.path.join(
                        os.sep, # needs initial /
                        os.path.relpath(self.install_dir, root))
                else:
                    relinstall_dir = self.install_dir
                log.info("ROOT=%s", root)
                log.info("PREFIX=%s", prefix)
                log.info("INSTALL_DIR=%s", self.install_dir)
                log.info("RELINSTALL_DIR=%s", relinstall_dir)
                # if prefix[0] == os.sep:
                #     # os.path.join will just return its second argument if
                #     # it starts with /.
                #     prefix = prefix[1:]
            
                for ofile in outfiles:
                    # self.install_dir should be <root>/<prefix>/lib
                    relpath = os.path.relpath(ofile, self.install_dir)
                    newpath = os.path.join(prefix, relpath)
                    log.info("rpath=%s", rpath)
                    log.info("newpath=%s", newpath)
                    cmd = "install_name_tool -id %(np)s %(of)s" \
                        % dict(np=newpath, of=ofile)
                    log.info(cmd)
                    ## TODO: Use subprocess
                    errorcode = os.system(cmd)
                    if errorcode:
                        raise DistutilsExecError("command failed: %s" % cmd)
                    # See what other dylibs it links to.  If they're
                    # ours, then we have to make sure they link to the
                    # final location.
                    ## TODO: This isn't tested because we don't link
                    ## to other dylibs in OOFCanvas.  It will have to
                    ## be modified for OOF2.
                    f = os.popen('otool -L %s' % ofile)
                    for line in f.readlines():
                        l = line.lstrip()
                        if l.startswith("build"): # it's one of ours
                            dylib = l.split()[0] # full path in build dir
                            dylibname = os.path.split(dylib)[1]
                            cmd = 'install_name_tool -change %s %s %s' % (
                                dylib,
                                os.path.join(self.install_dir, dylibname),
                                ofile)
                            log.info(cmd)
                            ## TODO: Use subprocess
                            errorcode = os.system(cmd)
                            if errorcode:
                                raise DistutilsExecError("command failed: %s"
                                                         % cmd)
        else:
            self.warn("'%s' does not exist! no shared libraries to install"
                      % self.build_dir)
            return
        return outfiles

    def get_outputs(self):
        # List of files that would be installed if this command were run.
        if not self.distribution.has_shared_libraries():
            return []
        build_cmd = self.get_finalized_command('build_shlib')
        build_files = build_cmd.get_outputs()
        build_dir = build_cmd.build_shlib
        prefix_len = len(build_dir) + len(os.sep)
        outputs = []
        for file in build_files:
            outputs.append(os.path.join(self.install_dir, file[prefix_len:]))
        return outputs

    def get_inputs(self):
        if not self.distribution.has_shared_libraries():
            return []
        build_cmd = self.get_finalized_command('build_py')
        return build_cmd.get_outputs()
            
