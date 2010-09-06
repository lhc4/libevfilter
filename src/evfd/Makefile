# compilers and tools
CC=gcc
CFLAGS=-Wall -W -ggdb -I.. -L.. -levfilter

BINARIES=evfd

all: $(BINARIES)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

install:
	install -m 775 $(BINARIES) /usr/sbin/

clean:
	rm -rf $(BINARIES)
	
