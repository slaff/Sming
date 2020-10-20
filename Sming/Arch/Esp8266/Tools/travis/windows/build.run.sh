#!/bin/bash
set -ex # exit with nonzero exit code if anything fails

echo "Windows build script"

if [ "$SMING_ARCH" = "Host" ]; then
    echo "Build a couple of basic applications"
    $MAKE_PARALLEL Basic_Serial Basic_ProgMem STRICT=1 V=1

    echo "Run basic tests"
    $MAKE_PARALLEL tests
else
    $MAKE_PARALLEL Basic_Ssl
    $MAKE_PARALLEL Basic_SmartConfig
fi