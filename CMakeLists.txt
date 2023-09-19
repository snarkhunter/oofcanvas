# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modifed
# versions of this software, you first contact the authors at
# oof_manager@nist.gov.

cmake_minimum_required(VERSION 3.18)
project(oofcanvas VERSION 1.1.1)

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Options to be set by the user

## TODO: Can we list only the available versions of swig and python?
set(OOFCANVAS_SWIG_VERSION 4.1 CACHE STRING "Choose 4.0 or 4.1")
set_property(CACHE OOFCANVAS_SWIG_VERSION PROPERTY STRINGS 4.0 4.1)

set(pythonOptions None Python2 Python3)
set(OOFCANVAS_PYTHON_API Python3 CACHE STRING "Generate the Python API")
set_property(CACHE OOFCANVAS_PYTHON_API PROPERTY STRINGS ${pythonOptions})

if(${OOFCANVAS_PYTHON_API} STREQUAL "Python3")
  set(OOFCANVAS_PYTHON3_VERSION "Latest" CACHE STRING "Python3 version number")
  set_property(CACHE OOFCANVAS_PYTHON3_VERSION
    PROPERTY STRINGS 3.8 3.9 3.10 3.11 Latest)
else()
  unset(OOFCANVAS_PYTHON3_VERSION CACHE)
endif()

message("**** OOFCANVAS_PYTHON3_VERSION=${OOFCANVAS_PYTHON3_VERSION}")

option(OOFCANVAS_USE_IMAGEMAGICK "Use ImageMagick" ON)

## TODO NUMPY: OOF2 is crashing on linux (Ubuntu 22.04 on VirtualBox
## on Mac) when numpy and imagemagick are both activated in oofcanvas.
## See oof2-guitest 000100_tutorial_micro.  So I've made it an
## "advanced" variable here, which should discourage user from turning
## it on by mistake.
option(
  OOFCANVAS_USE_NUMPY "Display scikit-image/numpy images (experimental)" OFF)
mark_as_advanced(FORCE OOFCANVAS_USE_NUMPY)

# OOFCANVAS_SWIG_USE_BUILTIN is experimental.  It won't work unless we've
# figured out how to avoid any monkeypatching of the swigged classes.
option(OOFCANVAS_SWIG_USE_BUILTIN "Use the -builtin option for swig (experimental)" OFF)
mark_as_advanced(FORCE OOFCANVAS_SWIG_USE_BUILTIN)

# If OOFCanvas is being installed by a regular user, the python files
# should be installed into lib/pythonX.Y/site-packages/oofcanvas,
# which is probably in /usr/local or the user's home directory.  But
# if it's being installed by MacPorts or another package manager they
# should be installed in
# ${Python3_LIBRARY_DIRS}/pythonX.Y/site-packages.  The package
# manager should set OOFCANVAS_SYSTEM_INSTALL to ON, which will cause
# the files to be installed under Python3_LIBRARY_DIRS.
## TODO: What about Python2?
option(OOFCANVAS_SYSTEM_INSTALL OFF "Install in system directory?")
mark_as_advanced(OOFCANVAS_SYSTEM_INSTALL)

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Required version numbers for dependencies.

set(GTK3_MIN_VERSION 3.22)
set(CAIRO_MIN_VERSION 1.12)
set(PANGOCAIRO_MIN_VERSION 1.40)
set(PANGO_MIN_VERSION 1.40)
set(PYGOBJECT_MIN_VERSION 3.22)
set(MAGICK_MIN_VERSION 6.0)
set(MAGICK_MAX_VERSION 7.0) # must be less than this
set(NUMPY_MIN_VERSION 1.21)

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Build type

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)

# Set the default build type.  See
# https://www.kitware.com/cmake-and-the-default-build-type/
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
    STRING "Debug or Release" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

# Apparently using -DDEBUG for debugging is not the modern
# convention. CMake instead assumes that we're using -DNDEBUG when not
# debugging, so add -DDEBUG here.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Libraries to build. The cmake target is called "oofcanvasCore"
# instead of "oofcanvas" because we want the python interface to be
# "oofcanvas", and cmake target names must be unique.  But it will be
# installed as "liboofcanvas.<suffix>" anyway.

## TODO: Make oofcanvasGUI optional? 

set(oofcanvaslibs
  oofcanvasCore
  oofcanvasGUI)

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# swig_sources() should be called in the CMakeLists.txt files in all
# subdirectories that contain swig files.
#  swig_sources(
#        SWIGFILES  a b      # .swg suffix is assumed
#        LIBRARIES  # names of libraries to link to, must be cmake targets
#        CFLAGS     # additional compiler options
 
function(swig_sources)
  if(${OOFCANVAS_PYTHON_API} STREQUAL None)
    return()
  endif()
  set(multiValueArgs SWIGFILES LIBRARIES CFLAGS INCLUDE_DIRECTORIES)
  cmake_parse_arguments(PARSE_ARGV 0 SS "" "" "${multiValueArgs}")
  # "SS" for Swig_Sources

  # message("--- ${CMAKE_CURRENT_SOURCE_DIR}")
  # message("SWIGFILES: ${SS_SWIGFILES}")
  # message("LIBRARIES: ${SS_LIBRARIES}")
  # message("CFLAGS   : ${SS_CFLAGS}")
  # message("INCLUDE_DIRS : ${SS_INCLUDE_DIRECTORIES}")

  foreach(swigfile ${SS_SWIGFILES})
    set_property(
      SOURCE ${swigfile}.swg
      PROPERTY CPLUSPLUS ON)
    # Include directories for the swig command
    set_property(
      SOURCE ${swigfile}.swg
      PROPERTY INCLUDE_DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${PROJECT_SOURCE_DIR}
      ${SS_INCLUDE_DIRECTORIES})
    # Include directories for the C++ compiler.
    set_property(
      SOURCE ${swigfile}.swg
      PROPERTY GENERATED_INCLUDE_DIRECTORIES
      ${SS_INCLUDE_DIRECTORIES}
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${PROJECT_SOURCE_DIR}
      ${PROJECT_BINARY_DIR} 	# for headers made by configure_file()
      ${PYINCL}
      ${GTK3_INCLUDE_DIRS}
      ${CAIRO_INCLUDE_DIRS}
      ${PANGOCAIRO_INCLUDE_DIRS})
    # Compiler options for the C++ compiler
    set_property(
      SOURCE ${swigfile}.swg
      PROPERTY GENERATED_COMPILE_OPTIONS
      -Wno-deprecated-register
      ${CAIRO_CFLAGS}
      ${PANGOCAIRO_CFLAGS}
      ${GTK3_CFLAGS})
    # Tell C++ that we're using Python.  There's no need to tell swig
    # that we're using Python.  It already knows.
    set_property(
      SOURCE ${swigfile}.swg
      PROPERTY GENERATED_COMPILE_DEFINITIONS
      OOFCANVAS_USE_PYTHON)
    if(${DEBUG})
      set_property(
	SOURCE ${swigfile}.swg
	PROPERTY GENERATED_COMPILE_DEFINITIONS
	DEBUG)
      set_property(
	SOURCE ${swigfile}.swg
	PROPERTY COMPILE_DEFINITIONS
	DEBUG)
    endif()
    if(${OOFCANVAS_USE_IMAGEMAGICK})
      # Preprocessor definitions for swig
      set_property(
	SOURCE ${swigfile}.swg
	PROPERTY COMPILE_DEFINITIONS
	OOFCANVAS_USE_IMAGEMAGICK)
      # Preprocessor definitions for the C++ compiler
      set_property(
	SOURCE ${swigfile}.swg
	PROPERTY GENERATED_COMPILE_DEFINITIONS
	OOFCANVAS_USE_IMAGEMAGICK)
      set_property(
	SOURCE ${swigfile}.swg
	PROPERTY GENERATED_COMPILE_OPTIONS
	${MAGICK_CFLAGS} APPEND)
    endif()
    if(${OOFCANVAS_USE_NUMPY})
      set_property(
	SOURCE ${swigfile}.swg
	APPEND
	PROPERTY COMPILE_DEFINITIONS
	OOFCANVAS_USE_NUMPY)
      set_property(
	SOURCE ${swigfile}.swg
	APPEND
	PROPERTY GENERATED_COMPILE_DEFINITIONS
	OOFCANVAS_USE_NUMPY)
    endif()
    swig_add_library(
      ${swigfile}
      TYPE MODULE
      LANGUAGE PYTHON
      SOURCES ${swigfile}.swg)
    set(alllibs ${Python${PYMAJOR}_LIBRARIES} ${SS_LIBRARIES})
    target_link_libraries(
      ${swigfile}
      PUBLIC
      ${alllibs})
    if(${OOFCANVAS_USE_IMAGEMAGICK})
      target_link_libraries(
	${swigfile}
	PUBLIC
	${MAGICK_LDFLAGS})
    endif()
    
    # Get the path from the top of the source directory hierarchy to
    # the current directory.  This is the path from the top of the
    # installation directory hierarchy to the installation directory
    # for the compiled swig output and python file.

    # file(RELATIVE_PATH ...) has been superseded by cmake_path(...)
    # in cmake 3.20, but 3.20 isn't available on Ubuntu 20.04.
    if(${CMAKE_VERSION} VERSION_LESS "3.20")
      file(
	RELATIVE_PATH relpath
	${PROJECT_SOURCE_DIR}/oofcanvas
	${CMAKE_CURRENT_SOURCE_DIR})
    else()
      set(relpath ${CMAKE_CURRENT_SOURCE_DIR})
      cmake_path(
	RELATIVE_PATH
	relpath
	BASE_DIRECTORY ${PROJECT_SOURCE_DIR}/oofcanvas)
    endif()
    # Install the swig-generated and compiled library
    install(
      TARGETS ${swigfile}
      DESTINATION ${PYDEST}/${relpath})
    # Install the swig-generated python file
    install(
      FILES ${CMAKE_CURRENT_BINARY_DIR}/${swigfile}.py
      DESTINATION ${PYDEST}/${relpath})
  endforeach()

endfunction(swig_sources)

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# In subdirectories, call set_public_headers with a list of header
# files that should be installed into <prefix>/include/oofcanvas.

# TODO: There is a correct way to do this, and this isn't it.  The
# correct way uses the PUBLIC_HEADER property, but I couldn't get it
# to work (perhaps because PUBLIC_HEADER is expecting a framework
# build on Macs?).

# Hacks on top of hacks.  I don't know if using a global property to
# create a global variable is recommended, but it feels wrong.  I
# don't know what's right. I found this recommended at
# https://stackoverflow.com/questions/10031953/how-to-set-the-global-variable-in-a-function-for-cmake

# Even more hacks.  The initial value of public_header_list is not set
# here.  It's initialzed to swigruntime.h, which is the only header
# that doesn't come from the source directory.  It was easier to do
# that than to figure out how to get set_public_headers to use a
# different directory.

function(set_public_headers)
  get_property(tmp GLOBAL PROPERTY public_header_list)
  foreach(header ${ARGV})
    set(tmp ${tmp} ${CMAKE_CURRENT_SOURCE_DIR}/${header})
  endforeach()
  set_property(GLOBAL PROPERTY public_header_list "${tmp}")
endfunction(set_public_headers)

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Set C++ version
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS False)	# use -std=c++11 instead of -std=gnu++11
set(CMAKE_CXX_STANDARD_REQUIRED True) # don't fall back to an earlier standard

set(BUILD_SHARED_LIBS ON)

# On macOS 12, we need to set RPATH or libraries installed outside the
# default locations won't be found.
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Find prerequisites.

# Without this line, cmake finds the system python on Mac even if
# MacPorts is installed.  With this line, it finds MacPorts' python,
# whether the argument is FIRST or LAST. This confuses me.  If
# Python_FIND_FRAMEWORK is used instead, it seems to always find the
# system python.
set(CMAKE_FIND_FRAMEWORK LAST)

# Find Python.
## If we include FindPython3 instead of FindPython here, then the
## latest version is found instead of the requested version. ??
include(FindPython)
if(${OOFCANVAS_PYTHON_API} STREQUAL Python3)
  set(PYMAJOR 3)
  if("${OOFCANVAS_PYTHON3_VERSION}" STREQUAL "Latest")
    find_package(Python3 COMPONENTS Interpreter Development)
  else()
    find_package(
      Python3 ${OOFCANVAS_PYTHON3_VERSION} EXACT
      COMPONENTS Interpreter Development)
  endif()
  set(OOFCANVAS_PYTHON3_VERSION ${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR})
  set(OOFCANVAS_PYTHON_VERSION ${OOFCANVAS_PYTHON3_VERSION})
  set(PYMINOR ${Python3_VERSION_MINOR})
else()
  set(PYMAJOR 2)
  set(PYMINOR 7)
  find_package(Python2 2.7 COMPONENTS Interpreter Development)
  set(OOFCANVAS_PYTHON_VERSION ${Python2_VERSION_MAJOR}.${Python2_VERSION_MINOR})
endif()

if(${PYMAJOR})
  set(PYEXEC ${Python${PYMAJOR}_EXECUTABLE})
  set(PYINCL ${Python${PYMAJOR}_INCLUDE_DIRS})
  set(PYLIBS ${Python${PYMAJOR}_LIBRARIES})
endif()

message("**** PYEXEC=${PYEXEC}")
# message("PYINCL= ${PYINCL}")
# message("PYLIBS= ${PYLIBS}")
# message("Python3 is ${Python3_EXECUTABLE}")
# message("Python3_INCLUDE_DIRS is ${Python3_INCLUDE_DIRS}")
# message("Python3_LIBRARIES is ${Python3_LIBRARIES}")
# message("Python3_LIBRARY_DIRS is ${Python3_LIBRARY_DIRS}")
# message("Python3_RUNTIME_LIBRARY_DIRS is ${Python3_RUNTIME_LIBRARY_DIRS}")
# message("Python3_SITELIB is ${Python3_SITELIB}")

set(PYLIBPATH python${PYMAJOR}.${PYMINOR}/site-packages/${CMAKE_PROJECT_NAME})
if(OOFCANVAS_SYSTEM_INSTALL)
  set(PYDEST ${Python3_LIBRARY_DIRS}/${PYLIBPATH})
else()
  set(PYDEST lib/${PYLIBPATH})
endif()

message("**** PYDEST=${PYDEST}")

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

include(FindSWIG)
include(UseSWIG)
find_package(SWIG ${OOFCANVAS_SWIG_VERSION} COMPONENTS python)
## UseSWIG can generate dependencies only for cmake >= 3.20 ? See
## https://cmake.org/cmake/help/latest/release/3.20.html
set(SWIG_USE_SWIG_DEPENDENCIES True)
set(SWIG_SOURCE_FILE_EXTENSIONS ".swg" ".i")
if("${OOFCANVAS_PYTHON_API}" STREQUAL "Python3" AND "${SWIG_VERSION}" VERSION_LESS "4.1")
  list(APPEND CMAKE_SWIG_FLAGS -py3)
endif()
if(${OOFCANVAS_SWIG_USE_BUILTIN})
  list(APPEND CMAKE_SWIG_FLAGS -builtin)
endif()
# UseSWIG doesn't seem to use -DNDEBUG or -DDEBUG when calling
# swig.  This has to be set with a generator expression because
# CMAKE_BUILD_TYPE can't be evaluated reliably at this time.
list(APPEND CMAKE_SWIG_FLAGS $<$<CONFIG:Debug>:-DDEBUG>)

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Use pkg-config to get info about dependencies

include(FindPkgConfig)

# MacPorts puts some pkg-config files in the python library
if(APPLE)
  set(ENV{PKG_CONFIG_PATH}
    "$ENV{PKG_CONFIG_PATH}:${Python${PYMAJOR}_LIBRARY_DIRS}/pkgconfig")
endif()
# Anaconda
if($ENV{CONDA_PREFIX})
  set(ENV{PKG_CONFIG_PATH}
    "$ENV{PKG_CONFIG_PATH}:$ENV{CONDA_PREFIX}/lib/pkgconfig")
endif()

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Check the numpy version and get its include path, if using scikit-image
if(${OOFCANVAS_USE_NUMPY}) 
  execute_process(
    COMMAND ${PYEXEC} -c "import numpy; print(numpy.version.version, end='')"
    OUTPUT_VARIABLE NUMPY_VERSION
  )
  if(${NUMPY_VERSION} VERSION_LESS ${NUMPY_MIN_VERSION})
    message(FATAL_ERROR
      "Numpy version is ${NUMPY_VERSION}. Version ${NUMPY_MIN_VERSION} is required.")
  endif()
  # Find the numpy include directory
  execute_process(
    COMMAND ${PYEXEC} -c "import numpy; print(numpy.get_include(), end='')"
    OUTPUT_VARIABLE NUMPY_INCLUDE
  )
endif()

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

pkg_check_modules(
  GTK3 REQUIRED
  gtk+-3.0>=${GTK3_MIN_VERSION})
pkg_check_modules(
  CAIRO REQUIRED
  cairomm-1.0>=${CAIRO_MIN_VERSION})
pkg_check_modules(
  PANGOCAIRO REQUIRED
  pangocairo>=${PANGOCAIRO_MIN_VERSION})

pkg_check_modules(
  PANGO REQUIRED
  pango>=${PANGO_MIN_VERSION})

# TODO: Don't include pygobject unless the GUI is being built.
if(NOT (${OOFCANVAS_PYTHON_API} STREQUAL None))
  pkg_check_modules(
    PYGOBJECT REQUIRED
    pygobject-3.0>${PYGOBJECT_MIN_VERSION})
  # add_compile_definitions(SWIG_TYPE_TABLE=oofcanvas)
endif()

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

if(${OOFCANVAS_USE_IMAGEMAGICK})
  pkg_check_modules(
    MAGICK REQUIRED
    Magick\+\+>=${MAGICK_MIN_VERSION}
    Magick\+\+<${MAGICK_MAX_VERSION})
endif()

## Dump all variables
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Create swigruntime.h by running "swig -external-runtime". The
# dependence of oofswigruntime.h on swigruntime.h is set explicitly in
# oofcanvas/CMakeLists.txt.

add_custom_command(
  OUTPUT swigruntime.h	# oof uses swigruntime.h
  COMMAND ${SWIG_EXECUTABLE} -python -external-runtime swigruntime.h
)

# Hack-ack-ack.  See comments for set_public_headers() above.
set_property(
  GLOBAL PROPERTY public_header_list
  ${CMAKE_CURRENT_BINARY_DIR}/swigruntime.h)

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Create the library targets.

add_library(oofcanvasCore SHARED)
add_library(oofcanvasGUI SHARED)

# When installed, rename the oofcanvasCore target to oofcanvas.  Cmake
# requires target names to be unique, and using oofcanvas from the
# start creates conflicts.

set_target_properties(oofcanvasCore
  PROPERTIES
  OUTPUT_NAME "oofcanvas")

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

configure_file(
  ${PROJECT_SOURCE_DIR}/oofcanvas/oofcanvas.h.in
  ${PROJECT_BINARY_DIR}/oofcanvas/oofcanvas.h)

configure_file(
  ${PROJECT_SOURCE_DIR}/oofcanvas/version.h.in
  ${PROJECT_BINARY_DIR}/oofcanvas/version.h)

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Generate the pkg-config file for oofcanvas.

# The list PKG_CONFIG_CFLAGS is expanded in oofcanvas.pc.in.
# OOFCANVAS_USE_IMAGEMAGICK and OOFCANVAS_USE_NUMPY are set in
# oofcanvas.h.in, and don't have to be added to the command line
# options in oofcanvas.pc.in.
if(${OOFCANVAS_USE_IMAGEMAGICK})
  list(APPEND PKG_CONFIG_CFLAGS ${MAGICK_CFLAGS_OTHER})
endif()
if(NOT ${OOFCANVAS_PYTHON_API} STREQUAL None)
  list(APPEND PKG_CONFIG_CFLAGS "-DOOFCANVAS_USE_PYTHON=${PYMAJOR}")
endif()
# change the ';'s in the list to spaces
string(REPLACE ";" " " PKG_CONFIG_CFLAGS "${PKG_CONFIG_CFLAGS}")
# Generate the file.
configure_file(
  ${PROJECT_SOURCE_DIR}/oofcanvas.pc.in
  ${PROJECT_BINARY_DIR}/oofcanvas.pc
  @ONLY)

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Set compilation and link flags for the oofcanvas libraries.

foreach(olib ${oofcanvaslibs})
  target_compile_options(${olib}
    PRIVATE
    "${GTK3_CFLAGS}"
    "${CAIRO_CFLAGS}"
    "${PANGOCAIRO_CFLAGS}"
    -Wno-deprecated-register
    )
  target_include_directories(${olib}
    PRIVATE
    "${PROJECT_BINARY_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}"
    ## TODO? Don't add all include dirs here. Use only as required by
    ## setting property INCLUDE_DIRECTORIES on source files
    "${GTK3_INCLUDE_DIRS}"
    "${PANGOCAIRO_INCLUDE_DIRS}"
    "${CAIRO_INCLUDE_DIRS}"
    )
  if(${OOFCANVAS_USE_IMAGEMAGICK})
    #message("Using ImageMagick, LDFLAGS=${MAGICK_LDFLAGS}")
    #message("LIBRARIES=${MAGICK_LIBRARIES}")
    target_compile_definitions(${olib}
      PUBLIC
      OOFCANVAS_USE_IMAGEMAGICK)
    target_compile_options(${olib}
      PRIVATE
      ${MAGICK_CFLAGS})
    target_include_directories(${olib}
      PUBLIC
      "${MAGICK_INCLUDE_DIRS}")
    target_link_options(${olib}
      PRIVATE
      ${MAGICK_LDFLAGS})
  endif()
  if(${OOFCANVAS_USE_NUMPY})
    target_compile_definitions(${olib}
      PUBLIC
      OOFCANVAS_USE_NUMPY)
    target_include_directories(${olib}
      PUBLIC
      ${NUMPY_INCLUDE})
  endif()
  if(NOT (${OOFCANVAS_PYTHON_API} STREQUAL None))
    target_compile_definitions(${olib}
      PUBLIC
      OOFCANVAS_USE_PYTHON=${PYMAJOR})
    target_include_directories(${olib}
      PUBLIC
      "${PYINCL}")
    target_link_libraries(${olib}
      PUBLIC
      ${PYLIBS})
  endif()
  if(${OOFCANVAS_SWIG_USE_BUILTIN})
    # Swig defines SWIGPYTHON_BUILTIN internally if -builtin is being
    # used.  We might need to know it when compiling other files.
    target_compile_definitions(${olib}
      PUBLIC
      SWIGPYTHON_BUILTIN)
  endif()
  if(${DEBUG})
    target_compile_definitions(${olib}
      PUBLIC
      DEBUG)
    # target_compile_options(${olib}
    #   PUBLIC
    #   UNDEBUG)
  endif()
  target_compile_options(${olib} PRIVATE -Wno-deprecated-register)
endforeach()

# Additional stuff needed by the gui part
target_include_directories(oofcanvasGUI
  PUBLIC
  ${PYGOBJECT_INCLUDE_DIRS}
  ${PANGO_INCLUDE_DIRS})
target_compile_options(oofcanvasGUI
  PUBLIC
  ${PYGOBJECT_CFLAGS}
  ${PANGO_CFLAGS})

target_link_libraries(
  oofcanvasCore
  PUBLIC
  ${${Python_Version}_LIBRARIES}
  ${CAIRO_LINK_LIBRARIES}
  ${GTK3_LINK_LIBRARIES}
  ${PANGOCAIRO_LINK_LIBRARIES})

target_link_libraries(
  oofcanvasGUI
  PRIVATE
  oofcanvasCore)

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Add C++ files to their library targets and tell swig about its input
# files.

add_subdirectory(oofcanvas)

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Install compiled libraries

install(
  TARGETS
  oofcanvasCore
  oofcanvasGUI
  LIBRARY DESTINATION lib
  )

install(
  FILES
  ${PROJECT_BINARY_DIR}/oofcanvas.pc
  DESTINATION lib/pkgconfig)

# Install the __init__.py files, which are the only pure python files
# in the project.

install(
  FILES
  ${PROJECT_SOURCE_DIR}/oofcanvas/__init__.py
  DESTINATION
  ${PYDEST})
install(
  FILES
  ${PROJECT_SOURCE_DIR}/oofcanvas/oofcanvasgui/__init__.py
  DESTINATION
  ${PYDEST}/oofcanvasgui)

## Install header files.  oofcanvas.h is special because it's created
## by configure_file() and is in <prefix>/include/oofcanvas.

install(
  FILES
  ${PROJECT_BINARY_DIR}/oofcanvas/oofcanvas.h
  ${PROJECT_SOURCE_DIR}/oofcanvas/oofcanvasgui/oofcanvasgui.h
  DESTINATION
  include/oofcanvas)

# The rest of the header files are installed in
# <prefix>/include/oofcanvas/oofcanvas.  The compiler is given
# -I<prefix>/include/oofcanvas and user code uses "#include
# <oofcanvas.h>" or "#include <oofcanvas/otherheader.h>"

# The header files passed to set_public_headers() are in a global
# property, which has to be copied to a variable before it can be
# used.  TODO: Is there a better way?
get_property(tmp GLOBAL PROPERTY public_header_list)
install(
  FILES
  ${tmp}
  ${PROJECT_BINARY_DIR}/oofcanvas/version.h
  DESTINATION
  include/oofcanvas/oofcanvas)
  
# Packaging

# Don't use cmake's packaging tools because I couldn't get them to
# work and we have a perfectly good make_dist script from the old
# days.  But in order to ensure that the version number used by the
# script is the same as the version number defined here, the script is
# created from make_dist.in.  Check that the file exists because
# make_dist.in isn't itself included in the distribution.

if(EXISTS ${PROJECT_SOURCE_DIR}/make_dist.in)
  configure_file(
    ${PROJECT_SOURCE_DIR}/make_dist.in
    ${PROJECT_BINARY_DIR}/make_dist
    @ONLY
  )
endif()

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Add an "uninstall" target.  Copied from
# https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#can-i-do-make-uninstall-with-cmake

if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
