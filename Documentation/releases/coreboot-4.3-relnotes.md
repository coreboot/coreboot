coreboot 4.3 release notes
==========================

The "Oh, has FOSDEM started?" release

Dear coreboot community,

today marks the release of coreboot 4.3, the third release on our time
based release schedule. Since the last release, 1030 commits by 114
authors added a net total of 17500 lines to the source code. Thank you
to all who contributed!

The release tarballs are available at http://www.coreboot.org/releases/.
There's also a 4.3 tag and branch in the git repository.

Besides the usual addition of new mainboards (14) and chipsets
(various), a big theme of the development since 4.2 was cleaning up the
code: 20 mainboards were removed that aren't on the market for years
(and even hard to get on Ebay). For several parts of the tree, we
established tighter controls, making errors out of what were warnings
(and cleaning up the code to match) and provided better tests for
various aspects of the tree, and in general tried to establish a more
consistent structure across the code base.

Besides that, we had various improvements across the tree, each
important when using the hardware, but to numerous for individual shout
outs. Martin compiled a list that's best posted verbatim. Thanks Martin!

Log of commit 529fd81f640fa514ea4c443dd561086e7c582a64 to commit
1bf5e6409678d04fd15f9625460078853118521c for a total of 1030 commits:

Mainboards
----------

### Added 14 mainboards

* asus/kfsn4-dre_k8: Native init Dual AMD K8 CPUs & Nvidia CK804
  southbridge
* esd/atom15: Bay Trail SOC mainboard using Intel's FSP
* gigabyte/ga-g41m-es2l: Intel Core 2 / Native init x4x NB / I82801GX SB
* google/guado: Intel Broadwell chromebox (Asus Chromebox CN62)
* google/oak: Mediatek MT8173 SoC chromebook
* google/tidus: Intel Broadwell chromebox (Lenovo ThinkCentre Chromebox)
* google/veyron_emile: Rockchip RK3288 SoC board
* intel/d510mo: Native init Intel Pineview with Intel I82801GX
  southbridge
* intel/littleplains: Intel Atom c2000 (Rangeley) SoC board
* intel/stargo2: Intel Ivy Bridge / Cave Creek usint Intel's FSP
* lenovo/r400: Intel Core 2 / Native init GM45 NB / Intel I82801IX SB
* lenovo/t500: Intel Core 2 / Native init GM45 NB / Intel I82801IX SB
* purism/librem13: Intel Broadwell Laptop using Intel MRC
* sunw/ultra40m2: Native init Dual AMD K8 Processors & Nvidia MCP55 SB

### Removed 20 mainboards

* arima/hdama
* digitallogic/adl855pc
* ibm/e325, e326
* intel/sklrvp
* iwill/dk8s2, dk8x
* newisys/khepri
* tyan/s2735, s2850, s2875, s2880, s2881 & s2882
* tyan/s2885, s2891, s2892, s2895, s4880 & s4882

### Improvements to mainboards

* amd/bettong: fixes to Interrupts, Memory config, S4, EMMC, UARTS
* asus/kgpe-d16: IOMMU and memory fixes, Add CMOS options, Enable GART
* intel/strago: GPIO, DDR, & SD config, FSP updates, Clock fixes
* ACPI fixes across various platforms
* Many individual fixes to other mainboards

### Continued updates for the Intel Skylake platform

* google/chell, glados, & lars: FSP & Memory updates, Add Fan & NHLT
  support
* intel/kunimitsu: FSP & GPIO updates, Add Fan & NHLT (audio) support

Build system
------------
* Update build to use FMAP based firmware layout with multiple cbfs
  sections
* Enable Kconfig strict mode - Kconfig warnings are no longer allowed.
* Enable ACPI warnings are errors in IASL - warnings are no longer
  allowed.
* Tighten checking on toolchains and give feedback to users if there are
  issues
* Updates to get the ADA compiler to work correctly for coreboot
* Various improvements to Makefiles and build scripts
* Cleanup of CBFS file handling

Utilities
---------
* cleanups and improvements to many of the utilities
* cbfstool: Many fixes and extensions to integrate with FMAP
* Add amdfwtool to combine AMD firmware blobs instead of using shell
  scripts.
* Toolchain updates: new versions of GMP & MPFR. Add ADA.
* Updates for building on NetBSD & OS X

Payloads
--------
* SeaBIOS: Update stable release to 1.9.0
* coreinfo: fix date, hide cursor, use crosscompiler to build
* libpayload: updates for cbfs, XHCI and DesignWare HCD controllers

ARM
---
* Added 1 soc: mediatek/mt8173
* Various fixes for ARM64 platforms

X86
---
* Added 2 northbridges: intel/pineview & x4x
* Removed 1 northbridge: intel/i440lx
* Added 1 southbridge: intel/fsp_i89xx
* Removed 2 southbridge(s): intel/esb6300 & i82801cx
* Rename amd/model_10xxx to family_10h-family_15h.
* ACPI: fix warnings, Add functions for IVRS, DMAR I/O-APIC and HPET
  entries
* Work in many areas fixing issues compiling in 64-bit
* Numerous other fixes across the tree

Areas with significant work on updates and fixes
------------------------------------------------
* cpu/amd/model_fxx
* intel/fsp1_x: Fix timestanps & postcodes, add native CAR & microcode
* nb/amd/amdfam10: Add S3, voltage & ACPI, speed fixes & MANY other
  changes
* nb/amd/amdmct: Add S3, mem voltage, Fix performance & MANY other
  changes
* nb/intel/sandybridge: Add IOMMU & ACPI DMAR support, Memory cleanup
* soc/intel/braswell: FSP & ACPI updates, GPIO & clock Fixes
* soc/intel/fsp_baytrail: GPIO, microcode and Interrupt updates.
* soc/intel/skylake: FSP, Power/Thermal & GPIO Updates, Add NHLT support
* sb/amd/sb700: Add ACPI & CMOS Setting support, SATA & clock Fixes

MIPS
----
* Imgtec Pistachio: Memory, PLL & I2C fixes, add reset

SuperIO
-------
* Expand functionality for ite/it8718f & nuvoton/nct5572d superio
  devices

### Added 3 SIOs

* intel/i8900
* winbond/w83667hg-a & wpcd376i

### Removed 6 SIOs

* fintek/f71889
* ite/it8661f
* nsc/pc8374 & pc97307
* nuvoton/nct6776
* smsc/fdc37m60x

Lib
---
* Several updates for reading EDID tables

MISC
----
* Commonlib: continued updates for cbfs changes
* Work on getting license headers on all coreboot files
* Drop the third paragraph of GPL copyright header across all of
  coreboot

Submodules
----------
* 3rdparty/blobs: Update to CarrizoPI 1.1.0.1 (Binary PI 1.5)

coreboot statistics
-------------------
Total commits: 1030
Total authors: 114
New authors: 46
Total Reviewers: 41
Total lines added: 88255
Total lines removed: -70735
Total delta: 17520
