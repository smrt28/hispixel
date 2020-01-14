# HisPixel

Terminal application

- minimalistic design
- highly configurable

# Build

## Ubuntu 18.4

```
$ sudo apt install autogen autoconf libtool pkgconf libvte-2.91-dev libgtk-3-dev libboost-all-dev
$ autoreconf -if
$ mkdir build
$ cd build
$ ../configure
$ make

```

## Config file

The config file is read from:

```
~/.hispixel/config
~/.config/hispixel
```

in this order

## Remote control commands
Dump terminal number 1 history:

```
qdbus $HISPIXEL_APP_ID /com/hispixel com.hispixel.GDBUS.VteDump 1
```

Dump first terminal history
```
qdbus $HISPIXEL_APP_ID /com/hispixel com.hispixel.GDBUS.VteDump first
```

Run ls command in terminal number 2
```
qdbus $HISPIXEL_APP_ID /com/hispixel com.hispixel.GDBUS.Feed "2 ls"
```
