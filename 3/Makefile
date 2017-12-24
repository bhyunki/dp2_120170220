BINDIR =	/usr/local/sbin
MANDIR =	/usr/local/man/man8
CC =		cc
CFLAGS =	-O -ansi -pedantic -U__STRICT_ANSI__ -Wall -Wpointer-arith -Wshadow -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wno-long-long -std=c99 -lpthread
#SYSVLIBS =	-lnsl -lsocket
LDFLAGS =	-s $(SYSVLIBS) -lpthread

all:		micro_httpd client

micro_httpd:	micro_httpd.o
	$(CC) micro_httpd.o $(LDFLAGS) -o micro_httpd

micro_httpd.o:	micro_httpd.c
	$(CC) $(CFLAGS) -c micro_httpd.c

client:
	gcc -o client client.c -lpthread

install:	all
	rm -f $(BINDIR)/micro_httpd
	cp micro_httpd $(BINDIR)/micro_httpd
	rm -f $(MANDIR)/micro_httpd.8
	cp micro_httpd.8 $(MANDIR)/micro_httpd.8

clean:
	rm -f micro_httpd *.o core core.* *.core
	rm -f client
