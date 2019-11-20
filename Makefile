
.SUFFIXES:

CFILES = canvas.C utility.C canvasitem.C canvaslayer.C canvasrectangle.C \
         oofcanvascmodule.C canvassegments.C canvasshape.C canvassegment.C \
	 canvascircle.C swiglib.C

HFILES = canvas.h utility.h canvasitem.h canvaslayer.h canvasrectangle.h \
         canvassegment.h canvassegments.h swiglib.h canvascircle.h \
	 pythonexportable.h canvasshape.h

OFILES = $(CFILES:.C=.o)

ARCH := $(shell uname)

## Poor man's attempt at portability.  This should be done properly at
## some point.  Also, we probably shouldn't rely on the copy of SWIG
## in my copy of OOF2.
ifeq ($(ARCH), Darwin)
CXX = clang++ -Wno-deprecated-register
SWIG = /Users/langer/FE/OOF2/builddir-develop-cocoa-debug/temp.macosx-10.14-x86_64-2.7-2d/swig-build/bin/swig
LDFLAGS0 = -dylib -undefined dynamic_lookup

else ifeq ($(ARCH), Linux)
CXX = g++ -fPIC
SWIG = /home/langer/OOF2/builddir-master-dist/temp.linux-x86_64-2.7-2d/swig-build/bin/swig
LDFLAGS0 = -shared
endif

SWIGARGS = -shadow -dnone -python -c++ -c -DDEBUG

export PKG_CONFIG_PATH = /opt/local/Library/Frameworks/Python.framework/Versions/2.7/lib/pkgconfig/

CXXFLAGS = -std=c++11 \
           `pkg-config --cflags  cairomm-1.0`    \
           `pkg-config --cflags  python-2.7`     \
           `pkg-config --cflags  pygobject-2.0` \
           `pkg-config --cflags  gtk+-2.0` -g -DDEBUG       


LDFLAGS = `pkg-config --libs cairomm-1.0`       \
          `pkg-config --libs python-2.7`         \
          `pkg-config --libs gtk+-2.0`

.SUFFIXES: .o .C

all: oofcanvascmodule.so

%.o: %.c $(HFILES)
	echo ARCH "$(ARCH)"
	$(CXX) -c $(CXXFLAGS) -o $@ $<

oofcanvascmodule.so: $(OFILES) $(HFILES)
	$(CXX) $(LDFLAGS0) -o $@  $(OFILES) $(LDFLAGS)

oofcanvascmodule.C: oofcanvas.swg $(HFILES)
	$(SWIG) $(SWIGARGS) -o oofcanvascmodule.C oofcanvas.swg

.PHONY: clean

clean:
	rm -f *.o oofcanvascmodule* oofcanvas.py

# Type "make print-VAR" to print the value of VAR
print-%:
	@echo $* = $($*)

