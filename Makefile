CC = g++
CFLAGS = -Wall -std=c++11
LIBS = -lncurses

TARGET = mandelbrot_animated
SOURCE = mandelbrot_animated.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean