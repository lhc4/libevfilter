#!/bin/bash
set -x
DIR="/tmp/evfilter"

DEST="$HOME/rpmbuild/SOURCES"
if [ ! -d "$DEST" ]
then
	DEST="/usr/src/packages/SOURCE"
fi

# remove old stuff from $DIR
if [ -e "$DIR" ]
then
	rm -rf "$DIR"
fi

# still there ?
if [ -e "$DIR" ]
then
	echo STDERR "Could not delete $DIR."
	exit 1
fi

# clean up
pushd src
make clean
cd tests
make clean
popd

# read version
VERSION=`perl -ne 'print $1 if $_=~/^Version:\s+([^\s]+)/;' evfilter.spec`

# copy files
cp -a src "$DIR/"
BASE=`basename "$DIR"`
pushd "$DIR"
cd ..
tar cf - "$BASE" | bzip2 -f > "$DEST/evfilter-$VERSION.tar.bz2"
rm -rf "$DIR"
popd

# build
rpmbuild -ba evfilter.spec
