# compilers and tools
CC=gcc
CFLAGS=-Wall -W -ggdb -I.. -L.. -levfilter

BINARIES=param_loader load_all input_line draw select hotplug select_hotplug

all: $(BINARIES)

draw: draw.c sdl_utils.c sdl_utils.h
	$(CC) $(CFLAGS) `sdl-config --static-libs --cflags` -lSDL_gfx draw.c sdl_utils.c -o draw

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BINARIES)
	
