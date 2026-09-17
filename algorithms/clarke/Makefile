BINARY = clarke

CC = clang
OPT = -O0
CFLAGS = -Wall -Wextra -g 

CFILES = $(wildcard *.c)

#all: 
#	@echo $(CFILES)
all: $(BINARY)
	@$(info Compiling)

$(BINARY): $(CFILES)
	@$(CC) -o $@ $^

clean:
	@rm -rf $(BINARY) 

.PHONY: all clean
