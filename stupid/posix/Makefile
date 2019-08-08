PR = Simple
CC = gcc
CXX= g++
CFLAGS  = -Os -g
CFLAGS += -Wall
CFLAGS += -I./lib

MFLAGS = -o $(PR)
LFLAGS = -L./lib -lstpapi -lasmapi

ifeq ($(OS),Windows_NT)
REMOVE = DEL
LIBA = .\lib\libasmapi.a
LIBB = .\lib\libstpapi.a
else
REMOVE = rm -f
LIBA = ./lib/libasmapi.a
LIBB = ./lib/libstpapi.a
endif

all: $(PR)


OBJECTS = main.o

$(PR): $(OBJECTS) $(LIBA) $(LIBB)
	$(CXX) $(MFLAGS) $(OBJECTS) $(LFLAGS)
%.o: %.c
	$(CC) -c $(CFLAGS) $<
%.o: %.cpp
	$(CXX) -c $(CFLAGS) $<

ifeq ($(OS),Windows_NT)
$(LIBA):
	cd .\stpasm && $(MAKE) all
$(LIBB):
	cd .\simple && $(MAKE) all
else
$(LIBA):
	cd ./stpasm && $(MAKE) all
$(LIBB):
	cd ./simple && $(MAKE) all
endif

clean:
	$(REMOVE) *.o *~
## jen Unix
distclean: clean
	cd ./stpasm && $(MAKE) clean
	cd ./simple && $(MAKE) clean
	$(REMOVE) $(PR) $(LIBA) $(LIBB)
