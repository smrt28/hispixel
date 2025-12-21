# HisPixel Project Context

## Overview
**HisPixel** is a minimalistic, highly configurable terminal emulator for Linux, built with C++17, GTK+ 3, and VTE 2.91. Its distinguishing feature is **Workspaces** (referred to as the "Z-axis" in the code), allowing multiple independent sets of tabs with distinct configurations (colors, names). It also features D-Bus integration for scripting and automation.

## Build & Run

### Dependencies
Ensure the following development packages are installed (names based on Debian/Ubuntu):
*   `libvte-2.91-dev`
*   `libgtk-3-dev`
*   `libboost-all-dev`
*   `autoconf`, `automake`, `libtool`, `pkg-config`
*   `gdbus-codegen` (often part of `libglib2.0-dev` or separate)

### Build Steps
The project uses the Autotools build system.

1.  **Generate configuration scripts:**
    ```bash
    autoreconf -if
    ```

2.  **Configure and Build:**
    ```bash
    mkdir -p build && cd build
    ../configure
    make
    ```

3.  **Run:**
    ```bash
    ./src/hispixel
    ```

### Testing
Unit tests are implemented using Google Test and located in `src/test.cc`. They primarily cover utility classes (`AnyTypeMap`, `ValueCast`, `Parslet`).

1.  **Build Tests:**
    The test binary `testpixel` is built alongside the main application by the default `make` command.

2.  **Run Tests:**
    ```bash
    ./src/testpixel
    ```

## Project Structure

### Source (`src/`)
*   **Application Entry:**
    *   `main.cc`: Entry point.
    *   `hispixelapp.cc/h`: Main application class `HisPixelApp`. Handles GTK signals, window management, and the main event loop.
*   **Core Logic:**
    *   `tabs.cc/h`: Manages `Tab` and `Tabs` objects. Implements the Z-axis (workspace) logic.
    *   `config.cc/hxx`: Configuration file parser and storage.
    *   `colormanager.cc/h`: Manages color schemes per workspace.
*   **Integrations:**
    *   `dbushandler.cc/h`: D-Bus interface implementation.
    *   `com.hispixel.GDBUS.xml`: D-Bus interface definition.
*   **Utilities:**
    *   `utils.cc/h`, `parslet.h`, `valuecast.h`, `anytypemap.h`: General purpose helpers.

### Scripts (`scripts/`)
*   `list-fonts.py`: Python script (using PyGObject) to list available system fonts.
*   `env.sh`: Helper to set up environment variables for D-Bus interaction.

### Packaging (`debian/`)
*   Contains rules and metadata for building Debian packages (`.deb`).

## Key Concepts

*   **Z-Axis (Workspaces):** The application is organized into workspaces (Z-axis). The `Tabs` class filters and manages tabs based on their `z_axe` property.
*   **Configuration:** Config is loaded from `~/.config/hispixel`. It supports typed values and workspace-specific settings (e.g., `z_color_1`, `z_names`).
*   **D-Bus:** The application exposes methods via D-Bus (e.g., `Dump`, `Feed`, `FocusZ`) defined in `src/com.hispixel.GDBUS.xml`, enabling external control.
