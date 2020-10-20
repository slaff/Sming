#!/bin/bash
set -ex # exit with nonzero exit code if anything fails

INSTALLER="$TRAVIS_OS_NAME/install.sh"

if [ -f "$TRAVIS_BUILD_DIR/Sming/Arch/$SMING_ARCH/Tools/travis/$INSTALLER" ]; then
    source "$TRAVIS_BUILD_DIR/Sming/Arch/$SMING_ARCH/Tools/travis/$INSTALLER"
fi