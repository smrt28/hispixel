#!/bin/bash

if [ "x$HISPIXEL_APP_ID" = "x" ]; then
    return
fi

CACHE_BASE_DIR=~/.hispixel/cache
CACHE_DIR="$CACHE_BASE_DIR/$HISPIXEL_APP_ID"
LATEST="$CACHE_DIR/latest"

function __his_dir_init() {
    mkdir -p $CACHE_DIR
}

function __his_dump_init() {
    __his_dir_init
    R=$(head -c5 /dev/urandom  | base32)
    T=$(date +"%Y-%m-%d_%H:%M:%S")
    FILENAME="$CACHE_DIR/$HISPIXEL_APP_TID-$T-$R-$1"
}

function __his_latest_link() {
    rm -f "$LATEST"
    ln -s "$FILENAME" "$LATEST"
}

function his-dump() {
    __his_dump_init $1 $2
    qdbus $HISPIXEL_APP_ID /com/hispixel com.hispixel.GDBUS.VteDump "$1 $2" > $FILENAME
    __his_latest_link
    cat $FILENAME | tail -n +2
}

function his-less() {
    __his_dump_init $1
    qdbus $HISPIXEL_APP_ID /com/hispixel com.hispixel.GDBUS.VteDump "$1 $2" > $CACHE_DIR/$FILENAME
    __his_latest_link
    cat $FILENAME | tail -n +2 | less
}

function his-last() {
    cat $LATEST | tail -n +2
    return 0        
}

