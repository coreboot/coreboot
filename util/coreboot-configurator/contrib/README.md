# Distribution Packages ![alt text](images/StarLabs_Logo.png "Star Labs Systems")
The relevant packaging necessary to generate DEB, flatpak and PKG distribution packages is contained here. The generated packages can be used on a distribution such as Fedora, Debian, Ubuntu or Manjaro.

# DEB packages
To build the DEBs, run these commands (from the root of your git checkout):
```bash
cp -r contrib/debian .
debuild --no-lintian
```
To build source files, modify the [change log](debian/changelog) accordingly and run:
```bash
cp -r contrib/debian .
debuild -S
```

# PKG
A sample [PKGBUILD](PKGBUILD) is included.

# Flatpak
To build the Flatpak, run these commands (from the root of your git checkout):
```bash
meson build --prefix=/usr
pushd build
meson dist
popd
cp contrib/flatpak/org.coreboot.coreboot-configurator.json .
flatpak-builder build-dir org.coreboot.coreboot-configurator.json
```

## Copying or Reusing
Included scripts are free software licensed under the terms of the [GNU General Public License, version 3](https://www.gnu.org/licenses/gpl-3.0.txt).

# [© Star Labs® / All Rights Reserved.](https://starlabs.systems)
Any issues or questions, please contact us at [support@starlabs.systems](mailto:supportstarlabs.systems)

View our full range of Linux laptops at: [https://starlabs.systems](https://starlabs.systems)
