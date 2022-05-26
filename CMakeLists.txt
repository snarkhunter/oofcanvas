# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modifed
# versions of this software, you first contact the authors at
# oof_manager@nist.gov.

cmake_minimum_required(VERSION 3.16)
project(oofcanvas VERSION 1.1.0)

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Options to be set by the user

set(oof_build_types Debug Release)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Debug or Release")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${oof_build_types})

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set(DEBUG True)
endif()

option(OOFCANVAS_USE_IMAGEMAGICK "Use ImageMagick" ON)

## TODO: Allow user to choose python sub-version, not just 2 or 3.
set(pythonOptions None Python2 Python3)
set(PYTHON_API Python3 CACHE STRING "Generate the Python API")
set_property(CACHE PYTHON_API PROPERTY STRINGS ${pythonOptions})

# SWIG_USE_BUILTIN is experimental.  It won't work unless we've
# figured out how to avoid any monkeypatching of the swigged classes.
option(SWIG_USE_BUILTIN "Use the -builtin option for swig (experimental)" OFF)
#set(SWIG_USE_BUILTIN OFF)

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# Required version numbers for dependencies.

set(GTK3_MIN_VERSION 3.22)
set(CAIRO_MIN_VERSION 1.12)
set(PANGOCAIRO_MIN_VERSION 1.40)
set(PANGO_MIN_VERSION 1.40)
set(PYGOBJECT_MIN_VERSION 3.22)
set(MAGICK_MIN_VERSION 6.0)
set(MAGICK_MAX_VERSION 7.0) # must be less than this

#=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=##=--=#

# OOF2 libraries to build..  The cmake target is called
# "oofcanvasCore" instead of "oofcanvas" because we want the python
# interface to be "oofcanvas", and cmake target names must be unique.
# But it will be installed as "liboofcanvas.<suffix>" anyway.

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
  if(${PYTHON_API} STREQUAL None)
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
	PROPERTY COMPILE_DEFINITIONS OOFCANVAS_USE_IMAGEMAGICK)
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
    swig_add_library(
      ${swigfile}
      TYPE MODULE
      LANGUAGE PYTHON
      SOURCES ${swigfile}.swg)
    set(alllibs ${Python${PYMAJOR}_LIBRARIES} ${SS_LIBRARIES})
    swig_link_libraries(
      ${swigfile}
      PUBLIC
      ${alllibs})
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

# Set C++ version
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS False)	# use -std=c++11 instead of -std=gnu++11
set(CMAKE_CXX_STANDARD_REQUIRED True) # don't fall back to an earlier standard

set(BUILD_SHARED_LIBS ON)

set(CMAKE_FIND_FRAMEWORK LAST)	# Prefers MacPorts over system python

# Find Python.
if(${PYTHON_API} STREQUAL Python3)
  set(PYMAJOR 3)
  find_package(Python3 3.8 EXACT COMPONENTS Interpreter Development)
  set(PYTHON_VERSION ${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR})
elseif(${PYTHON_API} STREQUAL Python2)
  set(PYMAJOR 2)
  find_package(Python2 2.7 COMPONENTS Interpreter Development)
  set(PYTHON_VERSION ${Python2_VERSION_MAJOR}.${Python2_VERSION_MINOR})
endif()

if(${PYMAJOR})
  set(PYEXEC ${Python${PYMAJOR}_EXECUTABLE})
  set(PYINCL ${Python${PYMAJOR}_INCLUDE_DIRS})
  set(PYLIBS ${Python${PYMAJOR}_LIBRARIES})
endif()

#message("PYEXEC= ${PYEXEC}")
#message("PYINCL= ${PYINCL}")
#message("PYLIBS= ${PYLIBS}")

# Use a relative path so that DESTDIR mechanism works
set(PYDEST lib/python${PYTHON_VERSION}/site-packages/${CMAKE_PROJECT_NAME})
# message("PYDEST is ${PYDEST}")

include(FindSWIG)
include(UseSWIG)
find_package(SWIG 4.0 COMPONENTS python)
## UseSWIG can generate dependencies only for cmake >= 3.20 ? See
## https://cmake.org/cmake/help/latest/release/3.20.html
set(SWIG_SOURCE_FILE_EXTENSIONS ".swg" ".i")
if("${PYTHON_API}" STREQUAL "Python3")
  list(APPEND CMAKE_SWIG_FLAGS -py3)
endif()
if(${SWIG_USE_BUILTIN})
  list(APPEND CMAKE_SWIG_FLAGS -builtin)
endif()

# Use pkg-config to get info about dependencies
include(FindPkgConfig)
# MacPorts puts some pkg-config files in the python library
set(ENV{PKG_CONFIG_PATH}
  "$ENV{PKG_CONFIG_PATH}:${Python${PYMAJOR}_LIBRARY_DIRS}/pkgconfig")

pkg_check_modules(
  GTK3 REQUIRED
  gtk+-3.0>=${GTK3_MIN_VERSION})
pkg_check_modules(
  CAIRO REQUIRED
  cairomm-1.0>=${CAIRO_MIN_VERSION})
pkg_check_modules(
  PANGOCAIRO REQUIRED
  pangocairo>=${PANGOCAIRO_MIN_VERSION})

# TODO: Don't include pango and pygobject unless the GUI is being built.
pkg_check_modules(
  PANGO REQUIRED
  pango>=${PANGO_MIN_VERSION})

if(NOT (${PYTHON_API} STREQUAL None))
  pkg_check_modules(
    PYGOBJECT REQUIRED
    pygobject-3.0>${PYGOBJECT_MIN_VERSION})
  message("PYGOBJECT_INCLUDE_DIRS = ${PYGOBJECT_INCLUDE_DIRS}")
  add_compile_definitions(SWIG_TYPE_TABLE=oofcanvas)
endif()

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

add_library(oofcanvasCore SHARED)
add_library(oofcanvasGUI SHARED)

set_target_properties(oofcanvasCore
  PROPERTIES
  OUTPUT_NAME "oofcanvas")

configure_file(
  ${PROJECT_SOURCE_DIR}/oofcanvas/oofcanvas.h.in
  ${PROJECT_BINARY_DIR}/oofcanvas/oofcanvas.h)

# The list PKG_CONFIG_CFLAGS is expanded in oofcanvas.pc.in.
if(${OOFCANVAS_USE_IMAGEMAGICK})
  list(APPEND PKG_CONFIG_CFLAGS "-DOOFCANVAS_USE_IMAGEMAGICK")
endif()
if(NOT ${PYTHON_API} STREQUAL None)
  list(APPEND PKG_CONFIG_CFLAGS "-DOOFCANVAS_USE_PYTHON=${PYMAJOR}")
endif()
# change the ';'s in the list to spaces
string(REPLACE ";" " " PKG_CONFIG_CFLAGS "${PKG_CONFIG_CFLAGS}")
configure_file(
  ${PROJECT_SOURCE_DIR}/oofcanvas.pc.in
  ${PROJECT_BINARY_DIR}/oofcanvas.pc
  @ONLY)

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
  if(NOT (${PYTHON_API} STREQUAL None))
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
  if(${SWIG_USE_BUILTIN})
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
  DESTINATION lib)

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

## TODO PYTHON3: Install header files.
