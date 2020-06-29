CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

ktcc: $(OBJS)
	$(CC) -o ktcc $(OBJS) $(LDFLAGS)

$(OBJS): ktcc.h

run: ktcc
	./ktcc "playground/test.c" > tmp.s
	as -o mylibc.o mylibc.s
	cc -o tmp tmp.s mylibc.o
	./tmp

test: ktcc
	./test.sh

clean:
	rm -f ktcc *.o *~ tmp*

.PHONY: test clean
