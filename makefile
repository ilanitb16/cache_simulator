CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = cache_simulator
SOURCES = main.c cache.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

