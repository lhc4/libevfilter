# compilers and tools
CC=gcc
CFLAGS=-fPIC -Wall -W -ggdb -DDEBUG -I.. -I../keyparser

# source files
CSOURCES=commit.c dump.c no_repeat.c pressure_to_key.c speed_mod_rel.c  \
         barrier.c weighted_average_abs.c scale_abs.c mirror.c rotate.c \
	 abs2rel.c btn2rel.c kalman.c rel2scroll.c copy_to_handle.c     \
	 get_from_handle.c key_lock.c \

# object files
OBJECTS=$(CSOURCES:.c=.o) 

all: $(OBJECTS)

# rules for compiling c sources
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# clean!
clean:
	rm -rf $(OBJECTS)
