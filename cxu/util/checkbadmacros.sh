#!/bin/sh
# YAL zeldan

cd ..
FILES=$(ls *.hxx | sed -r 's/defs.hxx//g')
REGEXP='(THROW|ERRCALL|TRACE|ASSERT|VERIFY|TODO|NDEBUG)'
grep -E $REGEXP $FILES
cd $OLDPWD

#.
