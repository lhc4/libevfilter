# compilers and tools
CC=gcc
CFLAGS=-Wall -W -ggdb -I.. -L.. -levfilter

BINARY=evfd

$(BINARY): evfd.o evfd_lock.o

all: $(BINARY)

%: %.o
	$(CC) $(CFLAGS) $^ -o $@

%: %.c
	$(CC) $(CFLAGS) $< -o $@

install:
	install -m 775 $(BINARY) /usr/sbin/

clean:
	rm -rf $(BINARY) *.o
	
evfd_lock.o: evfd_lock.c evfd_lock.h
evfd.o: evfd.c evfd_lock.h evfd.h
evfd_lock.o: evfd_lock.c evfd_lock.h
