#!/bin/sh

BASENAME=flatuniverse-$(date +%Y%m%d)
FILENAME=$BASENAME.tar.gz

# dirs
rm -Rf $BASENAME
mkdir $BASENAME
cd ../Release/

# copy
cp -R data flatuniverse.exe ../build/$BASENAME/
cd ../build/

# replace config
mv $BASENAME/data/release.cfg $BASENAME/data/default.cfg

# pack
tar -zcf $FILENAME $BASENAME

# erase
rm -Rf $BASENAME/

echo -e "\nDone"
