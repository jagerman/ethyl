#!/bin/bash

set -e

UPSTREAM_VER="$1"
LIB_VER="${UPSTREAM_VER/[^0-9.]*/}"
LIB_VER="${LIB_VER%.[0-9]*}"
if ! grep -q "^Package: libethyl$LIB_VER\$" debian/control; then
    echo -e "\nError: debian/control doesn't contain the correct libethyl$LIB_VER version; you should run:\n\n    ./debian/update-lib-version.sh\n"
    exit 1
fi

if ! [ -f debian/libethyl$LIB_VER ]; then
    rm -f debian/libethyl[0-9]*.install
    sed -e "s/@LIB_VER@/$LIB_VER/" debian/libethyl.install.in >debian/libethyl$LIB_VER.install
fi
