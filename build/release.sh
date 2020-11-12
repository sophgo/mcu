list='sm5mini'
top=..

unset rm

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
    sm5mini)
        dir="$top/SM5MINI"
        key_word='MCU_SW_VER'
        ;;
    esac
    _version
    # echo "$1 $fw_ver"
}

function clean()
{
    for dir in $list; do
        rm -rf $dir
    done
}

function build()
{
    clean

    # build libopencm3 first
    pushd ../libopencm3
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
    done
}

build

