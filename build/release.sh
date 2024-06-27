#!/bin/bash
list='sm5g se5lite sm5gd32 bm1684evb sa5 sc5h sc5plus sm5mini sc5pro bm1684xevb sc7pro sc7hp75 sc7fp150 sg2042evb sg2042revb wolfclaw sg2042x4 sm7mini sm7mqy athena2evb athena2acp sm7g sm7m_mp_1_1 sm7m_mp_1_2' 
top="$PWD/.."

unset rm

echo $#

if [ $# -gt 0 ]; then
bsp_top=$1
TOOLCHAIN_BASE=$bsp_top/host-tools/gcc
source $top/tools/toolchain/setup.sh $top $TOOLCHAIN_BASE
fi

function _version()
{
    if [ "x$dir"  == "x" ]; then
        fw_ver='unversioned'
        return
    fi
    pushd $dir > /dev/null
    digit=`grep "define[ \t][ \t]*$key_word" ./ -nr --include=*.c --include=*.h | awk '{print $3}' | sed 's/[()\r]//g'`
    fw_ver=`printf %d $digit`
    popd > /dev/null
}

function version()
{
    dir=''
    key_word=''
    fw_ver='unversioned'
    case $1 in
        bm1684evb)
        dir="$top/BM1684EVB"
        key_word='MCU_VERSION'
        ;;
    sa5)
        dir="$top/SA5AIUC10"
        key_word='MCU_VERSION'
        ;;
    sc5h)
        dir="$top/SC5H"
        key_word='MCU_SW_VER'
        ;;
    sc5plus)
        dir="$top/SC5PLUS"
        key_word='MCU_VERSION'
        ;;
    sm5pcie)
        dir="$top/SM5PCIE"
        key_word='MCU_SW_VER'
        ;;
    se5v2pic16)
        dir="$top/LF1704_BM1684_SE5V2.X"
        key_word='SW_VERSION'
        ;;
    se5pic16)
        dir="$top/LF1704_BM1684_SE5.X"
        key_word='SW_VERSION'
        ;;
    se6v2pic16)
        dir="$top/LF1704_BM1684_SE6V2.X"
        key_word='SW_VERSION'
        ;;
    sm5mini)
        dir="$top/SM5MINI"
        key_word='MCU_SW_VER'
        ;;
    sc5pro)
        dir="$top/SC5PRO"
        key_word='MCU_SW_VER'
        ;;
    sm5gd32)
        dir="$top/SM5GD32"
        key_word='MCU_SW_VER'
        ;;
    se5lite)
        dir="$top/SE5LITE"
        key_word='MCU_SW_VER'
        ;;
    sm5g)
        dir="$top/SM5G"
        key_word='MCU_SW_VER'
        ;;
    sm7g)
        dir="$top/SM7G"
        key_word='MCU_SW_VER'
        ;;
    bm1684xevb)
        dir="$top/BM1684xEVB"
        key_word='MCU_SW_VER'
        ;;
    sc7pro)
        dir="$top/SC7PRO"
        key_word='MCU_SW_VER'
        ;;
    sc7hp75)
        dir="$top/SC7HP75"
        key_word='MCU_SW_VER'
        ;;
    sc7fp150)
	dir="$top/SC7FP150"
	key_word='MCU_SW_VER'
	;;
    sg2042evb)
        dir="$top/SG2042EVB"
        key_word='MCU_SW_VER'
        ;;
    sg2042revb)
        dir="$top/SG2042REVB"
        key_word='MCU_SW_VER'
        ;;
    sg2042x4)
        dir="$top/SG2042X4"
        key_word='MCU_SW_VER'
        ;;
    wolfclaw)
        dir="$top/WOLFCLAW"
        key_word='MCU_SW_VER'
        ;;
    sm7mini)
        dir="$top/SM7MINI"
        key_word='MCU_SW_VER'
        ;;
    sm7mqy)
        dir="$top/SM7MQY"
        key_word='MCU_SW_VER'
        ;;
    se7)
        dir="$top/SE7"
        key_word='MCU_SW_VER'
        ;;
    athena2evb)
        dir="$top/ATHENA2EVB"
        key_word='MCU_SW_VER'
        ;;
    athena2acp)
        dir="$top/ATHENA2ACP"
        key_word='MCU_SW_VER'
        ;;
    sm7m_mp_1_1)
        dir="$top/SM7M_MP_1_1"
        key_word='MCU_SW_VER'
        ;;
    sm7m_mp_1_2)
        dir="$top/SM7M_MP_1_2"
        key_word='MCU_SW_VER'
        ;;
    sm7_hk)
        dir="$top/SM7_HK"
        key_word='MCU_SW_VER'
        ;;
    esac
    _version
    # echo "$1 $fw_ver"
}

function clean()
{
    rm -rf release
    for dir in $list; do
        rm -rf $dir
    done
}

function build()
{
    mkdir -p release

    # build libopencm3 first
    pushd ../libopencm3
        make clean && make
    popd

    # build libgd first
    pushd ../libgd
        make clean && make
    popd

    date=`date '+%Y-%m-%d-%H-%M-%S'`
    set -e
    for proj in $list; do
        make PROJ=$proj
    done

    for proj in $list; do
        if echo $proj | grep -q pic16; then
            suffix=hex
        else
            suffix=bin
        fi
        version $proj
        file_name=$proj-mcu-v$fw_ver-$date.$suffix
        cp $proj/$proj-mcu.$suffix $proj/$file_name
        md5sum $proj/$file_name
        cp -r $proj/$file_name release
    done
}

function build_util()
{
    mkdir -p release
    pushd $top/tools/util
        # build host util(x86-64)
        make clean && make
        cp mcu-util $top/build/release/mcu-util-amd64
        # build aarch64 util
        make clean && make CROSS_COMPILE=aarch64-linux-gnu-
        cp mcu-util $top/build/release/mcu-util-aarch64
        # build riscv64 util
        make clean && make CROSS_COMPILE=riscv64-unknown-linux-gnu-
        cp mcu-util $top/build/release/mcu-util-riscv64
    popd
}

clean
build_util
build
# append release notes
cp $top/ReleaseNotes.md $top/build/release
