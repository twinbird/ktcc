CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

ktcc: $(OBJS)
	$(CC) -o ktcc $(OBJS) $(LDFLAGS)

$(OBJS): ktcc.h

run: ktcc
	./ktcc "playground/test.c" > tmp.s
	./ktcc "mylibc/mylibc.c" > tmp_mylibc.s
	as -o mylibc.o mylibc/mylibc_asm.s tmp_mylibc.s
	cc -o tmp tmp.s mylibc.o
	./tmp

test: ktcc
	./test.sh

clean:
	rm -f ktcc *.o *~ tmp* *.s

.PHONY: test clean
