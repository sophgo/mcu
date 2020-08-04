#!/bin/bash
match=$1
version_string=`git describe --dirty --match "${match}*" 2>/dev/null`

if [ $? != 0 ]; then
    echo 'no tag, please tag it first' 1>&2
    echo 'no-tag'
    exit 1
fi

version_string=`echo $version_string | sed "s/${match}\-//"`

echo $version_string

