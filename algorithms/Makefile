CC = clang
CFLAGS = -Wall -Wextra -std=c11

BIN = test_algorithms
CFILES := $(wildcard *.c)
OBJS := $(filter-out test_%.c, $(CFILES:.c=.o))

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LDLIBS)

%.o: %.c algorithms.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BIN) $(OBJS)

.PHONY: all clean
