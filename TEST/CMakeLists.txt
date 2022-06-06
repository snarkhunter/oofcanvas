# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modifed
# versions of this software, you first contact the authors at
# oof_manager@nist.gov.

cmake_minimum_required(VERSION 3.16)
project(canvastest VERSION 0.0.0)

set(oof_build_types Debug Release)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Debug or Release")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${oof_build_types})

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set(DEBUG True)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS False)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FindPkgConfig)
set(ENV{PKG_CONFIG_PATH}
  "$ENV{PKG_CONFIG_PATH}:$ENV{HOME}/lib/pkgconfig")
pkg_check_modules(OOFCANVAS REQUIRED oofcanvas>=1.1.0)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0>=3.22)

pkg_check_modules(CAIRO REQUIRED cairomm-1.0>=1.12)
pkg_check_modules(PANGOCAIRO REQUIRED pangocairo>=1.40)
pkg_check_modules(PANGO REQUIRED pango>=1.40)

## Dump all variables
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

add_executable(canvastest canvastest.C)

if(${DEBUG})
  target_compile_definitions(canvastest PUBLIC DEBUG)
endif()

target_compile_options(canvastest PRIVATE ${OOFCANVAS_CFLAGS})

target_include_directories(canvastest
  PRIVATE
  ${PROJECT_SOURCE_DIR}
  ${OOFCANVAS_INCLUDE_DIRS})

target_link_libraries(canvastest
  PRIVATE
  ${OOFCANVAS_LINK_LIBRARIES})

install(
  TARGETS
  canvastest)


add_executable(texttest texttest.C)
target_compile_options(texttest
  PRIVATE
  ${GTK3_CFLAGS}
  ${CAIRO_CFLAGS}
  ${PANGOCAIRO_CFLAGS})
target_include_directories(texttest
  PRIVATE
  ${GTK3_INCLUDE_DIRS}
  ${PANGOCAIRO_INCLUDE_DIRS},
  ${CAIRO_INCLUDE_DIRS})

target_link_libraries(texttest
  ${GTK3_LINK_LIBRARIES}
  ${CAIRO_LINK_LIBRARIES}
  ${PANGOCAIRO_LINK_LIBRARIES})

install(
  TARGETS
  texttest)
