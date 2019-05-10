
CFILES = canvas.C utility.C oofcanvas_wrap.C
HFILES = canvas.h utility.h

OFILES = $(CFILES:.C=.o)

CXX = clang++

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

_oofcanvas.so: $(OFILES)
	$(CXX) -dylib -undefined dynamic_lookup -o $@ $(LDFLAGS) $(OFILES)

oofcanvas_wrap.C: oofcanvas.swg
	swig -python -c++ -o oofcanvas_wrap.C oofcanvas.swg

.PHONY: clean

clean:
	rm -f *.o oofcanvas_wrap* oofcanvas.py
