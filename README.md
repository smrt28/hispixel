# HisPixel

Terminal application

- minimalistic design
- highly configurable

# Build

## Ubuntu 18.4/20.4

### On Ubuntu 20.4
```
sudo apt install qtchooser
```

### On Ubuntu 18.4
```
sudo apt install qdbus
```

### On Both Ubuntu's
```
$ sudo apt install autogen autoconf libtool pkgconf libvte-2.91-dev libgtk-3-dev libboost-all-dev g++ make
$ autoreconf -if
$ mkdir build
$ cd build
$ ../configure
$ make

```

## Config file

To create the default one:
```
$ mkdir -P ~/.hispixel/
$ hispixel -D > ~/.hispixel/config
```

The config file is read from:

```
~/.hispixel/config
~/.config/hispixel
```

## Using his-commands
source scripts/env.sh
```
$ . scripts/env.sh
$ his-dump [term. name]
```

## Gnome
```
cp scripts/hispixel.desktop  ~/.local/share/applications/hispixel.desktop
```

## Notes
It looks like Ubuntu 20.04 now has more than one package for Terminus.

* `xfonts-terminus` - Installs the "Terminus" font. Only some applications seem to support it. (eg: Shows up in Konsole, but not Terminator.)
* `fonts-terminus` - Installs the "Terminus (TTF)" font. This seems to work everywhere.
