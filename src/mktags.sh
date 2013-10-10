#!/bin/bash

# creates ctags

ctags -R .
find -maxdepth 1 -type d | while read A
do
	pushd "$A"
	ctags -R ..
	popd
done

