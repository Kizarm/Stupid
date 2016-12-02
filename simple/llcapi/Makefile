EXES = ctest
SLIB = libllcapi.so

CXX := clang++-3.8
RTTIFLAG := -fno-rtti
CXXFLAGS := $(shell llvm-config-3.8 --cxxflags) $(RTTIFLAG)
LLVMLDFLAGS := $(shell llvm-config-3.8 --ldflags --libs) -ldl

OBJECTS = main.o

CLANGLIBS = -lpthread
CLANGLIBS+= -lz -lcurses

SOBJS = llcapi.o

all: $(EXES)
.PHONY: clean

llcapi.o: llcapi.cpp
	$(CXX) -fPIC $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) -O3 -c -o $@ $<

$(SLIB): $(SOBJS)
	$(CXX) -o $(SLIB) -shared -fPIC $(SOBJS) $(CLANGLIBS) $(LLVMLDFLAGS)
	
$(EXES): $(OBJECTS) $(SLIB)
	$(CXX) -o $(EXES) $(OBJECTS) -L. -lllcapi

clean:
	-rm -f $(OBJECTS) $(SOBJS) *~
	-rm -f $(EXES) $(SLIB)

