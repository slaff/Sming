#!/bin/bash
set -ex # exit with nonzero exit code if anything fails

echo "Installing Windows Toolchains"

choco uninstall -y mingw
choco install python --version=3.8.6

export PATH=/C/Python38/:$PATH

rm -rf /C/MinGW
curl -LO $SMINGTOOLS/MinGW.7z
7z -o"/C/" x MinGW.7z

if [ "$SMING_ARCH" = Esp8266 ]; then

    if [ "$BUILD_COMPILER" == "udk" ]; then
        echo "Old toolchain"
        export TOOLCHAIN=esp-udk-win32.7z
        curl -LO "$SMINGTOOLS/$TOOLCHAIN"
        7z -o"$UDK_ROOT" x $TOOLCHAIN
    fi

    if [ "$BUILD_COMPILER" == "eqt" ]; then
        echo "New toolchain"
        mkdir -p "$EQT_ROOT"
        export TOOLCHAIN=i686-w64-mingw32.xtensa-lx106-elf-a5c9861.1575819473.zip
        curl -LO "https://github.com/earlephilhower/esp-quick-toolchain/releases/download/3.0.0-gnu5/$TOOLCHAIN"
        7z -o"$EQT_ROOT" x $TOOLCHAIN
    fi

fi

echo "Done."
