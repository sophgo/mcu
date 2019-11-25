#!/bin/bash

# only support linux

# usage: source setup.sh MCU-ROOT TOOLCHAIN-INSTALL-DIR

usage()
{
    echo 'usage: source setup.sh MCU-ROOT [TOOLCHAIN-INSTALL-DIR]'
}

if [ $# -lt 1 ]; then
    echo 'invalid usage' 1>&2
    usage
    return 1
fi


ROOT=$1
ROOT=`echo "$ROOT" | sed 's/\/$//'`

TOOLCHAIN_SOURCE=$ROOT/tools/toolchain

if [ ! -d $TOOLCHAIN_SOURCE ]; then
    echo 'cannot find toolchain binary executable file, maybe mcu root is wrong'
    echo $ROOT
    return 1
fi

if [ $# -lt 2 ]; then
    TOOLCHAIN_INSTALL=$TOOLCHAIN_SOURCE/install
else
    TOOLCHAIN_INSTALL=$2
    TOOLCHAIN_INSTALL=`echo "$TOOLCHAIN_INSTALL" | sed 's/\/$//'`
fi

if [ ! -d $TOOLCHAIN_INSTALL ]; then
    mkdir -p $TOOLCHAIN_INSTALL
fi

if [ ! -d $TOOLCHAIN_INSTALL ]; then
    echo 'cannot create toolchain install directory'
    echo $TOOLCHAIN_INSTALL
    return 1
fi

echo 'MCU Root Directory: ' $ROOT
echo 'Toolchain Installation Directory: ' $TOOLCHAIN_INSTALL

install_toolchain()
{
    local BINARY_SOURCE=$TOOLCHAIN_SOURCE/$1
    local BINARY_INSTALL=$TOOLCHAIN_INSTALL/$2
    local CROSS_COMPILE=$3

    # echo 'installing ' $BINARY_SOURCE 'to ' $BINARY_INSTALL
    echo "installing $CROSS_COMPILE"
    if [ -d $BINARY_INSTALL ]; then
        echo "$CROSS_COMPILE already installed"
    else
        pushd $TOOLCHAIN_INSTALL 1>/dev/null 2>&1
        # install
        tar -xf $BINARY_SOURCE
        popd 1>/dev/null 2>&1
    fi
    local EXEC_PATH=$BINARY_INSTALL/bin
    if echo $PATH | grep $EXEC_PATH 1>/dev/null 2>&1; then
        echo "$CROSS_COMPILE already in PATH"
    else
        PATH=$EXEC_PATH:$PATH
    fi
    EXEC_CHECK=`which ${CROSS_COMPILE}gcc | xargs dirname`
    if [ "x$EXEC_CHECK" != "x$EXEC_PATH" ]; then
        echo "$CROSS_COMPILE executable file in wrong place"
        return 1
    fi
    return 0
}

# arm none eabi
install_toolchain gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2 \
                  gcc-arm-none-eabi-9-2019-q4-major \
                  arm-none-eabi-

# arm atollic
install_toolchain gcc-atollic-eabi-6-x86_64-linux.tar.bz2 \
                  gcc-atollic-eabi-6-x86_64-linux \
                  arm-atollic-eabi-

# aarch64 linux gnu
install_toolchain gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu.tar.xz \
                  gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu \
                  aarch64-linux-gnu-

echo 'installation done'

