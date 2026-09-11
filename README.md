# Qt Platform Theme for CuarzoOS

Allows Qt apps to automatically send their menus to [Firmament](https://github.com/CuarzoSoftware/Firmament) (the global menu bar for ***Cuarzo OS***) using the [Heaven](https://github.com/CuarzoSoftware/Heaven) client library.

![](https://i.imgur.com/StKlGbk.png)

The plugin also binds the `lvr_private_handle` Wayland protocol on the application's own `wl_display` to obtain the compositor-assigned handle, which it forwards to Heaven so the compositor can match the active window to its menus.

## Dependencies

- **Qt 6** — Core, Gui, and the private/QPA headers (`qpa/qplatform*.h`).
- **Meson** + **Ninja**.
- **wayland-scanner** and the **wayland-client** library.
- **Heaven** (`cz-heaven-client`) and **cz-core**, discovered through `pkg-config`.

On Fedora:

```bash
sudo dnf install qt6-qtbase-devel qt6-qtbase-private-devel \
                 wayland-devel meson ninja-build
```

(The `qt6-qtbase-private-devel` package is what provides the QPA headers this plugin is built on.) `cz-heaven-client` and `cz-core` come from your [Cuarzo](https://github.com/CuarzoSoftware/Cuarzo) build/installation.

## Build & Install

```bash
meson setup builddir
ninja -C builddir
sudo ninja -C builddir install
```

The plugin (`libcuarzo.so`) is installed into Qt's `platformthemes` directory
(`qmake6 -query QT_INSTALL_PLUGINS` + `/platformthemes`, e.g.
`/usr/lib64/qt6/plugins/platformthemes/`).

## Usage

Run any Qt application under a Cuarzo/Wayland session with the platform theme selected:

```bash
QT_QPA_PLATFORMTHEME=cuarzo <your-qt-app>
```

To make it the default for every Qt app, export it from your session (e.g. in
`~/.config/environment.d/` or your compositor's startup):

```bash
export QT_QPA_PLATFORMTHEME=cuarzo
```

The app's menu bar will then appear in Firmament instead of inside the window.
