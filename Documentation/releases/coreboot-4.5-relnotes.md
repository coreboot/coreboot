coreboot 4.5 release notes
==========================

We are happy to announce the release of coreboot 4.5

The 4.5 release covers commit 80a3df260767 to commit 0bc12abc2b26.

This release is the first since the project switched from doing
quarterly releases to doing biannual releases.  The next release will be
in April of 2017.

Since the last release in April, the coreboot project has had 1889
commits by 119 authors.

The release tarballs and gpg signatures are available in the usual place
at <https://www.coreboot.org/downloads>

There is a 4.5 tag in the git repository, and a branch will be created
as needed.


Areas with significant updates
------------------------------

### Toolchain (29 commits)
* Updated mpfr version from 3.1.3 to 3.1.4
* Updated gcc version from 5.2.0 to 5.3.0
* Updated binutils version from 2.25 to 2.26.1 & Fix aarch64 build
  problem
* Updated gdb version from 7.9.1 to 7.11
* Updated iasl version from 20160318 to 20160831
* Updated python version from 3.4.3 to 3.5.1
* Updated expat version from 2.1.0 to 2.1.1
* Updated llvm / clang version from 3.7.1 to 3.8.0
* Updated make version from 4.1 to 4.2.1

### Build system (32 commits)
* Updates for cbfstool / fmap changes
* Order per-region files to optimize placement success
* Add support for the ADA language and toolchain.

### Utilities (103 commits)
* Lint - Update checkpatch.pl, add tools  to find non-ascii &
  unprintable chars and to verify a single newline at the end of files
* cbfstool - Update for Linux payloads, Honor FSP modules addresses, fix
  elf parsing
* Sconfig - Add 10 bit addressing mode for i2c devices, add generic
  device type, support strings, pass in devicetree filename
* General code cleanup (197 commits)
* Cleaning up code formatting and whitespace
* Fix spelling & capitalization
* Removing commented out code
* Transition away from device_t

### TPM (55 commits)
* Add support for Trusted Platform Module 2.0
* SPI & refactored I2C TPM driver

### Drivers (54 commits)
* Add ACPI support in several drivers
* coreboot_tables -  Extend serial port description
* Elog - refactor, add debug info
* I2C - add generic driver,
* SPI - Add new chip support, major refactoring, don't assume SPI flash
  boot device

### Lib (33 commits)
* Add real-time-clock functions
* Add RW boot device construct
* reg_script updates: add to bootblock, add xor support, add display
  support
* Timestamp fixes & updates

### Vendorcode
* AMD (14 commits) - Cleanup, add libagesa.a builds, remove unused code.
* Google (22 commits) - vboot2 updates and cleanup
* Intel (86 commits) - Add Intel FSP 2.0, update Broadwell DE support

### Payloads (37 commits)
* Subpayload support got extend and is enabled by default.
* nvramcui: refactor, update build
* SeaBIOS: Update stable version to 1.9.3, add bootorder file
* iPXE: Update stable version to the last commit of July 2016
* Fix broken linux boot sequence

Mainboard changes
-----------------

### Added 13 mainboards, plus a few mainboard variants not included here
* ADI RCC-DFF networking board (adi/rcc-dff) - intel/rangeley SoC
* AMD Evaluation Board DB-FT3B-LC (amd/db-ft3b-lc) - amd/00730F01
  (Family 16h  Models 30h-3Fh (Mullins)) CPU
* AMD f2950 / TONK 1201/2 Board (amd/f2950) - amd/geode_lx CPU
* Apple iMAC 5.2 (apple/imac52) - intel/i945 CPU
* Unibap Development Kit ODE E21XX - amd/00730F01 (Family 16h Models
  30h-3Fh (Mullins)) CPU
* elmex/pcm205400 - amd/Family_14 CPU
* elmex/pcm205401 - amd/Family_14 CPU
* Lenovo N21 chromebook (google/enguarde) - intel/baytrail SoC
* google/gale - Qualcomm IPQ40XX SoC
* AOpen Chromebox (google/ninja) - intel/baytrail SoC
* google/reef - intel/apollolake SoC
* Acer Chromebox CXI2 (google/rikku) - intel/Broadwell SoC
* google/rotor - marvell/MVMAP2315 SoC

### Removed 5 mainboards:
These were all development boards not available to the public.
* google/bolt - intel/haswell - removed in commit 139314b
* google/rush - nvidia/tegra132 - removed in commit e67cd9e
* google/rush_ryu - nvidia/tegra132 - removed in commit 0c63415
* google/slippy - intel/haswell - removed in commit bc24b85
* intel/amenia - intel/apollolake - removed in commit c2586db

### Existing boards with significant updates
* asus/kgpe-d16 - amd/socket_G34 - Add TPM support, enable secondary
  serial port
* emulation/spike-riscv: RISC-V -clean up, use generic bootblock, look
  for  CBFS in RAM, reimplement SBI
* google/gru - rockchip/RK3399 SoC (76 commits) - Board bringup
* google/oak - mediatek/mt8173 SoC- Add Elm variant, update memory,
  configure display, initialize touchscreen gpio
* intel/galilleo- intel/quark SoC (14 commits) - Board bringup, add
  galileo gen1 support, switch to FSP2.0
* intel/minnowmax - intel/fsp_baytrail SoC - Enable all PCIe ports,
  Program GPIO for power LED
* lenovo/x60 - intel/socket_mPGA478 - init GPIOs before dock check, add
  hda verb table
* siemens/mc_bdx1 - intel/fsp_broadwell_de SoC - Add external RTC, Set
  up MAC addresses, Update IRQs
* siemens/mc_tcu3 - intel/fsp_baytrail SoC - cleanup & LCD panel updates

Changes in chips
----------------
### Moved 3 northbridge/southbridge pairs to soc:
* dmp/vortex86ex
* intel/sch
* rdc/r8610

### Added 2 socs:
* marvell/mvmap2315 (12 commits)
* qualcomm/ipq40xx (22 commits)

### Removed 1 soc:
* nvidia/tegra132 - removed in commit 9ba0699

### Added 2 sios:
* nuvoton/nct6776
* nuvoton/nct6791d

### ARM (34 commits)
* Add armv7-r configuration

#### rockchip/rk3399 (73 commits)
* Bringup, memory updates

### RISC-V (40 commits)
* Improve and refactor trap handling

### X86 (225 commits)

### ACPI (40 commits)
* Add support for writing various entries and descriptor
  types, Add common definitions, Use 'GOOG' id for coreboot table
* amd/mct_ddr3 northbridge: Support non-ECC DIMMs, Update SMBIOS,
  various fixes
* arch/x86: many postcar stage updates, add common ACPI definitions,
  Support "weak" BIST and timestamp save routines
* intel/apollolake SoC (211 commits) - Chip bringup, Update bootblock
* intel/common: ACPI updates, Add smihandler, LPSS I2C driver, and  IGD
  OpRegion support
* intel/fsp_broadwell_de: IRQ fixes, SPI message fixes, Add DMAR table
  to ACPI
* intel/gm45 northbridge: Fix text mode init, enable vesa framebuffer,
  use VGA if connected
* intel/i945 northbridge: add native VGA init, Update divisor
  calculations
* intel/quark SoC (62 commits) - Chip bringup, add Fsp2.0 support,
  updates for serial console
* intel/skylake CPU (61 commits) - Finished Skylake bringup, start
  updating for Kabylake FSP
* intel/x4x northbridge (13 commits) - Memory & Graphics updates

Submodules
----------
Updated 4 submodules
* 3rdparty/blobs (6 commits)
* 3rdparty/arm-trusted-firmware (425 commits)
* 3rdparty/vboot (61 commits)
* 3rdparty/chromeec/ (676 commits)

Tested boards
-------------
The following boards were tested for this release:
* asrock/e350m1                 4.4-1890
* asus/kfsn4-dre                4.4-1698 / 4.5-17
* asus/kgpe-d16                 4.4-1802 / 4.5-17
* emulation/qemu-q35            4.4-1698 / 4.5-8
* gigabyte/ga-b75m-d3v          4.4-1757
* google/peppy                  4.4-1882
* lenovo/g505s                  4.4-1739
* lenovo/x201                   4.4-1886
* lenovo/x220                   4.4-1746 / 4.5-17

coreboot statistics
-------------------
* Total Commits:            1889
* Average Commits per day: 10.92
* Total authors:             119
* New authors:                47
* Total Reviewers:            67
* Total Submitters:           19
* Total lines added:      164950
* Total lines removed:   -182737
* Total difference:       -17787
