appname := mceliese

CXX := g++
CXXFLAGS  := -std=c++11 -O2 -Wall
LDLIBS := 
LOCALLIBS := 
INCLUDES  := 

srcfiles := $(shell find . -name "*.cpp")
objects  := $(patsubst %.C, %.o, $(srcfiles))

all: $(appname)

$(appname): $(objects)
	$(CXX) $(CXXFLAGS) -o $(appname) $(objects) $(LDLIBS) $(LOCALLIBS) $(INCLUDES)

depend: .depend

.depend: $(srcfiles)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	rm -f $(objects)

dist-clean: clean
	rm -f *~ .depend

include .depend
