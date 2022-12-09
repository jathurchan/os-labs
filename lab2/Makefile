SRC = $(wildcard *.c)
BIN = $(patsubst %.c,%,$(SRC))

CFLAGS += -Wall -Wextra -g
#CFLAGS += --std=c99

all: $(BIN)

clean:
	rm -f $(BIN)
	rm -f *.o
	rm -f *~

indent:
	indent -linux -i4 -nut -ts2 *.c

.PHONY: all clean indent
