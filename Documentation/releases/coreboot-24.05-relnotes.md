coreboot 24.05 release
========================================================================

The coreboot project is pleased to announce the release of coreboot version
24.05. This update represents three months of hard work and commitment from our
community. With over 20 new members and contributions from more than a hundred
fifty other people in coding, reviewing patches, and other areas, this release
showcases the strength of our collaborative efforts.

With this release, coreboot has expanded its support, adding 25 new platforms or
variants and 2 new processors, further demonstrating our dedication to offering
flexible and adaptable firmware solutions. From laptops and servers to embedded
devices, coreboot 24.05 is designed to enhance a variety of hardware platforms
with its strong features.

We are grateful to all the contributors who have made this release possible.
Your expertise and collaborative efforts continue to propel the coreboot project
forward. We value the participation of everyone in the community, from long-time
developers to those new to the project, and encourage you to explore the new
opportunities that coreboot 24.05 offers.

Our next release will be 24.08, scheduled for mid-August.



Significant or interesting changes
----------------------------------

### Mark 64-bit support as stable

A significant amount of work has gone into fully supporting 64-bit coreboot
builds. There are still additional pieces that are happening, but with SMM
holding page tables itself, we can consider SMM support stable and safe enough
for general use.


### security/tpm: support compiling in multiple TPM drivers

Previously, boards could only be built with code supporting TPM 1.x or TPM 2.x
specifications. This has been updated with code allowing both to be built in
simultaneously, allowing the system to query the TPM. For systems with
soldered-down TPMs or firmware TPM solutions, it’s still possible to specify a
single TPM version so that the code for the other version isn’t included.


### arch/arm64: Add EL1/EL2/EL3 support for arm64

Previously, arch/arm64 required coreboot to run on EL3 due to EL3 register
access. This might be an issue when, for example, one boots into TF-A first and
drops into EL2 for coreboot afterwards.

This patch aims at making arch/arm64 more versatile by removing the current EL3
constraint and allowing arm64 coreboot to run on EL1, EL2 and EL3.

The strategy is to add a Kconfig option (ARM64_CURRENT_EL) which allows us to
specify coreboot's EL upon entry. Based on that, we access the appropriate ELx
registers. So, for example, when running coreboot on EL1, we would not access
vbar_el3 or vbar_el2 but instead vbar_el1. This way, we don't generate faults
when accessing higher-EL registers.



Additional coreboot changes
---------------------------

* util/smmstoretool: support processing ROMs
* cpu/x86: Link page tables in stage if possible
* lib/lzmadecode: Allow for 8 byte reads on 64bit to speed up decompression
* mb/lenovo/*: Set VR12 PSI to fix crash
* Numerous fixes for clang support
* Ongoing code cleanup
* Docs: Replace Recommonmark with MyST Parser. For changes, see the commit
  message in <https://review.coreboot.org/73158>



Changes to external resources
-----------------------------

### Toolchain updates

* util/kconfig: Uprev to Linux 6.8's kconfig
* crossgcc: Upgrade CMake from 3.27.7 to version 3.28.3
* util/crossgcc: Update LLVM from 16.0.6 to 17.0.6
* crossgcc: Upgrade binutils from 2.41 to 2.42
* util/crossgcc/buildgcc: Use Intel mirror for ACPICA


### Git submodule pointers

- amd_blobs: Update from commit id 64cdd7c8ef to ae5fc7d277 (1 commits)
- arm-trusted-firmware: Update from commit id 17bef2248d to 48f1bc9f52 (517
  commits)
- cmocka: Update from commit id 8931845c35 to 8be3737209 (32 commits)
- fsp: Update from commit id 507ef01cce to cc6399e8c7 (14 commits)
- intel-microcode: Update from commit id ece0d294a2 to 41af345005 (1 commit)
- vboot: Update from commit id 3d37d2aafe to 09fcd2184f (27 commits)


#### External payloads

* payloads/U-Boot: Upgrade from U-Boot v2023.07 to v2024.4
* payloads/edk2: Add Kconfig options for LAPIC timer & UFS support
* payloads/Kconfig: Add flat binary as payload option



Platform Updates
----------------

### Added mainboards:

* AMD BirmanPlus for Glinda SoC
* AMD BirmanPlus for Phoenix SoC
* ASROCK Z97 Extreme6
* Dell OptiPlex 7020/9020 MT
* Dell OptiPlex 7020/9020 SFF
* Framework Azalea (Framework 13 AMD 7040)
* Google Brox EC ISH
* Google Bujia
* Google Glassway
* Google Greenbayupoc
* Google Kyogre
* Google Lotso
* Google Nova
* Google Pujjoga
* Google Riven
* Google Skitty
* Google Squirtle
* Google Sundance
* Google Tivviks
* Google Trulo
* Google Veluza
* Google Wugtrio
* Google Yavista
* HP Pro 3500 Series
* Lenovo ThinkCentre M700 / M900 Tiny
* Lenovo ThinkCentre M710s
* Raptor Computing Systems Talos II
* SiFive HiFive Unmatched


### Removed Mainboards

* Intel Alderlake-M RVP
* Intel Alderlake-M RVP with Chrome EC


### Updated SoCs

* Added src/soc/ibm/power9
* Added src/soc/intel/xeon_sp/gnr
* Added src/soc/sifive/fu740



Statistics from the 24.02 to the 24.05 release
--------------------------------------------

* Total Commits: 739
* Average Commits per day: 8.64
* Total lines added: 304721
* Average lines added per commit: 412.34
* Number of patches adding more than 100 lines: 60
* Average lines added per small commit: 37.74
* Total lines removed: 16195
* Average lines removed per commit: 21.91
* Total difference between added and removed: 288526
* Total authors: 131
* New authors: 23


Significant Known and Open Issues
---------------------------------

## coreboot-wide or architecture-wide issues

```{eval-rst}
+-----+-----------------------------------------------------------------+
| #   | Subject                                                         |
+=====+=================================================================+
| 522 | 'region_overlap()' issues due to an integer overflow.           |
+-----+-----------------------------------------------------------------+
| 519 | make gconfig - could not find glade file                        |
+-----+-----------------------------------------------------------------+
| 518 | make xconfig - g++: fatal error: no input files                 |
+-----+-----------------------------------------------------------------+
```


## Payload-specific issues

```{eval-rst}
+-----+-----------------------------------------------------------------+
| #   | Subject                                                         |
+=====+=================================================================+
| 499 | edk2 boot fails with RESOURCE_ALLOCATION_TOP_DOWN enabled       |
+-----+-----------------------------------------------------------------+
| 496 | Missing malloc check in libpayload                              |
+-----+-----------------------------------------------------------------+
| 484 | No USB keyboard support with secondary payloads                 |
+-----+-----------------------------------------------------------------+
| 414 | X9SAE-V: No USB keyboard init on SeaBIOS using Radeon RX 6800XT |
+-----+-----------------------------------------------------------------+
```


## Platform-specific issues

```{eval-rst}
+-----+-----------------------------------------------------------------+
| #   | Subject                                                         |
+=====+=================================================================+
| 524 | X2APIC Options cause Linux to crash on emulation/qemu-i440fx    |
+-----+-----------------------------------------------------------------+
| 517 | lenovo x230 boot stuck with connected external monitor          |
+-----+-----------------------------------------------------------------+
| 509 | SD Card hotplug not working on Apollo Lake                      |
+-----+-----------------------------------------------------------------+
| 507 | Windows GPU driver fails on Google guybrush & skyrim boards     |
+-----+-----------------------------------------------------------------+
| 506 | APL/GML don't boot OS when CPU microcode included "from tree"   |
+-----+-----------------------------------------------------------------+
| 505 | Harcuvar CRB - 15 of 16 cores present in the operating system   |
+-----+-----------------------------------------------------------------+
| 499 | T440p - EDK2 fails with RESOURCE_ALLOCATION_TOP_DOWN enabled    |
+-----+-----------------------------------------------------------------+
| 495 | Stoney Chromebooks not booting PSPSecureOS                      |
+-----+-----------------------------------------------------------------+
| 478 | X200 booting Linux takes a long time with TSC                   |
+-----+-----------------------------------------------------------------+
| 474 | X200s crashes after graphic init with 8GB RAM                   |
+-----+-----------------------------------------------------------------+
| 457 | Haswell (t440p): CAR mem region conflicts with CBFS_SIZE > 8mb  |
+-----+-----------------------------------------------------------------+
| 453 | Intel HDMI / DP Audio not present in Windows after libgfxinit   |
+-----+-----------------------------------------------------------------+
| 449 | ThinkPad T440p fail to start, continuous beeping & LED blinking |
+-----+-----------------------------------------------------------------+
| 448 | Thinkpad T440P ACPI Battery Value Issues                        |
+-----+-----------------------------------------------------------------+
| 446 | Optiplex 9010 No Post                                           |
+-----+-----------------------------------------------------------------+
| 439 | Lenovo X201 Turbo Boost not working (stuck on 2,4GHz)           |
+-----+-----------------------------------------------------------------+
| 427 | x200: Two battery charging issues                               |
+-----+-----------------------------------------------------------------+
| 412 | x230 reboots on suspend                                         |
+-----+-----------------------------------------------------------------+
| 393 | T500 restarts rather than waking up from suspend                |
+-----+-----------------------------------------------------------------+
| 350 | I225 PCIe device not detected on Harcuvar                       |
+-----+-----------------------------------------------------------------+
```


coreboot Links and Contact Information
--------------------------------------

* Main Website: <https://www.coreboot.org>
* Downloads: <https://coreboot.org/downloads.html>
* Source control: <https://review.coreboot.org>
* Documentation: <https://doc.coreboot.org>
* Issue tracker: <https://ticket.coreboot.org/projects/coreboot>
* Donations: <https://coreboot.org/donate.html>
