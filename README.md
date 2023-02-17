# HisPixel

Terminal application

- minimalistic design
- highly configurable
- **workspaces**

# Version 2.0.0

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

You can also configure the tab colors and names.
```
z_color_0 = ""
z_color_1 = "ff44ff aaaaff 44ffff ffff44 ff4444 44ff44"
z_gama = "-50"
z_names = "Q W E R T Y"
```

# Build

### On Ubuntu 20.4
```
sudo apt install qtchooser
```

### On Ubuntu 18.4
```
sudo apt install qdbus
```

### On Both Ubuntus
```
$ sudo apt install autogen autoconf libtool pkgconf libvte-2.91-dev libgtk-3-dev libboost-all-dev g++ make
$ autoreconf -if
$ mkdir build
$ cd build
$ ../configure
$ make

```

## Config file

To create the default config:
```
$ mkdir -P ~/.hispixel/
$ hispixel -D > ~/.hispixel/config
```

Place the confing file to:

```
~/.config/hispixel
```

## Using his-commands
source scripts/env.sh
```
$ sudo apt install gdbus
$ . scripts/env.sh
$ his-dump [workspace] [tab-number]
```

For instance, dumps content of the 3rd terminal on the workspace named Q:
```
$ his-dump q 3
```

## Gnome icon
```
cp scripts/hispixel.desktop  ~/.local/share/applications/hispixel.desktop
```

## Notes

* `sudo apt install xfonts-terminus` - Installs the "Terminus" font. Only some applications seem to support it. (eg: Shows up in Konsole, but not Terminator.)
* `sudo apt install fonts-terminus` - Installs the "Terminus (TTF)" font. This seems to work everywhere.

# Ubuntu 22.04.01 LTS

Recommended options:

* `term_font = "Ubuntu Mono"`
* `gama = "40"`
* `auto_open_tabs = "true"`

