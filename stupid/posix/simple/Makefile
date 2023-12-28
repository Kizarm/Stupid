#PR = ../Simple
PR = ../lib/libstpapi.a
CC = gcc
CXX= g++
#CXX= clang++-3.5
CFLAGS  = -Os -DUNIX
CFLAGS += -Wall
#CFLAGS += -Wno-write-strings
MFLAGS = -o $(PR)
LFLAGS = -L../lib -lasmapi -lllcapi

VPATH += ./llib

all: $(PR)

OBJECTS = Simple.o
OBJECTS+= Cmdline.o
OBJECTS+= CfgTypes.o
OBJECTS+= Konst.o
OBJECTS+= Konstdef.o
OBJECTS+= Lexem.o
OBJECTS+= Macrodef.o
OBJECTS+= Optimizer.o
OBJECTS+= Predef.o
OBJECTS+= Prikaz.o
OBJECTS+= Procdef.o
OBJECTS+= 8051Processor.o
OBJECTS+= LLVMProcessor.o
#OBJECTS+= LLVMStack.o
OBJECTS+= llvmcomponent.o

OBJECTS+= SynAn.o
OBJECTS+= Typdef.o
OBJECTS+= Vardef.o
OBJECTS+= Varref.o
OBJECTS+= Vyhodnot.o
OBJECTS+= Vypis.o
OBJECTS+= Vyraz.o

OBJECTS+= 60chyby.o
OBJECTS+= 60lexem.o
OBJECTS+= 60list.o
OBJECTS+= 60zasob.o
OBJECTS+= 60znak.o
OBJECTS+= restore.o
OBJECTS+= context.o
#OBJECTS+= main.o

PLATFRM := $(shell uname -m)
ifeq ($(PLATFRM), x86_64)
CFLAGS += -DPC64=1
OBJECTS+= peslib64.o
else ifeq ($(PLATFRM), armv7l)
CFLAGS += -DARM32=1
OBJECTS+= peslib.o
endif


#$(PR): $(OBJECTS)
#	$(CXX) $(MFLAGS) $(OBJECTS) $(LFLAGS)
clean:
	rm -f *.o *~
%.o: %.C
	$(CC) -c $(CFLAGS) $<
%.o: %.cpp
	$(CXX) -std=c++17 -c $(CFLAGS) $<
%.o: %.s
	as $< -o $@
context.s: ./wasm/libWASM.a
./wasm/libWASM.a:
	cd ./wasm && $(MAKE) all

$(PR): $(OBJECTS)
	ar rcs $(PR) $(OBJECTS)
