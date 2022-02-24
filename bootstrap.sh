#!/bin/sh

aclocal
autoheader
libtoolize
automake -a
autoconf
