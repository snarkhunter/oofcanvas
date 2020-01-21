# This software was produced by NIST, an agency of the
# U.S. government, and by statute is not subject to copyright in the
# United States. Recipients of this software assume all
# responsibilities associated with its operation, modification and
# maintenance. However, to facilitate maintenance we ask that before
# distributing modified versions of this software, you first contact
# the authors at oof_manager@nist.gov.

# This Makefile should be run with gnu make.

# To build the Python version, use "make USE_PYTHON=1".  This will build
# oofcanvascmodule.so and oofcanvas.py.  The actual value of USE_PYTHON
# doesn't matter -- it just has to be defined.

# To build the C++ version, just type "make", which will
# liboofcanvas.so on Linux and liboofcanvas.dylib on Mac.

# Run "make clean" before switching from the Python version to the C++
# version or vice versa.

#.SUFFIXES:

CFILES = canvas.C utility.C canvasitem.C canvaslayer.C canvasrectangle.C \
         canvassegments.C canvasshape.C canvassegment.C \
	 canvascircle.C canvaspolygon.C canvastext.C

# Additional C++ files needed for the Python interface
PYCFILES = oofcanvascmodule.C swiglib.C $(CFILES)

HFILES = canvas.h utility.h canvasitem.h canvaslayer.h canvasrectangle.h \
         canvassegment.h canvassegments.h swiglib.h canvascircle.h \
	 pythonexportable.h canvasshape.h canvaspolygon.h canvastext.h

ARCH := $(shell uname)

## Poor man's attempt at portability.  This should be done properly at
## some point.  Also, we probably shouldn't rely on the copy of SWIG
## in my copy of OOF2.
ifeq ($(ARCH), Darwin)
CXX = clang++ -Wno-deprecated-register
SWIG = /Users/langer/FE/OOF2/builddir-develop-cocoa-debug/temp.macosx-10.14-x86_64-2.7-2d/swig-build/bin/swig
ifdef USE_PYTHON
LDFLAGS0 = -bundle -undefined dynamic_lookup
else
LDFLAGS0 = -dynamiclib -undefined dynamic_lookup
endif
CPPLIB = liboofcanvas.dylib

else ifeq ($(ARCH), Linux)
CXX = g++ -fPIC
SWIG = /home/langer/OOF2/builddir-master-dist/temp.linux-x86_64-2.7-2d/swig-build/bin/swig
LDFLAGS0 = -shared
LDFLAGS1 = -shared # ?
CPPLIB = liboofcanvas.so
endif

SWIGARGS = -shadow -dnone -python -c++ -c -DDEBUG

export PKG_CONFIG_PATH = /opt/local/Library/Frameworks/Python.framework/Versions/2.7/lib/pkgconfig/

CXXFLAGS = -std=c++11 \
           `pkg-config --cflags  cairomm-1.0`    \
           `pkg-config --cflags  gtk+-3.0`

LDFLAGS = `pkg-config --libs cairomm-1.0`       \
          `pkg-config --libs gtk+-3.0`

ifdef DEBUG
CXXFLAGS := $(CXXFLAGS) -g -DDEBUG
endif


ifdef USE_PYTHON
CXXFLAGS := $(CXXFLAGS) -DPYTHON_OOFCANVAS \
	`pkg-config --cflags  python-2.7` \
        `pkg-config --cflags  pygobject-3.0`
LDFLAGS := $(LDFLAGS) `pkg-config --libs python-2.7`
OFILES := $(PYCFILES:.C=.o)
TARGET = oofcanvascmodule.so

else
OFILES = $(CFILES:.C=.o)
TARGET := $(CPPLIB)
endif

.SUFFIXES: .o .C

$(TARGET): $(OFILES) $(HFILES)
	@echo Making $@
	$(CXX) $(LDFLAGS0) -o $@  $(OFILES) $(LDFLAGS)

%.o: %.C $(HFILES)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

oofcanvascmodule.C oofcanvas.py: oofcanvas.swg oofcanvas.spy $(HFILES)
	$(SWIG) $(SWIGARGS) -DPYTHON_OOFCANVAS -o oofcanvascmodule.C oofcanvas.swg


canvastest: canvastest.o $(TARGET)
	$(CXX) -o $@ $<  -L. -loofcanvas $(LDFLAGS)

.PHONY: clean

clean:
	rm -f *.o oofcanvascmodule* oofcanvas.py $(CPPLIB)

# Type "make print-VAR" to print the value of VAR
print-%:
	@echo $* = $($*)

