PR = loader
CC = g++
CFLAGS = -Wall -Os -DUNIX=1
MFLAGS = -o $(PR)
LFLAGS = -lusb

all: $(PR)

OBJECTS = main.o Loader.o USBPes.o PesNet.o signal.o platform.o

$(PR): $(OBJECTS)
	$(CC) $(MFLAGS) $(OBJECTS) $(LFLAGS)
clean:
	rm -f *.o *~
%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

