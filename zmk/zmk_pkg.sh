#!/bin/sh
# YAL zeldan


# TODO sed on GNU, gsed on BSD

echo "#autogenerated by zmk_pkg.sh on $(date -Iseconds)" 
echo ""

while : ; do
	read X
	if [ $? -gt 0 ] ; then
		break
	fi
	XS=$(echo "$X" | sed -r 's/#.*//;s/^ *//;s/ *$//')
	if [ -n "$XS" ] ; then
		echo "# $XS"
		echo "ZMK_CPPFLAGS+=\$(shell pkg-config $XS --cflags-only-I)"
		echo "ZMK_CFLAGS+=\$(shell pkg-config $XS --cflags-only-other)"
		echo "ZMK_LDLIBS+=\$(shell pkg-config $XS --libs-only-L)"
		echo "ZMK_LDLIBS+=\$(shell pkg-config $XS --libs-only-l)"
		echo "ZMK_LDFLAGS+=\$(shell pkg-config $XS --libs-only-other)"
		echo ""
	fi
done

echo "#." 
