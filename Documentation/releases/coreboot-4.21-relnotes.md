Upcoming release - coreboot 4.21
========================================================================

The 4.21 release is scheduled for August 21st, 2023

In the past quarter year, the coreboot project has gotten over 1200 new
patches from around 140 authors, 20 of whom contributed for the first
time.

Thank you to all of our donors, the code contributors, the people who
take time to review all of those patches and all of the people who care
about the coreboot project. There have been a number of new companies
starting to use coreboot recently, and we appreciate all of the
contributions and support.

## Upcoming switch from master branch to main branch

Historically, the initial branch that was created in a new git
repository was named ‘master’. In line with many other projects,
coreboot has decided to switch away from this name and use the name
‘main’ instead. You can read about the initial reasoning on the SFC’s
website: https://sfconservancy.org/news/2020/jun/23/gitbranchname/

At some point before the 4.22 release, coreboot will be switching from
the master branch to the main branch. This shouldn’t be a difficult
change for most people, as everyone will just have to rebase on top of
a different branch name.

We’ve already created the main branch, and it is currently synced with
the master branch. Please update any scripts to point to main instead
of master.

At the point of the changeover, we will move all patches in gerrit to
the main branch and disable pushes to the master branch.

After the switch, we will sync the main branch to the master branch for
a while to give people a little more time to update any scripts that
are currently pointed at the master branch. Note that this update will
probably be done just once per day, and the frequency of updates will
be decreased over time. We plan to stop updating the master branch
following the 4.22 release.

Significant or interesting changes
----------------------------------

### lib: Support localized text of memory_training_desc in ux_locales.c

Most of the text in coreboot is for logging, and does not use
localization. There are however, some bits of text that can be
presented to the user, and this patch supplies a method to localize
them.

To support the localized text, we need to get the locale id by vboot
APIs and read raw string content file: preram_locales located at either
RO or RW.

The preram_locales file follows the format:

    [PRERAM_LOCALES_VERSION_BYTE (\x01)]
    [string_name_1] [\x00]
    [locale_id_1] [\x00] [localized_string_1] [\x00]
    [locale_id_2] [\x00] [localized_string_2] …
    [\x01]
    [string_name_2] [\x00] ...

This code will search for the correct localized string that its string
name is `memory_training_desc` and its locale ID matches the ID vb2api
returns. If no valid string found, we will try to display in English
(locale ID 0).


### Improved the bootsplash support

The JPEG decoder, that was added many years ago to display a bootsplash
in coreboot, has a few quirks. People used to do some voodoo with GIMP
to convert images to the right format, but we can also achieve the same
with ImageMagick's `convert`. The currently known constraints are:

* The framebuffer's color format is ignored,
* only YCC 4:2:0 color sampling is supported, and
* width and height have to be a multiple of 16 pixels.

Beside that, we can only display the bootsplash if it completely fits
into the framebuffer. As the latter's size is often decided at runtime,
we can't do much more than offering an option to set a specific size.

The build system has been extended so that the necessary adjustments to
the picture can be done by it and several options have been added to
Kconfig.


### libpayload/uhci: Re-write UHCI RH driver w/ generic_hub API

This is a complete rewrite of the UHCI root-hub driver, based on the
xHCI one. We are doing things by the book as far as possible. One
special case is uhci_rh_reset_port() which does the reset sequencing
that usually the hardware would do.

This abandons some quirks of the old driver:
* Ports are not disabled/re-enabled for every attachment anymore.
* We solely rely on the Connect Status Change bit to track changes.
* Further status changes are now deferred to the next polling round.


### linux_trampoline: Handle coreboot framebuffer & 64-bit addresses

Translate the coreboot framebuffer info from coreboot tables to the
Linux zero page.

To support full 64-bit addresses, there is a new field `ext_lfb_base`
since Linux 4.1. It is unclear, however, how a loader is supposed to
know if the kernel is compatible with this. Filling these previously
reserved bits doesn't hurt, but an old kernel would probably ignore
them and not know that it's handling a clipped, invalid address. So we
play safe, and only allow 64-bit addresses for kernels after the 2.15
version bump of the boot protocol.


### arch/x86: Don't allow hw floating point operations

Even though coreboot does not allow floating point operations, some
compilers like clang generate code using hw floating point registers,
e.g. SSE %XMMx registers on 64bit code by default. Floating point
operations need to be enabled in hardware for this to work (CR4). Also
in SMM we explicitly need to save and restore floating point registers
for this reason. If we instruct the compiler to not generate code with
FPU ops, this simplifies our code as we can skip that step.

With clang this reduces the binary size a bit. For instance ramstage
for emulation/qemu-q35 drops by 4 kB from from 216600 bytes
decompressed to 212768 bytes.

Since we now explicitly compile both ramstage and smihandler code
without floating point operations and associated registers we don't
need to save/restore floating point registers in SMM.

The EFER MSR is in the SMM save state and RSM properly restores it.
Returning to 32bit mode was only done so that fxsave was done in the
same mode as fxrstor, but this is no longer done.


### Caching of PCIe 5.0 HSPHY firmware in SPI flash

This adds the ability to cache the PCIe 5.0 HSPHY firmware in the SPI
flash. A new flashmap region is created for that purpose. The goal of
caching is to reduce the dependency on the CSME (Converged Security and
Management Engine) and the HECI (Host Embedded Controller Interface) IP
LOAD command which may fail when the CSME is disabled, e.g. soft
disabled by HECI command or HAP (High Assurance Platform mode). By
caching that firmware, this allows the PCIe 5.0 root ports to keep
functioning even if CSME/HECI is not functional.


### Extracting of TPM logs using cbmem tool

CBMEM can contain logs in different forms (at most one is present):
 * coreboot-specific format (CBMEM_ID_TPM_CB_LOG exported as
   LB_TAG_TPM_CB_LOG)
 * TPM1.2 format (CBMEM_ID_TCPA_TCG_LOG)
 * TPM2 format (CBMEM_ID_TPM2_TCG_LOG)

The last two follow specifications by Trusted Computing Group, but
until now cbmem couldn't print them.

These changes make the cbmem utility check for existence of TPM1.2/TPM2
logs in CBMEM and add code necessary for parsing and printing of their
entries.

`cbmem -L` for CONFIG_TPM1=y case

```
TCPA log:
	Specification: 1.21
	Platform class: PC Client
TCPA log entry 1:
	PCR: 2
	Event type: Action
	Digest: 5622416ea417186aa1ac32b32c527ac09009fb5e
	Event data: FMAP: FMAP
```

`cbmem -L` for CONFIG_TPM2=y case

```
TPM2 log:
	Specification: 2.00
	Platform class: PC Client
TPM2 log entry 1:
	PCR: 2
	Event type: Action
	Digests:
		 SHA256: 68d27f08cb261463a6d004524333ac5db1a3c2166721785a6061327b6538657c
	Event data: FMAP: FMAP
```


### soc/amd: read domain resource window configuration from hardware

Read the MMIO and IO decode windows for the PCI root complex and the
PCI bus number range decoded to the PCI root complex from the data
fabric registers and pass the information to the resource allocator so
it has the correct constraints to do its job. Also generate the
corresponding ACPI resource producers in the SSDT so that the OS knows
about this too. This is required for the upcoming USB 4 support.


Additional coreboot changes
---------------------------

* Added SPDX headers to more files to help automated license checking.
  The linter has been enabled to check the Makefiles as well.
* Cleaned up Kconfig files and source code.
* Enabled acpigen to generate tables for SPCR (Serial Port Console
  Redirection) and GTDT (Generic Timer Description Table).
* The resource allocation above the 4GiB boundary has been improved.
* Most of the code has been adjusted to make use of C99 flexible arrays
  instead of one-element or zero-length arrays.
* Additional Dockerfiles based on Arch and Alpine Linux have been added
  to build-test with alternate build environments, including musl-libc.
  They are very basic at the moment and not equal to the coreboot-sdk.
  They will be extended in the future.
* Added support for ITE IT8784E to superiotool.
* Added support for Intel 700 chipset series to inteltool and a build
  issue with musl–libc has been fixed.
* Added support for Intel 800 chipset series to ifdtool.
* The coreboot-sdk container has been extended so that it allows
  extracting the MRC binary from Haswell-based ChromeOS firmware images.
* From now on POST code preprocessor macros should have a POSTCODE
  prefix following the name of the POST code.
* The NASM compiler provided by the coreboot toolchain wasn’t properly
  integrated into xcompile and thus it wasn’t used by the build system.
  Instead, it was required to install NASM on the host in order to use
  it. This has been fixed.
* The time measurement done in abuild got improved and also an issue
  has been fixed when the variant name contains hyphens.
* The RISC-V code was enabled to build with Clang.
* Initial work has been done to transform Camelcase options to
  Snakecase.
* The buildgcc script is now able to just fetch the tarballs if desired,
  which is needed for reproducible build environments for example.



Changes to external resources
-----------------------------

### Toolchain

* binutils
  * Added binutils-2.40_stop_losing_entry_point_when_LTO_enabled.patch
* Upgrade IASL from 20221020 to 20230628
* Upgrade LLVM from 15.0.7 to 16.0.6
* Upgrade NASM from 2.15.05 to 2.16.01
  * Added nasm-2.16.01_handle_warning_files_while_building_in_a_directory.patch
* Upgrade CMake from 3.26.3 to 3.26.4
* Upgrade GCC from 11.3.0 to 11.4.0
  * Added gcc-11.4.0_rv32iafc.patch


### Git submodule pointers

#### /3rdparty
* amd_blobs: Update from commit id 1cd6ea5cc5 to 6a1e1457af (5 commits)
* arm-trusted-firmware: Update from commit id 4c985e8674 to 37366af8d4
(851 commits)
* blobs: Update from commit id 01ba15667f to a8db7dfe82 (14 commits)
* fsp: Update from commit id 6f2f17f3d3 to 3beceb01f9 (24 commits)
* intel-microcode: Update from commit id 2be47edc99 to 6f36ebde45 (5
commits)
* libgfxinit: Update from commit id 066e52eeaa to a4be8a21b0 (18
commits)
* libhwbase: Update from commit id 8be5a82b85 to 584629b9f4 (2 commits)
* qc_blobs: Update from commit id 33cc4f2fd8 to a252198ec6 (4 commits)
* vboot: Update from commit id 35f50c3154 to 0c11187c75 (83 commits)

#### /util
* goswid: Update from commit id bdd55e4202 to 567a1c99b0 (5 commits)
* nvidia/cbootimage: Update from commit id 65a6d94dd5 to 80c499ebbe (1
commit)


### External payloads

* Update the depthcharge payload from commit ID 902681db13 to c48613a71c
* Upgrade EDK2-MrChromebox from version 202304 to version 202306
* Upgrade SeaBIOS from version 1.16.1 to version 1.16.2
* Update tint from version 0.05 to version 0.07
* Update U-Boot from version 2021.07 to version v2023.07


Added mainboards:
-----------------
* ByteDance ByteDance bd_egs
* Google: Craaskov
* Google: Expresso
* Google: Karis
* Google: Karis4ES
* Google: Pirrha
* Google: Ponyta
* Google: Screebo4ES
* Google: Ovis
* Google: Ovis4ES
* Google: Rex EC ISH
* Google: Rex4ES
* HP Compaq Elite 8300 USDT
* HP EliteBook 820 G2
* IBM SBP1
* Intel Raptorlake silicon with Alderlake-P RVP
* Inventec Transformers
* MSI PRO Z790-P (WIFI)
* MSI PRO Z790-P (WIFI) DDR4
* Star Labs Star Labs StarBook Mk VI (i3-1315U and i7-1360P)
* System76 addw3
* System76 bonw15
* System76 darp9
* System76 galp7
* System76 gaze17 3050
* System76 gaze17 3060-b
* System76 gaze18
* System76 lemp12
* System76 oryp11
* System76 serw13


Removed Mainboards
------------------

* Intel Galileo


Updated SoCs
------------

* Removed src/soc/intel/quark


Statistics from the 4.20 to the 4.21 release
--------------------------------------------

* Total Commits: 1252
* Average Commits per day: 12.59
* Total lines added: 317734
* Average lines added per commit: 253.78
* Number of patches adding more than 100 lines: 86
* Average lines added per small commit: 36.22
* Total lines removed: 261063
* Average lines removed per commit: 208.52
* Total difference between added and removed: 56671

* Total authors: 143
* New authors: 21


Significant Known and Open Issues
---------------------------------


Issues from the coreboot bugtracker: https://ticket.coreboot.org/
```{eval-rst}
+-----+-----------------------------------------------------------------+
| #   | Subject                                                         |
+=====+=================================================================+
| 499 | edk2 boot fails with RESOURCE_ALLOCATION_TOP_DOWN enabled       |
+-----+-----------------------------------------------------------------+
| 495 | Stoney chromebooks not booting PSPSecureOS                      |
+-----+-----------------------------------------------------------------+
| 478 | X200 booting Linux takes a long time with TSC                   |
+-----+-----------------------------------------------------------------+
| 474 | X200s crashes after graphic init with 8GB RAM                   |
+-----+-----------------------------------------------------------------+
| 457 | Haswell (t440p): CAR mem region conflicts with CBFS_SIZE > 8mb  |
+-----+-----------------------------------------------------------------+
| 453 | Intel HDMI / DP Audio device not showing up after libgfxinit    |
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
| 414 | X9SAE-V: No USB keyboard init on SeaBIOS using Radeon RX 6800XT |
+-----+-----------------------------------------------------------------+
| 412 | x230 reboots on suspend                                         |
+-----+-----------------------------------------------------------------+
| 393 | T500 restarts rather than waking up from suspend                |
+-----+-----------------------------------------------------------------+
| 350 | I225 PCIe device not detected on Harcuvar                       |
+-----+-----------------------------------------------------------------+
| 327 | OperationRegion (OPRG, SystemMemory, ASLS, 0x2000) causes BSOD  |
+-----+-----------------------------------------------------------------+
```


coreboot Links and Contact Information
--------------------------------------

* Main Web site: https://www.coreboot.org
* IRC: https://web.libera.chat/#coreboot
* Downloads: https://coreboot.org/downloads.html
* Source control: https://review.coreboot.org
* Documentation: https://doc.coreboot.org
* Issue tracker: https://ticket.coreboot.org/projects/coreboot
* Donations: https://coreboot.org/donate.html
