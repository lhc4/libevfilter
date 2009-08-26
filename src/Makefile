# compilers and tools
CC=gcc
CFLAGS=-fPIC -Wall -W -ggdb -DDEBUG -Ikeyparser

# source files
CSOURCES=evfilter_param.c evfilter_err.c evfilter_profile.c evfilter_filter.c\
         evfilter_loader.c evfilter_line.c linux_input.c evfilter_select.c   \
	 evfilter_hotplug.c
KEYPARSER_HEADERS=keyparser/key_parser.h keyparser/keys.h
HEADERS=$(shell ls ./*.h)

# object files
OBJECTS=$(CSOURCES:.c=.o) 

all: libevfilter.so

make_keyparser:
	@cd keyparser && $(MAKE)

make_filters:
	@cd filters && $(MAKE)

libevfilter.so: make_filters make_keyparser $(OBJECTS) $(HEADERS) $(KEYPARSER_HEADERS)
	$(CC) --shared -Wl,-soname -Wl,libevfilter.so.0 $(OBJECTS) keyparser/*.o filters/*.o -o libevfilter.so


# rules for compiling c sources
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# clean!
clean:
	rm -rf $(OBJECTS) libevfilter.so
	@cd keyparser && $(MAKE) clean
	@cd filters && $(MAKE) clean
