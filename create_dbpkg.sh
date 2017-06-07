#!/bin/bash

# install deps: enable if needed
# sudo apt-get install devscripts
# sudo apt-get install debhelper
# sudo apt-get install build-essential

# defs
VER="0.01"
PACKAGE="saibm"
OUTPUT="$PACKAGE""_$VER"
REPO_PATH="SAI-P4-BM"
OUTPUT_TAR="$OUTPUT.orig.tar.gz"
OUTPUT_PATH=$PACKAGE
DEBVER=1
GIT_PATH=$PWD 
# create tarbell
mkdir ../TEMP
mkdir ../TEMP/$OUTPUT_PATH

#cd $REPO_PATH
#git archive debian --format=tar.gz --output ../TEMP/$OUTPUT_PATH/$OUTPUT_TAR
dpkg-buildpackage -us -uc

#mv $OUTPUT_TAR ./../$OUTPUT_PATH/
cd ../TEMP/

# unpack
cd $OUTPUT_PATH
tar xf $OUTPUT_TAR
mv $OUTPUT_TAR ./../

# add files to debian dir
mkdir debian
dch --create -v $VER-$DEBVER --package $PACKAGE
cd debian
#pwd
echo 10 > compat
#cp ../../../control .
#cp ../../../rules .
touch copyright
mkdir source
cd source
echo "3.0 (quilt)" > format
cd ../

# build package
debuild -us -uc

#install packages
#sudo dpkg -i ../${PACKAGE}_1.0-1_amd64.deb

