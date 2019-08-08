#PR = stpasm
PR = ../lib/libasmapi.a
CC = gcc
#CC = clang-3.5
CXX= g++
CFLAGS  = -Os -g
CFLAGS += -Wall
#CFLAGS += -Wno-parentheses -Wno-write-strings
CFLAGS += -ffunction-sections -fdata-sections
MFLAGS = -o $(PR)
#LFLAGS = -lm
LFLAGS = -Wl,--gc-sections,-Map=stpasm.map

all: $(PR)

OBJECTS = AsmApi.o
OBJECTS+= EX.o
OBJECTS+= FILE.o
OBJECTS+= IO.o
OBJECTS+= LINK.o
OBJECTS+= OP.o
OBJECTS+= RES.o
OBJECTS+= ST.o

#OBJECTS+= DAS.o
#OBJECTS+= DS.o
#OBJECTS+= NL.o
#OBJECTS+= RELOC.o
#OBJECTS+= SEMI.o

#$(PR): $(OBJECTS)
#	$(CC) $(MFLAGS) $(OBJECTS) $(LFLAGS)

clean:
	rm -f *.o *~
%.o: %.c
	$(CC) -c $(CFLAGS) $<
%.o: %.cpp
	$(CXX) -c $(CFLAGS) $<

$(PR): $(OBJECTS)
	ar rcs $(PR) $(OBJECTS)
