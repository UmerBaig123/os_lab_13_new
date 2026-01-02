CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
TARGET = myfs

SOURCES = myfs.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c ext2.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS) $(TARGET) *.img

.PHONY: all clean
