CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

ktcc: $(OBJS)
	$(CC) -o ktcc $(OBJS) $(LDFLAGS)

$(OBJS): ktcc.h

test: ktcc
	./test.sh

clean:
	rm -f ktcc *.o *~ tmp*

.PHONY: test clean
