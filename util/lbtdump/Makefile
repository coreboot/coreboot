TARGET=lbtdump

CC=gcc
CFLAGS=-g -O -Wall

all: $(TARGET)

$(TARGET): *.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)


