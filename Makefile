
CFILES = canvas.C utility.C
HFILES = canvas.h utility.h

OFILES = $(CFILES:.C=.o)

CXX = clang++

CXXFLAGS = `pkg-config --cflags cairomm-1.0` `pkg-config --cflags gtk+-2.0` -std=c++11
LDFLAGS = `pkg-config --libs cairomm-1.0` `pkg-config --libs gtk+-2.0`

%.o: %.c $(HFILES)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

liboofcanvas.dylib: $(OFILES)
	$(CXX) -dylib -undefined dynamic_lookup -o $@ $(LDFLAGS) $(OFILES)


.PHONY: clean

clean:
	rm -f *.o
