EXES = ctest
SLIB = ../../lib/libllcstp.so

CXX := clang++
RTTIFLAG := -fno-rtti
CXXFLAGS := $(shell llvm-config --cxxflags) -Oz $(RTTIFLAG)
LLVMLDFLAGS := $(shell llvm-config --ldflags --libs) -ldl

OBJECTS = llcapi.o main.o

CLANGLIBS = -lpthread
CLANGLIBS+= -lz -lcurses

all: $(EXES)
.PHONY: clean

llcapi.o: llcapi.cpp
	$(CXX) -fPIC $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) -Oz -c -o $@ $<

$(EXES): $(OBJECTS)
	$(CXX) -o $(EXES) $(OBJECTS) $(CLANGLIBS) $(LLVMLDFLAGS)

clean:
	-rm -f $(OBJECTS) *~
	-rm -f $(EXES)

