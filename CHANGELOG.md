Change log for PC Engines fork of coreboot
==========================================

Releases 4.0.x are based on PC Engines 20160304 release.
Releases 4.5.x and 4.6.x are based on mainline support submitted in
[this gerrit ref](https://review.coreboot.org/#/c/14138/).

## Quick build instructions
1. After cloning run `make crossgcc-i386` to build the toolchain.
2. `make menuconfig` and set:
  * `Mainboard/Mainboard vendor` → `PCEngines`
  * `Mainboard/Mainboard model` → `APU2`, `APU3` or `APU5`
3. `make`

## [Unreleased]
## [v4.6.10] - 2018-06-08
### Changed
- Updated SeaBIOS to 1.11.0.5
- Updated sortbootorder to v4.6.9

### Added
- S1 button support for apu5b

## [v4.6.9] - 2018-05-11
### Added
- Infrastructure to include microcode update
- Support for new building system pce-fw-builder

### Fixed
- Building is now possible with official coreboot-sdk container
- Retrieving board serial number

## [v4.6.8] - 2018-04-06
### Added
- Full feature and build support for APU1
- Serial console enable with S1 button feature

### Changed
- Updated sortbootorder to v4.6.8
- Switched from IDE to AHCI mode for SATA controller
- Updated SeaBIOS to 1.11.0.4

## [v4.6.7] - 2018-03-01
### Fixed
- SD cards performance drop
- SMBIOS part number format

### Changed
- xHCI has been enabled

## [v4.6.6] - 2018-01-31
### Changed
- Updated SeaBIOS to 1.11.0.3

### Fixed
- Memtest86+ screen refresh for serial

### Added
- Enabled ATA UDMA in SeaBIOS

## [v4.6.5] - 2017-12-29
### Added
- Apu features default values to `bootorder_def` file

### Changed
- Updated SeaBIOS to 1.11.0.2
- Updated sortbootorder to v4.6.5
- Forced EHCI controller for front USB ports
- Disabled xHCI controller in SeaBIOS

### Fixed
- Setting correct PCI_ROM_ID for iPXE depending on platform

## [v4.6.4] - 2017-11-30
### Added
- SPI support
- Security register support for reading serial number

### Changed
- Upgrade SeaBIOS to 1.11.0.1
- Update sortbootorder to v4.6.4
- Switch Memtest86+ to mainline

## [v4.6.3] - 2017-10-30
### Added
- Runtime configuration of UARTc/d, EHCI and mPCIe2 CLK for APU2/3/5

## [v4.6.2] - 2017-09-29
### Added
- "Disable retrieving SPD info" feature in Memtest86+
- Piotr Król and Kamil Wcisło as PC Engines platforms maintainers

### Changed
- APU3 and APU5 targets are now available as APU2 variants. This change removes
redundant code which was similar for APU2/3/5 boards.

### Fixed
- Date format in sign of life string

## [v4.6.1] - 2017-08-30
### Added
- APU5 target

## [v4.6.0] - 2017-07-21
### Added
- Allow to force GPP3 PCIe CLK attached to mPCIe2 slot based on Konfig option
  This helps in some cases, one example could be
  [mPCIe Ethernet extension card](https://github.com/pcengines/apu2-documentation/blob/master/docs/debug/mpcie_ethernet.md)

### Changed
- coreboot updated to [mainline 4.6 tag](https://github.com/coreboot/coreboot/tree/4.6)
- Update sortbootorder to v4.5.7

## [v4.5.8] - 2017-06-30
### Changed
- Moved sign of life strings (except for memory information) from `mainboard.c`
  to `romstage.c`. They are printed ~0.3s after power on, instead of over 2s.
- Update sortbootorder to v4.5.6
- Use the same sortbootorder for both mainline and legacy
- Enabled sortbootorder payload compression
- Update SeaBIOS to rel-1.10.2.1

### Fixed
- Fixed SeaBIOS debug level option in Kconfig

## [v4.5.7] - 2017-06-08
### Added
- print BIOS version on startup
- correct BIOS version in `SMBIOS`

### Changed
- set board names prefix to "PC Engines" (Kconfig)

## [v4.5.6] - 2017-05-30
### Changed
- updated sortbootorder to v4.5.5

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

[Unreleased]: https://github.com/pcengines/coreboot/compare/v4.6.10...coreboot-4.6.x
[v4.6.10]: https://github.com/pcengines/coreboot/compare/v4.6.9...v4.6.10
[v4.6.9]: https://github.com/pcengines/coreboot/compare/v4.6.8...v4.6.9
[v4.6.8]: https://github.com/pcengines/coreboot/compare/v4.6.7...v4.6.8
[v4.6.7]: https://github.com/pcengines/coreboot/compare/v4.6.6...v4.6.7
[v4.6.6]: https://github.com/pcengines/coreboot/compare/v4.6.5...v4.6.6
[v4.6.5]: https://github.com/pcengines/coreboot/compare/v4.6.4...v4.6.5
[v4.6.4]: https://github.com/pcengines/coreboot/compare/v4.6.3...v4.6.4
[v4.6.3]: https://github.com/pcengines/coreboot/compare/v4.6.2...v4.6.3
[v4.6.2]: https://github.com/pcengines/coreboot/compare/v4.6.1...v4.6.2
[v4.6.1]: https://github.com/pcengines/coreboot/compare/v4.6.0...v4.6.1
[v4.6.0]: https://github.com/pcengines/coreboot/compare/v4.5.8...v4.6.0
[v4.5.8]: https://github.com/pcengines/coreboot/compare/v4.5.7...v4.5.8
[v4.5.7]: https://github.com/pcengines/coreboot/compare/v4.5.6...v4.5.7
[v4.5.6]: https://github.com/pcengines/coreboot/compare/v4.5.5.2...v4.5.6
[v4.5.5.2]: https://github.com/pcengines/coreboot/compare/v4.5.5.1...v4.5.5.2
[v4.5.5.1]: https://github.com/pcengines/coreboot/compare/v4.5.5...v4.5.5.1
[v4.5.5]: https://github.com/pcengines/coreboot/compare/v4.5.4...v4.5.5
[v4.5.4]: https://github.com/pcengines/coreboot/compare/v4.5.3...v4.5.4
[v4.5.3]: https://github.com/pcengines/coreboot/compare/v4.5.2...v4.5.3
