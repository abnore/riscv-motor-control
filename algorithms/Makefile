CFLAGS = -Wall -Wextra -std=c11
LDLIBS :=
ifeq ($(shell uname -s),Linux)
	LDLIBS += -lm
endif

CFILES := $(wildcard *.c)
TEST_FILES := $(wildcard test_*.c)
BINS := $(TEST_FILES:.c=)

LIB_FILES := $(filter-out $(TEST_FILES),$(CFILES))
LIB_OBJS := $(LIB_FILES:.c=.o)
TEST_OBJS := $(TEST_FILES:.c=.o)

all: $(BINS)

$(BINS): %: %.o $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c motor_control.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BINS) $(LIB_OBJS) $(TEST_OBJS) *.log

.PHONY: all clean
