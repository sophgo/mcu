#!/bin/bash

echo $#
top="$PWD/.."
dir=''

function version()
{
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
    bm1686evb)
        dir="$top/BM1686EVB"
        key_word='MCU_SW_VER'
        ;;
    sc7pro)
        dir="$top/SC7PRO"
        key_word='MCU_SW_VER'
        ;;
    esac
}

function copy_code()
{
    if test -e "$top/mcu_release"
    then
        echo "$top/mcu_release file exist"
    else
        mkdir $top/mcu_release
    fi

    for i in "$@"; do
        # typeset -u code_name
        # code_name="$i"
        # echo $code_name
        version $i
        cp -r $dir $top/mcu_release
    done

    cp $top/.gitignore $top/mcu_release

}

function copy_tools()
{
    if test -e "$top/mcu_release/tools"
    then
        echo "$top/mcu_release/tools file exist"
    else
        mkdir $top/mcu_release/tools
    fi

    cp -r $top/tools/pack $top/mcu_release/tools
    cp -r $top/tools/util $top/mcu_release/tools
    cp -r $top/tools/toolchain $top/mcu_release/tools
}

function copy_build()
{
    if test -e "$top/mcu_release/build"
    then
        echo "$top/mcu_release/build file exist"
    else
        mkdir $top/mcu_release/build
    fi

    for i in "$@"
    do
        cp $top/build/$i.mak $top/mcu_release/build
        cp $top/build/$i-layout.xml $top/mcu_release/build
    done

    cp $top/build/Makefile $top/mcu_release/build
    cp $top/build/release.sh $top/mcu_release/build
}

function copy_lib()
{
    for i in "$@"
    do
        # typeset -u code_name
        # code_name="$i"
        version $i
        libstm32=$(cat $dir/Makefile | grep "lopencm")
        libgd32=$(cat $dir/Makefile | grep "lgd32e50")
        # echo $libstm32
        # echo $libgd32
        if test -n "$libstm32"
        then
            echo "stm32 family"
            cp -r $top/libopencm3 $top/mcu_release
        fi
        if test -n "$libgd32"
        then
            echo "gd32 family"
            cp -r $top/libgd $top/mcu_release
        fi
    done
}

function copy_upgrade()
{
    for i in "$@"
    do
        upgradefile=$(cat $top/build/$i-layout.xml | grep "/upgrader.bin")
        uartupgradefile=$(cat $top/build/$i-layout.xml | grep "/uart-upgrader.bin")
        echo $upgradefile
        echo $uartupgradefile
        if [  $upgradefile ]
        then
            echo "i2c upgrade"
            cp -r $top/upgrader $top/mcu_release
        fi
        if [  $uartupgradefile ]
        then
            echo "uart upgrade"
            cp -r $top/uart-upgrader $top/mcu_release
        fi
    done
}


copy_code $@
copy_tools
copy_build $@
copy_lib $@
copy_upgrade $@

