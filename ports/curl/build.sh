#!/bin/sh

PKG_NAME="curl"
PKG_VERSION="8.19.0"
SRC_URL="https://curl.se/download/curl-8.19.0.tar.xz"

build() {
    ./configure --prefix=/usr \
                --with-openssl \
                --with-zlib \
                --disable-static \
                --enable-optimize \
                --disable-dict \
                --disable-gopher \
                --disable-imap \
                --disable-pop3 \
                --disable-rtsp \
                --disable-smtp \
                --disable-telnet \
                --disable-tftp \
                --disable-smb \
                --disable-ldap \
                --without-brotli \
                --without-zstd \
                --without-libidn2
    make -j$(nproc)
}

install() {
    make DESTDIR=/data install
}