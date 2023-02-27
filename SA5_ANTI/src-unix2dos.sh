#!/bin/bash
suffix_list='c cpp h cxx hxx ld'

for suffix in $suffix_list; do
    echo "processing $suffix files"
    list=`find -type f -name "*.$suffix"`
    for file in $list; do
        dos2unix $file 1>/dev/null 2>&1
        sed -i s/'2020 STMicroelectronics'/'2019 STMicroelectronics/' $file
    done
done


