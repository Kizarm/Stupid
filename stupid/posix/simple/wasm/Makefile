LIB = libWASM.a
CC = clang -fPIC
CX = clang++ -std=c++14 -fno-exceptions -fno-rtti -fPIC
AR = llvm-ar
INCLUDES = -I.
TARGET = --target=wasm32-unknown-unknown
CFLAGS = -Oz -flto -Wall $(TARGET) -ffunction-sections -fdata-sections $(INCLUDES)

OBJS = heap.o newdel.o hack.o printf.o runtime.o timer.o

all: $(LIB)

%.o: %.cpp
	$(CX) -c $(CFLAGS) $< -o $@

$(LIB): $(OBJS)
	rm -f $(LIB)
	$(AR) rcs $(LIB) $(OBJS)
clean:
	rm -f *.o $(LIB)
.PHONY: all clean
