# compilers and tools
CC=gcc
CFLAGS=-fPIC -Wall -Wall -ggdb -DDEBUG -Ikeyparser

# source files
CSOURCES=evf_param.c evf_err.c evf_profile.c evf_filter.c     \
         evf_loader.c evf_line.c evf_input.c evf_io_queue.c   \
         evf_hotplug.c evf_uinput.c evf_handle.c evf_func.c   \
         config.c
KEYPARSER_HEADERS=keyparser/key_parser.h keyparser/keys.h
HEADERS=$(shell ls ./*.h)

# object files
OBJECTS=$(CSOURCES:.c=.o) 

all: tags libevfilter.so make_evfd

make_keyparser:
	@cd keyparser && $(MAKE)

make_filters:
	@cd filters && $(MAKE)

make_evfd:
	@cd evfd && $(MAKE)

libevfilter.so: make_filters make_keyparser $(OBJECTS) $(HEADERS) $(KEYPARSER_HEADERS)
	$(CC) --shared -Wl,-soname -Wl,libevfilter.so.0 $(OBJECTS) keyparser/*.o filters/*.o -o libevfilter.so


# rules for compiling c sources
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# update tags on make
tags: $(CSOURCES)
	./mktags.sh

# install
INSTALL_HEADERS=evfilter.h evf_err.h evf_filter.h evf_hotplug.h      \
                evf_line.h evf_loader.h evf_param.h evf_profile.h    \
                evf_io_queue.h evf_struct.h evf_input.h evf_uinput.h \
		evf_msg.h evf_func.h

install: libevfilter.so
	install -d $(RPM_BUILD_ROOT)/usr/lib
	install libevfilter.so $(RPM_BUILD_ROOT)/usr/lib/
	if ! [ -e /usr/lib/libevfilter.so.0 ]; then \
	ln -s libevfilter.so $(RPM_BUILD_ROOT)/usr/lib/libevfilter.so.0; \
	fi
	install -d $(RPM_BUILD_ROOT)/usr/include/evfilter/
	install -m 644 $(INSTALL_HEADERS) $(RPM_BUILD_ROOT)/usr/include/evfilter/
	@cd evfd && $(MAKE) install

# clean!
clean:
	rm -rf $(OBJECTS) libevfilter.so
	@cd keyparser && $(MAKE) clean
	@cd filters && $(MAKE) clean
	@cd evfd && $(MAKE) clean
