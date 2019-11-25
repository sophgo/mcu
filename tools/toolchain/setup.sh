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

ARM_NONE_EABI_SOURCE=$TOOLCHAIN_SOURCE/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2
ATOLLIC_EABI_SOURCE=$TOOLCHAIN_SOURCE/gcc-atollic-eabi-6-x86_64-linux.tar.bz2

ARM_NONE_EABI_INSTALL=$TOOLCHAIN_INSTALL/gcc-arm-none-eabi-9-2019-q4-major
ATOLLIC_EABI_INSTALL=$TOOLCHAIN_INSTALL/gcc-atollic-eabi-6-x86_64-linux

if [ -d $ARM_NONE_EABI_INSTALL ]; then
    echo 'arm-none-eabi toolchain already installed before'
else
    pushd $TOOLCHAIN_INSTALL
    # install arm-none-eabi-
    tar -xjf $ARM_NONE_EABI_SOURCE
    popd
fi

if [ -d $ATOLLIC_EABI_INSTALL ]; then
    echo 'atollic-eabi toolchain already installed before'
else
    pushd $TOOLCHAIN_INSTALL
    # install atollic-eabi-
    tar -xjf $ATOLLIC_EABI_SOURCE
    popd
fi

# add to PATH
ARM_NONE_EABI_EXEC=$ARM_NONE_EABI_INSTALL/bin
ATOLLIC_EABI_EXEC=$ATOLLIC_EABI_INSTALL/bin
if echo $PATH | grep $ARM_NONE_EABI_EXEC 1>/dev/null 2>&1; then
    echo 'arm-none-eabi aleady in PATH'
else
    PATH=$ARM_NONE_EABI_EXEC:$PATH
fi

if echo $PATH | grep $ATOLLIC_EABI_EXEC 1>/dev/null 2>&1; then
    echo 'atollic-eabi aleady in PATH'
else
    PATH=$ATOLLIC_EABI_EXEC:$PATH
fi

EXEC_CHECK=`which arm-none-eabi-gcc`
if [ x"EXEC_CHECK" == x"ARM_NONE_EABI_EXEC" ]; then
    echo 'arm-none-eabi executable file in wrong place'
    return 1
fi

EXEC_CHECK=`which arm-atollic-eabi-gcc`
if [ x"EXEC_CHECK" == x"ATOLLIC_EABI_EXEC" ]; then
    echo 'arm-none-eabi executable file in wrong place'
    return 1
fi

echo 'installation done'

