#!/bin/sh 

make clean
sync
make

TARGET_DIR="release"

if [ -d "$TARGET_DIR" ]; then
	rm -rf ${TARGET_DIR}
fi

destDir=${TARGET_DIR}/include
mkdir -p $destDir
cp -f src/*.h $destDir

destDir=${TARGET_DIR}/lib
mkdir -p $destDir
cp librum.a $destDir




