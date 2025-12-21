#!/bin/bash

# Patching VTE
# ------------
# git clone https://gitlab.gnome.org/GNOME/vte.git
# meson setup _build --prefix=$HOME/opt/vte-patched
# diff --git a/src/vtedefines.hh b/src/vtedefines.hh
# +++ b/src/vtedefines.hh
#-#define VTE_CELL_SCALE_MIN (1.)
#+#define VTE_CELL_SCALE_MIN (.4)
#
# ninja -C _build -j 10
#



# to run hispixel witch curtom vte build

#LD_LIBRARY_PATH=/home/smrt/w/vte/_build/src:$LD_LIBRARY_PATH ./build/src/hispixel
LD_LIBRARY_PATH=/opt/smrt/vte/_build/src:$LD_LIBRARY_PATH /opt/smrt/hispixel/build/src/hispixel

