Announcing coreboot 4.4
=======================

We are happy to announce the release of coreboot 4.4.  This is our
fourth quarterly release.  Since the last release, we've had 850 commits
by 90 authors adding 59000 lines to the codebase.

The release tarballs are available at https://www.coreboot.org/releases/
There is a 4.4 tag and branch in the git repository.

Log of commit 3141eac900 to commit 588ccaa9a7

Major areas that received significant changes in for this release:
* Build system (30 commits) - Add postcar stage, 'timeless' builds,
  extend site-local, test toolchain by version string, update
  dependencies, catch ACPI errors, add additional macros.
* Toolchain updates (40+ patches) - Update IASL to v20160318 , LLVM to
  v3.7.1, add GNU make, add nds32le GCC compiler
* Lint tools (30 patches) - Update existing lint utilities, add lint
  tests for executable bit, make sure site-local isn't committed, add
  test to break all lint tests.
* Payloads (60 commits) - Fixes for libpayload, coreinfo and nvramcui,
  add new payloads, see below.
* Maintainers file - (8 patches) - continue adding maintainers for
  various areas.
* Documentation for adding Intel FSP-based platforms (20 commits)

Mainboards
----------
### Added 9 mainboards
* asus/kcma-d8
* emulation/qemu-power8
* google/auron_paine
* google/gru
* intel/amenia
* intel/apollolake_rvp
* intel/camelbackmountain_fsp
* intel/galileo
* lenovo/t420

### Existing boards with significant updates
* asus/kgpe-d16
* google/oak
* google/chell
* intel/kunimitsu

Changes in chips
----------------
### Added 1 new architecture
* power8

### Added 1 processor
* qemu-power8

### Added 5 socs
* intel/apollolake
* intel/fsp_broadwell_de
* intel/quark
* marvell/armada38x
* rockchip/rk3399

### Existing chip areas with many changes
* cpuamd/mct_ddr3
* drivers/intel/fsp2_0
* northbridge/intel/sandybridge/raminit
* soc/intel/apollolake
* soc/intel/fsp_baytrail
* soc/intel/skylake
* soc/mediatek/mt8173

### Added 1 new vendorcode directory
* siemens

Submodules
----------
### Added 1 submodule
* chromeec

### Updated 3 submodules
* 3rdparty/arm-trusted-firmware (329 commits)
* 3rdparty/vboot (28 commits)
* util/nvidia/cbootimage (13 commits)

Other
-----
### Added 4 payloads
* depthcharge: For ChromeOS verified boot
* iPXE: For network booting
* Memtest86+: Updated with fixes for correctly testing coreboot with
  payloads
* U-Boot (Experimental): Alternate payload for booting an OS

### Added 6 utilities
* archive - Concatenates files into a single blob with an indexed header
* chromeos - Download and extract blobs from a ChromeOS image
* futility - vboot Firmware utility
* intelmetool - Shows information about the Intel ME on a platform.
* marvell/doimage_mv - No usage notes
* post - Simple utility to test post cards

coreboot statistics
-------------------
* Total Commits:          850
* Total authors:           90
* New authors:             28
* Total Reviewers:         40
* Total Submitters:        17
* Total lines added:    74054
* Total lines removed: -15056
* Total difference:     58998
