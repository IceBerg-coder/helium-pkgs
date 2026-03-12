#!/bin/sh

PKG_NAME="git"
PKG_VERSION="2.44.0"
SRC_URL="https://mirrors.edge.kernel.org/pub/software/scm/git/git-2.44.0.tar.xz"

build() {
    # Git usually requires make configure to generate configure script
    make configure
    ./configure --prefix=/usr \
                --without-tcltk \
                --without-iconv \
                --with-curl \
                --with-openssl \
                --with-zlib
    make -j$(nproc) all
}

install() {
    make DESTDIR=/data install
}