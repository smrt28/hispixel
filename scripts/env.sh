#!/bin/bash

function __his_dump_init() {
    CACHE_DIR=~/.hispixel/cache/$HISPIXEL_APP_ID
    mkdir -p $CACHE_DIR
    R=$(head -c5 /dev/urandom  | base32)
    T=$(date +"%Y-%m-%d_%H:%M:%S")
    FILENAME="$HISPIXEL_APP_TID-$T-$R-$1"
}

function his-dump() {
    __his_dump_init $1
    qdbus $HISPIXEL_APP_ID /com/hispixel com.hispixel.GDBUS.VteDump "$1" > $CACHE_DIR/$FILENAME
    cat $CACHE_DIR/$FILENAME | tail -n +2
}

function his-less() {
    __his_dump_init $1
    qdbus $HISPIXEL_APP_ID /com/hispixel com.hispixel.GDBUS.VteDump "$1" > $CACHE_DIR/$FILENAME
    cat $CACHE_DIR/$FILENAME | tail -n +2 | less
}

function his-rename() {
    if [ "x$2" = "x" ]; then
        ARGS="{} $1"
    else
        ARGS="$@"
    fi

    RV=$(qdbus $HISPIXEL_APP_ID /com/hispixel com.hispixel.GDBUS.SetName "$ARGS")
    echo $RV
    return $RV
}

function his-last() {
    return 0        
}

