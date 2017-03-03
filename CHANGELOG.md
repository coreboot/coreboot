Change log for PC Engines fork of coreboot
==========================================

Releases 4.0.x are based on PC Engines 20160304 release.
Releases 4.5.x are based on mainline support submitted in
[this gerrit ref](https://review.coreboot.org/#/c/14138/).

## Quick build instructions
1. After cloning run `make crossgcc-i386` to build the toolchain.
2. `make menuconfig` and set:
  * `Mainboard/Mainboard vendor` → `PCEngines`
  * `Mainboard/Mainboard model` → `APU2` or `APU3`
3. `make`

## [Unreleased]

## [v4.5.5.2] - 2017-03-03
### Changed
- (APU3 only) GPIO33 (SIMSWAP) set to output/low by default

## [v4.5.5.1] - 2017-03-02
### Changed
- (APU3 only) GPIO33 (SIMSWAP) set to output/high by default

## [v4.5.5] - 2017-02-24
### Added
- sgabios video oprom is added by default (v.1.0.0 pcengines version)

### Changed
- SeaBIOS up to 1.10.0.1
- sortbootorder up to 4.5.4
- coreboot rebased to the latest mainline

### Fixed
- Asmedia ASM106x controllers work in the mPCIe1 slot
- Memory size shown during boot corrected in 2GB SKU devices

### Known issues
- Asmedia ASM106x controllers are not working in the mPCIe2 slot
- Some XHCI USB booting stability issues after soft-reset

## [v4.5.4] - 2017-01-24
### Added
- add target for APU3 board (EHCI0 is enabled)

### Changed
- add coreboot build string message during boot
- change SeaBIOS version to 1.9.2.4
- optimizations for SeaBIOS config, faster boot

### Fixed
- fix `WARNING - Timeout at i8042_flush:71!` message during boot
- fix `ASSERTION ERROR: file 'src/northbridge/amd/pi/agesawrapper.c', line 305`
  message during boot
- fix disabling/enabling iPXE rom by `sortbootorder`
- fix custom menu/config options for iPXE rom

## [v4.5.3] - 2017-01-12
### Changed
- bootorder file align to 4K in flash
- add legacy logs on boot
- PCEngines SeaBIOS enabled by default
- turn off D4 and D5 leds on boot
- enable power on after power failure

[Unreleased]: https://github.com/pcengines/coreboot/compare/v4.5.5.2...coreboot-4.5.x
[v4.5.5.2]: https://github.com/pcengines/coreboot/compare/v4.5.5.2...v4.5.5.1
[v4.5.5.1]: https://github.com/pcengines/coreboot/compare/v4.5.5.1...v4.5.5
[v4.5.5]: https://github.com/pcengines/coreboot/compare/v4.5.5...v4.5.4
[v4.5.4]: https://github.com/pcengines/coreboot/compare/v4.5.4...v4.5.3
[v4.5.3]: https://github.com/pcengines/coreboot/compare/v4.5.3...v4.5.2
