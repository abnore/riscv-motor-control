CFLAGS = -Wall -Wextra -std=c11
LDLIBS :=
ifeq ($(shell uname -s),Linux)
	LDLIBS += -lm
endif

BIN = test_algorithms
CFILES := $(wildcard *.c)
OBJS := $(filter-out test_%.c, $(CFILES:.c=.o))

# Modify makefile, such that i dont have to do this, but can compile all tests
# independant
#	cc -std=c11 -Wall -Wextra -O2 \
#	    test_motor_control.c motor_control.c clarke_park.c pi_control.c svpwm.c \
#	    -lm -o test_motor_control
#	
#	./test_motor_control
all: $(BIN)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LDLIBS)

%.o: %.c algorithms.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BIN) $(OBJS)
	rm *.log

.PHONY: all clean
