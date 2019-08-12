# HisPixel

Terminal application

- minimalistic design
- highly configurable

# Build

## Ubuntu 18.4

```
$ sudo apt install autogen autoconf libtool pkgconf libvte-2.91-dev
$ autoreconf -if
$ mkdir build
$ cd build
$ ../configure
$ make

```


## Commands
Dump terminal number 1 history:

```
qdbus $HISPIXEL_APP_ID /com/hispixel com.hispixel.GDBUS.Rpc 1
```

Dump first terminal history
```
qdbus $HISPIXEL_APP_ID /com/hispixel com.hispixel.GDBUS.Rpc first
```

Get current tab number
```
qdbus $HISPIXEL_APP_ID /com/hispixel com.hispixel.GDBUS.Rpc n
```
