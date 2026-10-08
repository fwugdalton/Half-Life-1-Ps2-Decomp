#!/bin/sh
# Baixa o ee-gcc 2.95.2 (SCE, build 274) e o wibo (carregador de .exe Win32) do decomp.me.
set -e
DIR=$(dirname "$0")/compilers
mkdir -p "$DIR"
cd "$DIR"
if [ ! -x wibo ]; then
    curl -sSL -o wibo https://github.com/decompals/wibo/releases/download/0.6.16/wibo
    chmod +x wibo
fi
if [ ! -d ee-gcc2.95.2-274 ]; then
    curl -sSL https://github.com/decompme/compilers/releases/download/compilers/ps2_compilers.tar.xz | tar xJf - ee-gcc2.95.2-274
fi
echo "compilador em $DIR/ee-gcc2.95.2-274"
