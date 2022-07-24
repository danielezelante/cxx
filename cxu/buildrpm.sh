#!/bin/bash

rpmdev-setuptree

VER=$(../scmrev.sh)
FIXVER=$(echo ${VER} | sed -r 's/-.*//')
REPVER=$(echo ${VER} | sed -r 's/[^-]*-//;s/-.*//')
HASHVER=$(echo ${VER} | sed -r 's/.*-//')


readonly a=`mktemp`
make DISTFILENAMEFILE="$a" dist
cp "../$(<$a)" "$HOME/rpmbuild/SOURCES"
rm "$a"

readonly a=`mktemp`
make -C '../zmk' DISTFILENAMEFILE="$a" dist
cp "../$(<$a)" "$HOME/rpmbuild/SOURCES"
rm "$a"

rpmbuild -bb -D "fixver $FIXVER" -D "repver $REPVER" cxu.spec

