PR = ../Simple
#PR = ../lib/libstpapi.a
CC = gcc
CXX= g++
#CXX= clang++-3.5
CFLAGS  = -Os
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

OBJECTS+= main.o

OBJECTS+= peslib.o

$(PR): $(OBJECTS)
	$(CXX) $(MFLAGS) $(OBJECTS) $(LFLAGS)
clean:
	rm -f *.o *~
%.o: %.C
	$(CC) -c $(CFLAGS) $<
%.o: %.cpp
	$(CXX) -c $(CFLAGS) $<
%.o: %.s
	as $< -o $@

#$(PR): $(OBJECTS)
#	ar rcs $(PR) $(OBJECTS)
