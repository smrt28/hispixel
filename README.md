# HisPixel

Terminal application

- minimalistic design
- highly configurable

# Version 2.0.0

By default alt-q, alt-w, alt-e, alt-r shitch between the terminal workspaces. 
Until today, I have always kept maximized terminals open in several workspaces.
Since 2.0.0 hispixel has got the workspaces.

![This is an image](https://smrt28.cz/q-axe.png)
![This is an image](https://smrt28.cz/r-axe.png)

By default alt-q, alt-w, alt-e, alt-r switch between the workspaces.

You can configure the shortcuts:

```
bindsym alt+q focus_z 1
bindsym alt+w focus_z 2
bindsym alt+e focus_z 3
bindsym alt+r focus_z 4
bindsym alt+t focus_z 5
bindsym alt+y focus_z 6
```

And you can also configure the tab colors.
```
z_color_0 = ""
z_color_1 = "ff44ff aaaaff 44ffff ffff44 ff4444 44ff44"
z_gama = "-50"
z_names = "Q W E R T Y"
```

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

# Ubuntu 22.04.01 LTS

Recommended options:

* `term_font = "Ubuntu Mono"`
* `gama = "40"`
* `auto_open_tabs = "true"`

