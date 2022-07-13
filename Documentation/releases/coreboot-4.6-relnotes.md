coreboot 4.6 release notes
==========================

We are happy to announce the April 2017 release of coreboot, version
4.6.

The 4.6 release covers commit e74f5eaa to commit db508565

Since the last release in October 2016, the coreboot project had 1708
commits by 121 authors. The release tarballs and gpg signatures are
available in the usual place at https://www.coreboot.org/downloads

There is a pgp signed 4.6 tag in the git repository, and a branch will
be created as needed.

Changes: Past, ongoing, and future
----------------------------------

### CBMEM console development and the Linux Kernel

Our cbmem debug console was updated with some nice features. The cbmem
console now persists between reboots and is able to be used on some
platforms via late init. Also there is a new Linux kernel driver which
removes the need for the old cbmem tool to read from the cbmem area. You
can find the patch here https://patchwork.kernel.org/patch/9641997/ and
it can be enabled via GOOGLE_MEMCONSOLE_COREBOOT kconfig option in your
kernel - Note that this name may change going forward.

### Critical bugs in TPM 1.2 support

coreboot currently has issues with the TPM 1.2 LPC driver
implementation. This leads to a misbehavior in SeaBIOS where the TPM
gets temporarily deactivated. We plan to publish the bugfix release
4.6.1 when we have these issues sorted out.

### Native graphics and ram init improvements

The native graphics was reworked a while ago and should finally support
Windows. Numerous bug fixes and EDID support is also now available.
Finally, the native ram initialization for sandybridge/ivybridge
platforms got patched and supports more RAM modules.

### New and fresh payloads

SeaBIOS, FiLO, and iPXE were all recently updated to the latest
versions. Https downloads are the default for all payloads now. We
provide the libpayload project which is used for writing own payloads
from scratch. The library is MOSTLY licensed under BSD and recently
received new functionality in order to prepare for the upcoming
replacement for the old nvramcui payload. This new payload is called
cbui and is based on the nuklear graphics library including keyboard and
mouse support. The cbui payload is currently expected to be merged into
the main coreboot tree before the next release.  The upstream repository
is here: https://github.com/siro20/coreboot/tree/cbui/payloads/cbui

### UEFI support: A long road to go

coreboot can be used with the edk2 UEFI implementation which
is open source and available at Github. Sadly it is not currently
integrated into the coreboot build. This has several reasons:

* edk2 only supports GCC 4.8 profile. coreboot is now running on GCC 6.3.0.
* Incompatibilities with code inside the edk2 which has not been updated.

We started to make progress with the integration into our sources and
the hope is that by the end of the summer, we finally support the edk2
payload out-of-the- box. See the current patch state at
http://review.coreboot.org/#/c/15057/

### Fighting blobs and proprietary HW components

coreboot's ultimate goal would be to replace any closed source firmware
stack with free software components. Unfortunately this is not always
possible due to signed binaries such as the Intel ME firmware, the AMD
PSP and microcode. Recently, a way was discovered to let the Intel ME
run in a functional error state and reduce it from 1.5/5MB to 80KB. It's
not perfect but it works from Nehalem up to Skylake based Intel systems.
The tool is now integrated into the coreboot build system. The upstream
repository is https://github.com/corna/me_cleaner

Another ongoing improvement is the new utility blobtool. It is currently
used for generating the flash descriptor and GbE configuration data on
older mainboard which are known to be free software. It can easily be
extended for different binaries with well-defined specifications.

### Did you say Ada?

coreboot now supports Ada, and a lot work was done integrating Ada into
our toolchain. At the moment only the support for formal verification is
missing and will be soon added. At that point, we can prove the absence
of runtime errors in our Ada code. In short, everybody can start
developing Ada code for our project.

The existing Ada code which can be used from now on is another native
graphics initialization which will replace in the long term the current
implementation. The native graphics code supports all Intel platforms up
to skylake. We offer support for HDMI, VGA, DVI and DP external
interfaces as well and is ready to be integrated into our mainboard
implementations.

### Toolchain updates

A new coreboot toolchain is out. The major toolchain change was going
from GCC version 5.3.0 to 6.3.0. There were also minor version updates
to GMP, MPFR, Binutils, GDB, IASL, and Clang.

### Deprecation policy for boards

Starting with this release there will be a policy for deprecating
unmaintained boards. See the end of this announcement for details.

Change Summary
--------------

Build system (20 commits)
* Clean up Kconfig
* Show more useful error messages

Codebase cleanup (94 commits)
* Many fixes for files to pass checkpatch. Lots more to do here.
* Remove commented out code
* Updates to transition away from device_t
* Work to get rid of included C files

Documentation (6 commits)
* Start adding technotes/Design docs
* Add Kconfig documentation

ACPI & acpigen library
* Add GPIO macros
* Clean up and add more functions to ACPIGEN library

EC (26 commits)
* Add roda/it8518 embedded controller

TPM (41 commits)
* Cleanup
* Update ACPI ASL, Runtime generate ACPI table for TPM driver
* Make SPI TPM driver CAR-safe
* Update TPM init sequence

Devices (24 commits)
* Add a new SPI device type
* Allow devicetree accesses in postcar stage
* PCIEXP_ASPM: Unify code with other PCI-e tuning

Lib (28 commits)
* Add option to use Ada code in ramstage
* bootstate: add arch specific hook at coreboot exit
* cbfs: Add API to locate a file from specific region
* Add library to handle SPD data in CBFS or DIMM
* Add region file support
* Turn CBMEM console into a ring buffer that can persist across reboots

Commonlib (11 commits)
* Add xmalloc, xzmalloc and dma routines
* Add input and output buffer helpers

Drivers (29 commits)
* i2c: Pass in i2c_generic_config into i2c_generic_fill_ssdt
* i2c/alps: Add support for ALPS Touchpad driver
* i2c/generic: Add support for GPIO IRQ
* i2c/generic: Enable support for adding PowerResource for device
* i2c/hid: Add generic I2C HID driver
* i2c/max98927: add i2c driver for Maxim 98927 codec
* i2c/wacom_ts: Add support for WCOM touchscreen device driver
* pc80/rtc: Check CMOS checksum BEFORE reading CMOS value
* regulator: Add driver for handling GPIO-based fixed regulator
* storage: Add SD/MMC/eMMC driver based upon depthcharge

SPI interface
* Significant cleanup and refactoring

Include (17 commits)
* cpu/intel: Add MSR to support enabling turbo frequency
* elog: Add all EC event codes

SuperIO (12 commits)
* Updates for ITE SIOs
* Add 2 new chips
* Consolidate code to use common routines

vboot (23 commits)
* Add support for recovery hash space in TPM

RISC-V (25 commits)
* Add lowRISC System On Chip support
* Cbmem patches, move to common architectural code

ARM (16 commits)
* Init new serial struct variables for samsung exynos5420 & allwinner
  a10
* Fix verstage to use proper assembly versions of mem*()

RockChip RK3399 & platforms (46 commits)
* Memory, I2C, USB, SD & Display fixes

X86 Intel (193 commits)
* Broadwell/Sata: Add support for setting IOBP registers for Ports 2 and
  3.
* cpu/intel/common: Add/Use common function to set virtualization
* drivers/intel/fsp1_1: Fix boot failure for non-verstage case
* drivers/intel/fsp2_0: Reset on invalid stage cache.
* drivers/intel/gma: Add textmode using libgfxinit & use scaling to
  simplify  config
* drivers/intel/mipi_camera: Add MIPI CSI camera SSDT generator
* broadwell_de: Add SMM code
* intelblocks/msr: Move intel x86 MSR definition into common location
* intel/broadwell: Use the correct SATA port config for setting IOBP
  register
* intel/wifi: Create ACPI objects for wifi SAR configuration
* lynxpoint bd82x6x: Enable PCI-to-PCI bridge
* mrc: Add support for separate training cache in recovery mode
* nb/i945/early_init.c: Add FSB800 and 1067 to Egress Port Virtual
  Channel
* nb/i945/raminit: Add fixes for 800MHz & 1067MHz FSB CPUs
* nb/intel/gm45: Fix panel-power-sequence clock divisor
* nb/intel/i945: Fix PEG port on 945gc & sdram_enhanced_addressing for
  channel1
* nb/intel/pineview: Move to early cbmem
* nb/pineview/raminit: Skip Jedec init on resume, fix hot reset path
* nb/intel/sandybridge/gma: Always initialize DP buffer translation
* sb/ich7: Use common/gpio.h to set up GPIOs
* sb/intel/bd82x6x: Add TCO_Lock in finalize step
* sb/intel/common/gpio: Support ICH9M and prior
* sb/intel/i82801gx: Add i2c_block_read to smbus.h

sandybridge/raminit
* Fix CAS Write Latency, disable_channel, normalize_training & odt stretch
* Separate Sandybridge and Ivybridge
* Reset internal state on fallback attempts
* Find CMD rate per channel

soc/intel/common
* Add common routines for HECI, ITSS, PCR, RTC, systemagent, UART, XHCI,
& LPSS
* Save Memory DIMM Information in SMBIOS table

Apollolake (183 commits)
* Switch to common routines for LPSS, RTC, ITSS, UART, XHCI, & PCR
* Enable turbo
* Add save/restore variable MRC cache
* Allow ApolloLake SoC to use FSP CAR Init
* Allow USB2 eye pattern configuration in devicetree

Quark & platforms (14 commits)
* Fix I2c & Serial port config
* Add vboot support

ga-g41m-es2l, x4x northbridge & LGA775 (23 commits)
* Memory fixes
* Add S3 suspend/resume

Skylake / Kabylake (208 commits)
* Add devicetree settings for acoustic noise mitigation
* Perform CPU MP Init before FSP-S Init
* Add support for GSPI controller & add GSPI controller get_config
support
* Enable Systemagent IMGU
* Add USB Port Over Current support & Expand USB OC pins support PCH-H
* Extract DIMM Information from FSP MEM INFO HOB
* Add support for eSPI SMI events
* Update ACPI & various methods

X86 amd (116 commits)
* ACPI S3: Remove HIGH_MEMORY_SAVE where possible
* AMD fam10 binaryPI: Remove invalid PCI ops on CPU domain
* binaryPI platforms: Drop ACPI S3 support
* sb/amd/sb700: Disable LPC ROM mapping when SPI Flash is used
* southbridge/amd: Add LPC bridge acpi path for Family14 and SB800
* arch/x86: remove CAR global migration when postcar stage is used
* x86/acpi: Add VFCT table

AMD: vendorcode, AGESA, binaryPI (72 commits)
* Cleanup & consolidate duplicate code
* Fork for new cache-as-ram init code & Fix binaryPI cache-as-ram
* Refactor S3 support functions and Delay ACPI S3 backup until ramstage
  loader

amd/mct:
* Fix CsMux45 configuration, maximum read latency, & DQ mask calculation

Mainboards (198 commits)
* asus/f2a85-m_le: Activate IOMMU support
* lenovo/h8: Add USB Always On
* google/oak: Enable dual DSI for rowan and the BOE 8-lane MIPI/DSI panel
* google/parrot: Fix keyboard interrupts, DSDT
* google/veyron: Work around RAM code strapping error
* lenovo/t400: Rewrite dock from t60
* intel/d510mo: enable ACPI resume from S3
* intel/d945gclf: Fix resume from S3 suspend
* lenovo/t400: Implement hybrid graphic in romstage
* Enable libgfxinit on lenovo/t420 & x230, kontron/ktqm77, google/slippy
* lenovo/x60,t60: Move EC CMOS parameters in checksummed space
* mc_tcu3: Do not abort initialization of PTN3460 when HW-ID is missing
* mc_tcu3: Swap LVDS even and odd lanes for a certain hardware
* purism/librem13: Enable support for M.2 NVMe & Fix M.2 issues

Payloads (53 commits)
* Update FILO, SeaBIOS, & iPXE versions
* Many libpayload fixes and updates

Toolchain (19 commits)
* Update GCC, Binutils, GMP, MPFR, GDB, IASL and LLVM

Utilities: (145 commits)
* abuild: Build saved config files and print failed builds at the end
* autoport: Create superiotool logs and fix romstage generator
* board-status: Update bucketize script and add README file
* cbfstool: Add cbfs-compression-tool and enable adding precompressed
  files
* cbmem: Add custom aligned memcpy() implementation
* ectool: Fix timeout on sending EC command and support OpenBSD
* ifdtool: Fix ICH Gbe unlock
* intelmetool: Add support for Wildcat Point LP, fix segfault on edge
  cases
* inteltool: Add support for CH6-10, ICH10, Wildcat Point-LP and fix ICH
SPIBAR
* sconfig: Add a new SPI device type
* superiotool: Add new chips - IT8783E/F, W83627DHG, W83627EHG, F71808A

Changes in chips
----------------

Added 1 processor & northbridge:
* amd/pi/00670F00

Added 1 soc:
* lowrisc/lowrisc

Removed 1 northbridge:
* intel/e7501

Added 2 sios:
* fintek/f71808a
* ite/it8783ef

Mainboard changes
-----------------

Added 52 mainboards and variants:
* AMD Gardenia - AMD Stoney Ridge
* Asus F2A85_M_PRO - AMD Family 15h Trinity
* Asus P5GC_MX - Intel Socket LGA775
* Gigabyte GA_945GCM_S2L & GA_945GCM_S2C variant - Intel Socket LGA775
* Google Auron variants: Yuna, Gandof, Lulu - Intel Broadwell
* Google Beltino variants: McCloud, Monroe, Tricky, Zako - Intel Haswell
* Google Eve - Intel Kabylake
* Google Fizz - Intel Kabylake
* Google Gru variants: Bob, Scarlet - RockChip RK3399
* Google Oak variants: Hana, Rowan - MediaTek MT8173
* Google Poppy & Soraka variant - Intel Kabylake
* Google Rambi variants: Banjo, Candy, Clapper, Glimmer, Gnawty, Heli,
  Kip, Orco, Quawks, Squawks, Sumo, Swanky, & Winky - Intel Baytrail
* Google Reef variants: Sand, Snappy, Nasher - Intel Apollolake
* Google Slippy variants: Leon, Wolf - Intel Haswell
* Intel KBLRVP3 & KBLRVP7 - Intel Kabylake
* Intel LEAFHILL - Intel Apollolake
* Intel MINNOW3 - Intel Apollolake
* Lenovo L520: Intel Sandybridge
* Lenovo S230U: Intel Ivybridge
* Lenovo X1 Carbon GEN1 - Intel Sandybridge
* lowRISC NEXYS4DDR - RiscV
* MSI MS7721 - AMD Bulldozer
* PC Engines APU2 - AMD Jaguar
* RODA RV11 & RW11 variant - Intel Ivybridge
* Sapphire Pure Platinum H61 - Intel Socket LGA1155
* Siemens MC_APL1 - Intel Apollolake

Removed 10 mainboard variants:
* Google Auron (Still available as a base-board for variants)
* Google Veyron Chromeboxes: Brain, Danger, Emile, Romy
* Google Veyron Test Projects: Gus, Nicky, Pinky, Shark, Thea

Utilities
---------

Added 2 new utilities:
* blobtool
* me_cleaner

Submodules
----------

Updated 5 submodules
* 3rdparty/blobs (10 commits)
* 3rdparty/arm-trusted-firmware (172 commits)
* 3rdparty/vboot (158 commits)
* 3rdparty/chromeec/ (810 commits)
* util/nvidia/cbootimage (2 commits)

Tested boards
-------------

The following boards were tested recently:
* emulation qemu-q35                    4.6-1
* asus kgpe-d16                         4.6-1
* asus kfsn4-dre                        4.6-1
* asus p5gc-mx                          4.6-1
* lenovo x60                            4.5-1681 / 4.6-7
* lenovo x230                           4.5-1674 / 4.6-27
* asrock e350m1                         4.5-1662 / 4.6-7
* lenovo t420                           4.5-1640
* lenovo x200                           4.5-1598 / 4.6-33
* sapphire pureplatinumh61              4.5-1575
* gigabyte ga-945gcm-s2l                4.5-1568
* lenovo t400                           4.5-1564
* lenovo t60                            4.5-1559
* gigabyte m57sli                       4.5-1526
* purism librem13                       4.5-1503
* gigabyte ga-g41m-es2l                 4.5-1444
* google slippy                         4.5-1441
* intel d510mo                          4.5-1341

coreboot statistics from e74f5eaa43 to db508565d2
-------------------------------------------------

* Total Commits: 1708
* Average Commits per day: 8.75
* Total authors: 121
* New authors: 34
* Total Reviewers: 72
* Total Submitters: 19
* Total lines added: 177576
* Total lines removed: - 107460
* Total difference: 70116

Code removal after the 4.6 release
----------------------------------

The only platform currently scheduled for removal is the
bifferos/bifferboard & soc/rdc/r8610. This platform is one of two that
still uses romcc to compile romstage and doesn't have cache-as-ram in
romstage - the others were all removed long ago. Additionally, it seems
to be impossible to buy, so as far as it can be determined, no testing
has been done recently.

Code removal after the 4.7 release
----------------------------------

One of the things that the coreboot project has struggled with is how to
maintain the older platforms while still moving the rest of the
platforms forward. Currently there are numerous platforms in the
codebase which have not been well maintained.

Starting with the 4.7 release in October, the coreboot leadership is
going to set standards that platforms are expected to meet to remain in
the active codebase. These will generally be announced 3 - 6 months in
advance to give time to get changes in. The expectation is not
necessarily even that all work to meet the goal will be completed, but
it is expected that a reasonable effort has started to meet the goal at
the time of the release. Regardless of the work that's been done,
platforms which have not met the goal by the following release will be
removed.

The next expectation that will need to be met for all platforms is cbmem
in romstage. This currently affects numerous platforms, including most,
if not all of AMD's platforms. Work to update many of these platforms
has started, but there are others that have not made any progress
towards this goal. A list of the platforms that are affected by this
will be sent to the mailing list shortly.

Code removal after the 4.8 release
----------------------------------

To further clean things up, starting with the 4.8 release, any platform
that does not have a successful boot logged in the board_status repo in
the previous year (that is, within the previous two releases) will be
removed from the maintained coreboot codebase. Chips that do not have
any associated boards will also be removed. These platforms will be
announced before the release so that there is time for people to test if
desired.

This is not meant to be a high bar, but as a measure to clean up the
codebase and eliminate boards and chips that are actually no longer
being used. The cleanup will happen just after the release, so the
removed platforms will still be available in the release branch if
desired. If there is still interest, developers can bring back old chips
and boards by porting them to the new tree (and bringing them to current
standards).

This gives everyone until April 2018 to get any boards that they care
about tested before the first removal.

All the code removal information will also be sent to the mailing list
along with additional details.
