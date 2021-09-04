#!/bin/bash

if [[ ! -f "libmbsega.a" ]]; then
	echo "Run this script from the directory where you built libmbsega."
	exit 1
fi

mkdir -p dist

if [ -d "mbsega-SDL2" ]; then
	for lib in $(ldd mbsega-SDL2/mbsega-SDL2.exe | grep mingw | sed "s/.*=> //" | sed "s/(.*)//"); do
		cp "${lib}" dist
	done
	cp mbsega-SDL2/mbsega-SDL2.exe dist
fi