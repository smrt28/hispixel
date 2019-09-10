#!/bin/bash

CACHE_DIR=~/.hispixel/cache/$HISPIXEL_APP_ID

mkdir -p $CACHE_DIR

R=$(head -c5 /dev/urandom  | base32)
T=$(date +"%Y-%m-%d_%H:%M:%S")

FILENAME="$HISPIXEL_APP_TID-$T-$R"

qdbus $HISPIXEL_APP_ID /com/hispixel com.hispixel.GDBUS.VteDump "$1" > $CACHE_DIR/$FILENAME

rm -f $CACHE_DIR/latest
ln -s "$CACHE_DIR/$FILENAME" "$CACHE_DIR/latest"
exit
cat $CACHE_DIR/$FILENAME | tail -n +2 | less
