# compilers and tools
CC=gcc
CFLAGS=-Wall -W -ggdb -I.. -L.. -levfilter

BINARIES=evfd

all: $(BINARIES)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BINARIES)
	
