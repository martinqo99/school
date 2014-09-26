# Autor : Michal Lukac, xlukac05@stud.fit.vutbr.cz
# Popis : Makefile pre projekt do predmetu IPK na VUT FIT

CC=gcc
CFLAGS=-std=c99 -pedantic -g -I/usr/local/include/libxml2 -I/usr/local/include -L/usr/local/lib -lxml2 -lz -L/usr/local/lib -liconv
#HEAD=udt.h

making:
	$(CC) $(CFLAGS) rdtclient.c -o rdtclient
	$(CC) $(CFLAGS) rdtserver.c -o rdtserver
