#!/bin/sh

PKG_NAME="zlib"
PKG_VERSION="1.3.2"
SRC_URL="https://zlib.net/zlib-1.3.2.tar.xz"

build() {
    ./configure --prefix=/usr
    make -j$(nproc)
}

install() {
    # Install to /data/usr so it's persistent and found by he-sh's GCC paths
    make install DESTDIR=/data
}
