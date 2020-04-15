#!/bin/bash
suffix_list='c cpp h cxx hxx ld'

for suffix in $suffix_list; do
    echo "processing $suffix files"
    list=`find -type f -name "*.$suffix"`
    for file in $list; do
        grep STMicroelectronics $file > /dev/null
        is_st=$?

        grep 'ARM Limited' $file > /dev/null
        is_arm=$?

        grep 'ARM LIMITED' $file > /dev/null
        is_arm_upper=$?

        if [ $is_st -eq 0 ] || [ $is_arm -eq 0 ] || [ $is_arm_upper -eq 0 ]; then
            dos2unix $file 1>/dev/null 2>&1
            sed -i s/'2020 STMicroelectronics'/'2019 STMicroelectronics'/ $file
        fi
    done
done


