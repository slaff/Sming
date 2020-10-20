#!/bin/bash
set -ex # exit with nonzero exit code if anything fails

if [ "$BUILD_COMPILER" = "udk" ]; then
    export ESP_HOME=$UDK_ROOT
fi
if [ "$BUILD_COMPILER" = "eqt" ]; then
    export ESP_HOME=$EQT_ROOT
fi

export PATH=/C/MinGW/msys/1.0/bin:/C/MinGW/bin:C/Python38:$PATH
export MAKE='mingw32-make.exe'
export MAKE_PARALLEL=$MAKE
export PYTHON='/C/Python38/python'

alias make="$MAKE"