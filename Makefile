# Copyright 2005 Wojciech A. Koszek <wkoszek@FreeBSD.org>
SRCS=diskread.c geom_sunx86label_enc.c
CFLAGS+= -Wall -pedantic

all: diskread

diskread: $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS)
clean:
	rm -rf diskread
