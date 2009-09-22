#
# Global Makefile
#

# 
# make all binaries
#
all:
	make -C src

#
# clean 
#
clean:
	make -C src clean

#
# install
#
install:
	make -C src install

tar:
	make clean
	cd .. && tar czf libevfilter.tgz libevfilter/
