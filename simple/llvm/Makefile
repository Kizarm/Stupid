PR = test
CC = gcc
CFLAGS = -Wall -Os -g
MFLAGS = -o $(PR)
LFLAGS = -ldl
PLIB = libpes.so

all: $(PR)

OBJECTS = main.o PesApi.o

$(PR): $(OBJECTS) $(PLIB)
	$(CC) $(MFLAGS) $(OBJECTS) $(LFLAGS)
clean:
	rm -f *.s *.o *~ *.ll

$(PLIB): simpledef.o test.o
	gcc -g -shared -fPIC simpledef.o test.o -o $(PLIB)
simpledef.o: simpledef.c
	gcc -g -c -Os -fPIC  $< -o $@
test.ll: test.c
	clang-3.5 -O0 -S -emit-llvm test.c -o test.ll
test.s: test.ll
	llc-3.5 -relocation-model=pic -O3 test.ll -o test.s

%.o: %.s
	as  $< -o $@
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

