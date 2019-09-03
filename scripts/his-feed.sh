#!/bin/bash
#TAB="$1"
#shift
COMMAND="$@"
qdbus $HISPIXEL_APP_ID /com/hispixel com.hispixel.GDBUS.Feed "$COMMAND"
