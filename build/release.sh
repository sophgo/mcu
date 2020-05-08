list='bm1684evb sa5 sc5h sc5plus sm5pcie'

unset rm

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
        cp $proj/$proj-mcu.bin $proj/$proj-mcu-$date.bin
        md5sum $proj/$proj-mcu-$date.bin
    done
}

build
