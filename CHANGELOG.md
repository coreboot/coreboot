Change log for PC Engines fork of coreboot
==========================================

Releases 4.0.x are based on PC Engines 20160304 release.
Releases 4.5.x are based on mainline support submitted in
[this gerrit ref](https://review.coreboot.org/#/c/14138/).

## [Unreleased]

## [v4.5.4] - 2017-01-24
### Changed
- optimizations for SeaBIOS, faster boot
- fix `WARNING - Timeout at i8042_flush:71!` message during boot
- fix `ASSERTION ERROR: file 'src/northbridge/amd/pi/agesawrapper.c', line 305`
  messager during boot
- add coreboot build string message during boot
- fix disabling/enabling iPXE rom by `sortbootorder`
- fix custom menu/config options for iPXE rom

## [v4.5.3] - 2017-01-12
### Changed
- bootorder file align to 4K in flash
- add legacy logs on boot
- PCEngines SeaBIOS enabled by default
- turn off D4 and D5 leds on boot
- enable power on after power failure

[Unreleased]: https://github.com/pcengines/coreboot/compare/v4.5.4...coreboot-4.5.x
[v4.5.4]: https://github.com/pcengines/coreboot/compare/v4.5.3...v4.5.4
[v4.5.3]: https://github.com/pcengines/coreboot/compare/v4.5.3...v4.5.2
