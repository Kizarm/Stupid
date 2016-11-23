#PREFIX = arm-none-eabi-
PREFIX =

PR = test
CC = $(PREFIX)gcc
AS = $(PREFIX)as
SIZE    = $(PREFIX)size
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump

CFLAGS = -Wall -Os -g
MFLAGS = -o $(PR)
LFLAGS =
PLIB = libpes.so
TGTS = $(PLIB)

ifeq ($(PREFIX),)
TGTS   += $(PR)

all: $(TGTS)

LFLAGS += -ldl

OBJECTS = main.o PesApi.o

$(PR): $(OBJECTS)
	$(CC) $(MFLAGS) $(OBJECTS) $(LFLAGS)

$(PLIB): simpledef.o test.o
	$(CC) -g -shared -fPIC simpledef.o test.o -o $(PLIB) -lm
simpledef.o: simpledef.c
	$(CC) -g -c -Os -fPIC  $< -o $@
Test.mac.ll: Test.stp ../Simple
	../Simple i
test.s: test.ll
	llc-3.8 -relocation-model=pic -O3 test.ll -o test.s
else
all: $(TGTS)
# pro cortex-m4f s floating point unit
LLCFP = -mcpu=cortex-m4 -mattr=+fp-only-sp,+d16

LFLAGS += -T script.ld
LFLAGS += -nostartfiles
LFLAGS += -Wl,--gc-sections,-Map=pes.map -lm

CFLAGS += -mthumb -mcpu=cortex-m4
#CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
EFLAGS += -ffunction-sections -fdata-sections
EFLAGS += -Wa,-adhlns=$(@:%.o=%.lst)

$(PLIB): simpledef.o test.o
	$(CC) $(CFLAGS) simpledef.o test.o -o $(PLIB) $(LFLAGS)
	$(SIZE) $(PLIB)
	$(OBJCOPY) --strip-unneeded -O ihex  $(PLIB) pes.hex
	$(OBJDUMP) -h -S $(PLIB) > pes.lst
#simpledef.o: simpledef.c
#	$(CC) -g -c -Os -fPIC  $< -o $@
Test.mac.ll: Test.stp ../Simple
	../Simple f
test.s: test.ll
	llc-3.8 -O3 $(LLCFP) test.ll -o test.s
endif

test.ll: Test.mac.ll
	opt-3.8 -S -O3 Test.mac.ll -o test.ll

%.o: %.s
	$(AS)  $< -o $@
%.o: %.c
	$(CC) -c $(CFLAGS) $(EFLAGS) $< -o $@

clean:
	rm -f *.s *.o *.so *~ *.ll *.lst *.map test
