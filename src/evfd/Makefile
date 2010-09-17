# compilers and tools
CC=gcc
CFLAGS=-Wall -W -ggdb -I.. -L.. -levfilter

BINARY=evfd

$(BINARY): evfd.o evfd_msg.o evfd_lock.o

all: $(BINARY)

%: %.o
	$(CC) $(CFLAGS) $^ -o $@

%: %.c
	$(CC) $(CFLAGS) $< -o $@

install:
	install -m 775 $(BINARIES) /usr/sbin/

clean:
	rm -rf $(BINARY) *.o
	
