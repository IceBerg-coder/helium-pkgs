#!/bin/sh

PKG_NAME="expat"
PKG_VERSION="2.6.0"
SRC_URL="https://github.com/libexpat/libexpat/releases/download/R_2_6_0/expat-2.6.0.tar.xz"

build() {
    ./configure --prefix=/usr \
                --disable-static \
                --without-docbook \
                --without-examples \
                --without-tests
    make -j$(nproc)
}

install() {
    make DESTDIR=/data install
}