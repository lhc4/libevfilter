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

# install
INSTALL_HEADERS=evfilter.h evfilter_err.h evfilter_filter.h evfilter_hotplug.h        \
                evfilter_line.h evfilter_loader.h evfilter_param.h evfilter_profile.h \
                evfilter_select.h evfilter_struct.h linux_input.h

install: libevfilter.so
	install libevfilter.so /usr/lib/
	ln -s /usr/lib/libevfilter.so /usr/lib/libevfilter.so.0
	install -d /usr/include/evfilter/
	install $(INSTALL_HEADERS) /usr/include/evfilter/ 

# clean!
clean:
	rm -rf $(OBJECTS) libevfilter.so
	@cd keyparser && $(MAKE) clean
	@cd filters && $(MAKE) clean
