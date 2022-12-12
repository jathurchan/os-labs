CFLAGS := -g -Wall -Wextra
LDFLAGS := -L.
LDLIBS := -lseg

BIN := segclt

all: $(BIN)

segclt.o : segclt.c segdef.h

clean:
	rm -f $(BIN)
	rm -f *.o
	rm -f *~

indent:
	indent -linux -i4 -nut -ts2 *.c

.PHONY: all clean indent
