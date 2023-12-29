EXES = ctest

CXX := clang++
RTTIFLAG := -fno-rtti
CXXFLAGS := $(shell llvm-config --cxxflags) -Oz $(RTTIFLAG)
LLVMLDFLAGS := $(shell llvm-config --ldflags --libs)

OBJECTS = llcapi.o main.o

all: $(EXES)
.PHONY: all clean

llcapi.o: llcapi.cpp
	$(CXX) -fPIC $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) -Oz -c -o $@ $<

$(EXES): $(OBJECTS)
	$(CXX) -o $(EXES) $(OBJECTS) $(LLVMLDFLAGS)

clean:
	-rm -f $(OBJECTS) *~
	-rm -f $(EXES)

