# HisPixel

[![Version](https://img.shields.io/badge/version-1.1.0-blue.svg)](https://github.com/smrt28/hispixel)
[![License](https://img.shields.io/badge/license-GPL-green.svg)](LICENSE)

A minimalistic, highly configurable terminal emulator with workspace support.

## Features

- **Minimalistic Design** - Clean, distraction-free interface
- **Highly Configurable** - Customize colors, fonts, keybindings, and behavior
- **Workspace Support** (v2.0.0+) - Multiple independent terminal environments
- **Multi-tab Management** - Organize terminals with tabs
- **D-Bus Integration** - Scriptable interface for automation
- **Color Themes** - Per-workspace color customization
- **GTK3-based** - Native Linux desktop integration

## Screenshots

![Workspace Q](https://smrt28.cz/q-axe.png)
![Workspace R](https://smrt28.cz/r-axe.png)

## Table of Contents

- [Installation](#installation)
  - [Dependencies](#dependencies)
  - [Building from Source](#building-from-source)
  - [Desktop Integration](#desktop-integration)
- [Configuration](#configuration)
  - [Quick Start](#quick-start)
  - [Configuration Options](#configuration-options)
- [Usage](#usage)
  - [Keyboard Shortcuts](#keyboard-shortcuts)
  - [Workspaces](#workspaces)
  - [D-Bus Commands](#d-bus-commands)
- [Advanced](#advanced)
- [Contributing](#contributing)
- [License](#license)

## Installation

### Dependencies

**Required:**
- GTK+ 3.0 or higher
- VTE 2.91 (Virtual Terminal Emulator)
- GIO 2.0
- Boost libraries (≥1.50):
  - boost-iostreams
  - boost-thread
  - boost-program-options
  - boost-system
- gdbus-codegen

**Build tools:**
- autoconf (≥2.69)
- automake
- libtool
- pkg-config
- g++ (C++11 compatible)
- make

### Building from Source

#### Ubuntu/Debian

**Ubuntu 22.04 LTS:**
```bash
sudo apt install autogen autoconf libtool pkg-config \
                 libvte-2.91-dev libgtk-3-dev libboost-all-dev \
                 g++ make
```

**Ubuntu 20.04:**
```bash
sudo apt install qtchooser autogen autoconf libtool pkg-config \
                 libvte-2.91-dev libgtk-3-dev libboost-all-dev \
                 g++ make
```

**Ubuntu 18.04:**
```bash
sudo apt install qdbus autogen autoconf libtool pkg-config \
                 libvte-2.91-dev libgtk-3-dev libboost-all-dev \
                 g++ make
```

#### Build Steps

```bash
# Clone the repository (if not already done)
git clone https://github.com/smrt28/hispixel.git
cd hispixel

# Generate configure script
autoreconf -if

# Create build directory
mkdir -p build
cd build

# Configure and build
../configure
make

# Optional: Install system-wide
sudo make install

# Or run from build directory
./src/hispixel
```

#### Build Options

Enable debug mode:
```bash
../configure --enable-debug
make
```

### Desktop Integration

#### GNOME/GTK Desktop

```bash
# Copy desktop entry
cp scripts/hispixel.desktop ~/.local/share/applications/hispixel.desktop

# Edit the file to set correct paths
# Update 'Exec=' and 'Icon=' fields as needed
```

See [share/README](share/README) for detailed desktop integration steps.

## Configuration

### Quick Start

Generate default configuration:
```bash
hispixel -D > ~/.config/hispixel
```

The configuration file will be automatically loaded from:
- `~/.config/hispixel`

### Configuration Options

#### Terminal Appearance

```ini
# Font settings
term_font = "Terminus"
label_font = "Terminus"
term_font_size = "12"
allow_bold = "true"

# Colors
color_bg = "#000000"
color_fg = "#e5e5e5"
gama = "0"  # Gamma adjustment (-100 to 100)

# Behavior
scrollback_lines = "50000"
audible_bell = "false"
command = "/bin/bash"
```

#### Window Settings

```ini
window_width = "400"
window_height = "300"
show_tabbar = "true"
tabbar_bg_color = "#303030"
tabbar_on_bottom = "false"
```

#### Tabs and Workspaces

```ini
startup_tabs = "1"
auto_open_tabs = "true"

# Workspace colors (6 colors for tab differentiation)
z_color_0 = ""
z_color_1 = "ff44ff aaaaff 44ffff ffff44 ff4444 44ff44"
z_gama = "-50"
z_names = "Q W E R T Y"
z_name_color = "ffffff"
```

For complete configuration reference, see [CONFIGURATION.md](CONFIGURATION.md).

## Usage

### Keyboard Shortcuts

#### Tab Navigation
| Shortcut | Action |
|----------|--------|
| `Alt+1` through `Alt+9` | Switch to tab 1-9 |
| `Alt+Ctrl+Z` | Open new tab |
| `Ctrl+D` | Close current tab |

#### Tab Management
| Shortcut | Action |
|----------|--------|
| `Ctrl+Alt+1` through `Ctrl+Alt+9` | Swap current tab with position 1-9 |
| `Ctrl+Alt+T` | Toggle tabbar visibility |

#### Workspace Switching
| Shortcut | Action |
|----------|--------|
| `Alt+Q` | Switch to workspace 1 (Q) |
| `Alt+W` | Switch to workspace 2 (W) |
| `Alt+E` | Switch to workspace 3 (E) |
| `Alt+R` | Switch to workspace 4 (R) |
| `Alt+T` | Switch to workspace 5 (T) |
| `Alt+Y` | Switch to workspace 6 (Y) |

All shortcuts are fully customizable in the config file.

### Workspaces

Workspaces provide independent terminal environments, similar to virtual desktops:

- Each workspace has its own set of tabs
- Each workspace can have unique color schemes
- Quickly switch between workspaces with keyboard shortcuts
- Ideal for organizing different projects or contexts

Example workspace configuration:
```ini
# Define workspace names
z_names = "DEV TEST PROD DOC ADMIN MISC"

# Custom colors for each workspace's tabs
z_color_1 = "ff44ff aaaaff 44ffff ffff44 ff4444 44ff44"
z_gama = "-50"

# Bind keys to workspaces
bindsym alt+q focus_z 1  # DEV
bindsym alt+w focus_z 2  # TEST
bindsym alt+e focus_z 3  # PROD
bindsym alt+r focus_z 4  # DOC
bindsym alt+t focus_z 5  # ADMIN
bindsym alt+y focus_z 6  # MISC
```

### D-Bus Commands

HisPixel provides a D-Bus interface for programmatic control and automation.

#### Setup

Install D-Bus tools (if not already installed):
```bash
# Ubuntu/Debian
sudo apt install gdbus
```

Source the helper script:
```bash
source scripts/env.sh
```

#### Dump Terminal Content

Dump the content of a specific terminal:
```bash
his-dump <workspace> <tab-number>
```

Examples:
```bash
# Dump 3rd terminal on workspace Q
his-dump q 3

# Dump 1st terminal on workspace W
his-dump w 1
```

For complete D-Bus API documentation, see [DBUS_API.md](DBUS_API.md).

## Advanced

### Font Recommendations

**Ubuntu 22.04 LTS:**
```ini
term_font = "Ubuntu Mono"
gama = "40"
auto_open_tabs = "true"
```

**Terminus Font:**
- `sudo apt install xfonts-terminus` - X11 bitmap font (may not work everywhere)
- `sudo apt install fonts-terminus` - TrueType version (recommended)

**List Available Fonts:**
```bash
python3 scripts/list-fonts.py
```

### Command-line Options

```bash
hispixel [OPTIONS]

Options:
  -D, --dump-config     Dump default configuration to stdout
  -d, --daemon          Run as daemon (background process)
  -h, --help            Show help message
  --version             Show version information
```

### Troubleshooting

**Issue: Tabs not auto-opening**
```ini
# Add to config
auto_open_tabs = "true"
startup_tabs = "1"
```

**Issue: Font not rendering correctly**
- Ensure font is installed system-wide
- Try TrueType versions (fonts-terminus instead of xfonts-terminus)
- Use `list-fonts.py` to verify available fonts

**Issue: D-Bus commands not working**
```bash
# Ensure D-Bus tools are installed
sudo apt install gdbus

# Source environment
source scripts/env.sh
```

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for:
- Development setup
- Code structure and architecture
- Coding standards
- Submitting pull requests

## Project Structure

```
hispixel/
├── src/              # Source code
│   ├── main.cc       # Application entry point
│   ├── hispixelapp.* # Main application class
│   ├── config.*      # Configuration parser
│   ├── tabs.*        # Tab/workspace management
│   ├── dbus*.*       # D-Bus interface
│   └── gtest/        # Embedded Google Test framework
├── scripts/          # Helper scripts and desktop files
├── share/            # Desktop integration resources
├── configure.ac      # Autoconf configuration
└── README.md         # This file
```

## Known Issues

- Counter overflow in terminal ID generation (tabs.h:18) - unlikely in practice
- Memory leak in D-Bus signal handlers (dbushelper.h:47) - bounded, registration happens once

See the issue tracker for more details and to report bugs.

## License

This project is licensed under the GPL License. See the LICENSE file for details.

## Author

Ondřej Holeček <ondrej.holecek@gmail.com>

## Links

- [Project Website](https://smrt28.cz)
- [Screenshots](https://smrt28.cz/q-axe.png)
- [Issue Tracker](https://github.com/smrt28/hispixel/issues)

---

**HisPixel** - Terminal emulator for power users who value simplicity and customization.
