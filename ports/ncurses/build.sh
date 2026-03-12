#!/bin/sh

PKG_NAME="ncurses"
PKG_VERSION="6.6"
SRC_URL="https://ftp.gnu.org/gnu/ncurses/ncurses-6.6.tar.gz"

build() {
    # --with-shared: Build .so libraries
    # --without-debug: Smaller binaries
    # --without-ada: We don't have an Ada compiler
    # --without-cxx-binding: Skip failing ancient C++ headers
    # --enable-widec: Support for wide characters (UTF-8)
    ./configure --prefix=/usr \
                --with-shared \
                --without-debug \
                --without-ada \
                --without-cxx-binding \
                --enable-widec \
                --disable-stripping
    
    make -j$(nproc)
}

install() {
    # Install to /data/usr so it persists
    make install DESTDIR=/data
}
