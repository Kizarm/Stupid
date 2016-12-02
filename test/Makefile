PREFIX = arm-none-eabi-
#PREFIX =
ISF ?= Test

PR = test
CC = $(PREFIX)gcc
AS = $(PREFIX)as
LD = $(PREFIX)ld
SIZE    = $(PREFIX)size
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump

CFLAGS = -Wall -Os -g
MFLAGS = -o $(PR)
LFLAGS =
PLIB = libpes.so
TGTS = $(PLIB)

VPATH += ../simple/llvm

ifeq ($(PREFIX),)
TGTS   += $(PR)

all: $(TGTS)

LFLAGS += -ldl

OBJECTS = main.o PesApi.o

$(PR): $(OBJECTS)
	$(CC) $(MFLAGS) $(OBJECTS) $(LFLAGS)

$(PLIB): test.o
	$(LD) -shared -fPIC test.o -o $(PLIB)
$(ISF).mac.ll: $(ISF).stp ../Simple
	../Simple $(ISF).stp i
test.s: test.ll
	llc-3.8 -relocation-model=pic -O3 test.ll -o test.s
else
all: $(TGTS)
# pro cortex-m4f s floating point unit
LLCFP = -mcpu=cortex-m4 -mattr=+fp-only-sp,+d16

LFLAGS += -T script.ld
#LFLAGS += -nostartfiles
LFLAGS += --gc-sections -Map=pes.map

CFLAGS += -mthumb -mcpu=cortex-m4
#CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
EFLAGS += -ffunction-sections -fdata-sections
EFLAGS += -Wa,-adhlns=$(@:%.o=%.lst)

$(PLIB): test.o
	$(LD) $(LFLAGS) test.o -o $(PLIB)
	$(SIZE) $(PLIB)
	$(OBJCOPY) --strip-unneeded -O ihex  $(PLIB) pes.hex
	$(OBJDUMP) -h -S $(PLIB) > pes.lst
$(ISF).mac.ll: $(ISF).stp ../Simple
	../Simple $(ISF).stp f
test.s: test.ll
	llc-3.8 -function-sections -O3 $(LLCFP) test.ll -o test.s
endif

test.ll: $(ISF).mac.ll
	opt-3.8 -S -O3 $(ISF).mac.ll -o test.ll

%.o: %.s
	$(AS)  $< -o $@
%.o: %.c
	$(CC) -c $(CFLAGS) $(EFLAGS) $< -o $@

clean:
	rm -f *.s *.o *.so *~ *.ll *.lst *.map *.hex *.err test
