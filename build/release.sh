list='bm1684evb sa5 sc5h sc5plus sm5pcie se5v2pic16'

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
        if echo $proj | grep -q pic16; then
            suffix=hex
        else
            suffix=bin
        fi
        cp $proj/$proj-mcu.$suffix $proj/$proj-mcu-$date.$suffix
        md5sum $proj/$proj-mcu-$date.$suffix
    done
}

build
