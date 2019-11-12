
CFILES = canvas.C utility.C canvasitem.C canvaslayer.C canvasrectangle.C \
         oofcanvascmodule.C canvassegments.C swiglib.C

HFILES = canvas.h utility.h canvasitem.h canvaslayer.h canvasrectangle.h \
         canvassegments.h swiglib.h

OFILES = $(CFILES:.C=.o)

CXX = clang++

SWIG = /Users/langer/FE/OOF2/builddir-develop-cocoa-debug/temp.macosx-10.14-x86_64-2.7-2d/swig-build/bin/swig
SWIGARGS = -shadow -dnone -python -c++ -c -DDEBUG

export PKG_CONFIG_PATH = /opt/local/Library/Frameworks/Python.framework/Versions/2.7/lib/pkgconfig/

CXXFLAGS = -std=c++11 -Wno-deprecated-register  \
           `pkg-config --cflags cairomm-1.0`    \
           `pkg-config --cflags gtk+-2.0`       \
           `pkg-config --cflags python-2.7`     \
           `pkg-config --cflags pygobject-2.0`


LDFLAGS = `pkg-config --libs cairomm-1.0`       \
          `pkg-config --libs gtk+-2.0`          \
          `pkg-config --libs python-2.7`

%.o: %.c $(HFILES)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

oofcanvascmodule.so: $(OFILES) $(HFILES)
	$(CXX) -dylib -undefined dynamic_lookup -o $@ $(LDFLAGS) $(OFILES)

oofcanvascmodule.C: oofcanvas.swg $(HFILES)
	$(SWIG) $(SWIGARGS) -o oofcanvascmodule.C oofcanvas.swg
#	swig.x -python -c++ -o oofcanvas_wrap.C oofcanvas.swg

gtktester: gtktester.o $(OFILES)
	$(CXX) -o gtktester gtktester.o $(OFILES) $(LDFLAGS)

stackoverflow-example: stackoverflow-example.o
	$(CXX) -o stackoverflow-example stackoverflow-example.o $(LDFLAGS)

# Simple versions don't use python or OOFCanvas, just plain gtk and cairomm
C_SIMPLE = -std=c++11 -Wno-deprecated-register \
            `pkg-config --cflags cairomm-1.0`         \
            `pkg-config --cflags gtk+-2.0`
L_SIMPLE = `pkg-config --libs cairomm-1.0` `pkg-config --libs gtk+-2.0`

gtktester2.o: gtktester2.C
	$(CXX) -c $(C_SIMPLE) -o gtktester2.o gtktester2.C

gtktester2: gtktester2.o
	$(CXX) -o gtktester2 gtktester2.o $(L_SIMPLE)

.PHONY: clean

clean:
	rm -f *.o oofcanvascmodule* oofcanvas.py _oofcanvas.so
