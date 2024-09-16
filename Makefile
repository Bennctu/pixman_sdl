CC = gcc
CFLAGS = `pkg-config --cflags pixman-1 sdl2`
LDFLAGS = `pkg-config --libs pixman-1 sdl2`

CFLAGS += -fsanitize=address

SRCDIR = test
SRC = $(SRCDIR)/composite_test.c
TARGET = composite_test

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
