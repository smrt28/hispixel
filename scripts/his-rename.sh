#!/bin/bash


if [ "x$2" = "x" ]; then
    ARGS="{} $1"
else
    ARGS="$@"
fi

RV=$(qdbus $HISPIXEL_APP_ID /com/hispixel com.hispixel.GDBUS.SetName "$ARGS")
echo $RV

exit $RV
