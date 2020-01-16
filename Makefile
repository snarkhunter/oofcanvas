
.SUFFIXES:

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
LDFLAGS0 = -bundle -undefined dynamic_lookup
CPPLIB = liboofcanvas.dylib

else ifeq ($(ARCH), Linux)
CXX = g++ -fPIC
SWIG = /home/langer/OOF2/builddir-master-dist/temp.linux-x86_64-2.7-2d/swig-build/bin/swig
LDFLAGS0 = -shared
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
CXXFLAGS ::= $(CXXFLAGS) -g -DDEBUG
endif


ifdef PYTHON
CXXFLAGS := $(CXXFLAGS) -DPYTHON_OOFCANVAS \
	`pkg-config --cflags  python-2.7` \
        `pkg-config --cflags  pygobject-3.0`
LDFLAGS := $(LDFLAGS) `pkg-config --libs python-2.7`
OFILES := $(PYCFILES:.C=.o)

else
OFILES = $(CFILES:.C=.o)
endif


ifdef PYTHON

endif

.SUFFIXES: .o .C

all: oofcanvascmodule.so

%.o: %.C $(HFILES)
	# @echo ARCH "$(ARCH)"
	$(CXX) $(PYCFLAGS) -c $(CXXFLAGS) -o $@ $<

oofcanvascmodule.so $(CPPLIB): $(OFILES) $(HFILES)
	@echo Making $@
	$(CXX) $(LDFLAGS0) -o $@  $(OFILES) $(LDFLAGS)

oofcanvascmodule.C oofcanvas.py: oofcanvas.swg oofcanvas.spy $(HFILES)
	$(SWIG) $(SWIGARGS) -DPYTHON_OOFCANVAS -o oofcanvascmodule.C oofcanvas.swg

.PHONY: clean

clean:
	rm -f *.o oofcanvascmodule* oofcanvas.py

# Type "make print-VAR" to print the value of VAR
print-%:
	@echo $* = $($*)

