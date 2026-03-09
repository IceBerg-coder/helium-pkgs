#!/bin/sh

PKG_NAME="nano"
PKG_VERSION="7.2"
SRC_URL="https://www.nano-editor.org/dist/v7/nano-7.2.tar.gz"

build() {
    ./configure --prefix=/usr && make
}

install() {
    mkdir -p /data/hpm/bin
    cp src/nano /data/hpm/bin/nano
    chmod +x /data/hpm/bin/nano
}
