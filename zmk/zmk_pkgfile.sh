#!/bin/sh
# YAL zeldan

SYNTAX="syntax: $0 infile outfile"

INP="$1"
OUT="$2"

if [ -z "$1" ] ; then
	echo "$SYNTAX"
	exit 1
fi

if [ -z "$2" ] ; then
	echo "$SYNTAX"
	exit 1
fi

if [ -r "$1" ] ; then
	zmk_pkg.sh < "$1" > "$2"
else
 	echo "# no pkg-config used" > "$2"
fi

