BINS = park clarke

CC = clang
CFLAGS = -Wall -Wextra -std=c11

all: $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(BINS)

.PHONY: all clean
