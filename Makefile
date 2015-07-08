# Copyright 2005 Wojciech A. Koszek <wkoszek@FreeBSD.org>
SRCS=diskread.c geom_sunx86label_enc.c
CFLAGS+= -Wall -pedantic

all: diskread

diskread: $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

tests:
	./diskread ./data/disk/c1d1p0 > test/c1d1p0.o
	./diskread ./data/disk/c1d1p1 > test/c1d1p1.o
	./diskread ./data/disk/c1d1p2 > test/c1d1p2.o
	./diskread ./data/disk/c1d1p3 > test/c1d1p3.o
	./diskread ./data/disk/c1d1p4 > test/c1d1p4.o
	./diskread ./data/disk/c1d1s1 > test/c1d1s1.o
check:
	@echo "# comparing results (no output means OK)"
	@diff -u test/c1d1p0.t test/c1d1p0.o
	@diff -u test/c1d1p1.t test/c1d1p1.o
	@diff -u test/c1d1p2.t test/c1d1p2.o
	@diff -u test/c1d1p3.t test/c1d1p3.o
	@diff -u test/c1d1p4.t test/c1d1p4.o
	@diff -u test/c1d1s1.t test/c1d1s1.o

clean:
	rm -rf diskread
	rm -rf test/*.o
