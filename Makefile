TARGET = cd

CROSS_COMPILE =

CC = $(CROSS_COMPILE)g++

OBJS = config.o input.o video.o font.o decoder.o audio.o main.o

VPATH = ./

CFLAGS = -I. -O2 `/usr/bin/sdl-config --cflags`

LDFLAGS = `/usr/bin/sdl-config --libs` -lasound -lSDL_ttf

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@
	@echo Done!!!

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET) *.cfg