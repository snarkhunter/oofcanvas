// -*- C++ -*-

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

#ifndef VERSION_SWG
#define VERSION_SWG

%module(package="oofcanvas") "version"

%import "oofcanvas/typemaps.swg"

%{
#define SWIG_FILE_WITH_INIT
#include "oofcanvas/cversion.h"
using namespace OOFCanvas;
%}

%rename(version) oofcanvs_version;
const std::string *version();

#endif // VERSION_SWG
