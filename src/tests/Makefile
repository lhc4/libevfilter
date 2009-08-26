# compilers and tools
CC=gcc
CFLAGS=-Wall -W -ggdb -I.. -L.. -levfilter

BINARIES=param_loader load_all input_line draw select hotplug select_hotplug

all: $(BINARIES)

draw:
	$(CC) $(CFLAGS) `sdl-config --static-libs --cflags` -lSDL_gfx draw.c -o draw

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BINARIES)
	
