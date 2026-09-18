CC = clang
CFLAGS = -Wall -Wextra -std=c11
LDLIBS = -lm

BIN = test_algorithms
OBJS = algorithms.o test_algorithms.o

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LDLIBS)

%.o: %.c algorithms.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BIN) $(OBJS)

.PHONY: all clean
