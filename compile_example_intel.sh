#!/bin/sh

./fix_timestamp.sh 
./configure CC=icc CXX=icpc FC=ifort F77=ifort --enable-gtk2 LDFLAGS="-lz -lgccpp" --with-gc-prefix=/usr
make
