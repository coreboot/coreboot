coreboot 4.22 & 4.22.01 releases
========================================================================

The next release is planned for the 19th of February, 2024

These notes cover the latest updates and improvements to coreboot over
the past three months. A big thank you to the returning contributors as
well as the 14 individuals who committed code for the first time. We
greatly appreciate everyone's dedication and expertise. As with past
releases, this one reflects a commitment to open source innovation,
security enhancements, and expanding hardware support.


## 4.22.01 release

The week between tagging a release and announcing it publicly is used
to test the tagged version and make sure everything is working as we
expect. This is done instead of freezing the tree and doing release
candidates before the release.

For the 4.22 release cycle we found an uninitialized variable error on
the sandybridge/ivybridge platforms and rolled that into the 4.22.01
release package.

## coreboot version naming update

This release is the last release to use the incrementing 4.xx release
name scheme. For future releases, coreboot is switching to a
Year.Month.Sub-version naming scheme. As such, the next release,
scheduled for February of 2024 will be numbered 24.02, with the
sub-version of 00 implied. If we need to do a fix or future release of
the 24.02 release, we'll append the values .01, .02 and so on to the
initial release value.


## coreboot default branch update

Immediately after the 4.21 release, the coreboot project changed the
default git branch from 'master' to 'main'. For the first couple of
months after the change, The master branch was synced with the main
branch several times a day, allowing people time to update any scripts.
As of 2023-11-01, the sync rate has slowed to once a week. This will
continue until the next release, at which time the master branch will
be removed.



Significant or interesting changes
----------------------------------

### x86: support .data section for pre-memory stages

x86 pre-memory stages did not support the `.data` section and as a
result developers were required to include runtime initialization code
instead of relying on C global variable definitions.

Other platforms do not have that limitation. Hence, resolving it helps
to align code and reduce compilation-based restrictions (cf. the use of
`ENV_HAS_DATA_SECTION` compilation flag in various places of coreboot
code).

There were three types of binary to consider:
1. eXecute-In-Place pre-memory stages
2. `bootblock` stage is a bit different as it uses Cache-As-Ram but
   the memory mapping and its entry code different
3. pre-memory stages loaded in and executed from Cache-As-RAM
   (cf. `CONFIG_NO_XIP_EARLY_STAGES`).

eXecute-In-Place pre-memory stages (#1) rely on a new ELF segment as
the code segment Virtual Memory Address and Load Memory Address are
identical but the data needs to be linked in cache-As-RAM (VMA) to be
stored right after the code (LMA).

`bootblock` (#2) also uses this new segment to store the data right
after the code and it loads it to Cache-As-RAM at runtime. However, the
code involved is different.

Not eXecute-In-Place pre-memory stages (#3) did not need any special
work other than enabling a .data section as the code and data VMA / LMA
translation vector is the same.

Related important commits:
- c9cae530e5 ("cbfstool: Make add-stage support multiple ignore sections")
- 79f2e1fc8b ("cbfstool: Make add-stage support multiple loadable segments")
- b7832de026 ("x86: Add .data section support for pre-memory stages")


### x86: Support CBFS cache for pre-memory stages and ramstage

The CBFS cache scratchpad offers a generic way to decompress CBFS files
through the cbfs_map() function without having to reserve a per-file
specific memory region.

CBFS cache x86 support has been added to pre-memory stages and
ramstage.

1. **pre-memory stages**: The new `PRERAM_CBFS_CACHE_SIZE` Kconfig can
   be used to set the pre-memory stages CBFS cache size. A cache size
   of zero disables the CBFS cache feature for all pre-memory stages.
   The default value is 16 KiB which seems a reasonable minimal value
   enough to satisfy basic needs such as the decompression of a small
   configuration file. This setting can be adjusted depending on the
   platform's needs and capabilities.

   Note that we have set this size to zero for all the platforms
   without enough space in Cache-As-RAM to accommodate the default
   size.

2. **ramstage**: The new `RAMSTAGE_CBFS_CACHE_SIZE` Kconfig can be used
   to set the ramstage CBFS cache size. A cache size of zero disables
   the CBFS cache feature for ramstage. Similarly to pre-memory stages
   support, the default size is 16 KiB.

   As we want to support the S3 suspend/resume use case, the CBFS cache
   memory cannot be released to the operating system and therefore
   cannot be an unreserved memory region. The ramstage CBFS cache
   scratchpad is defined as a simple C static buffer as it allows us to
   keep the simple and robust design of the static initialization of
   the `cbfs_cache` global variable (cf. src/lib/cbfs.c).

   However, since some AMD SoCs (cf. `SOC_AMD_COMMON_BLOCK_NONCAR`
   Kconfig) already define a `_cbfs_cache` region we also introduced a
   `POSTRAM_CBFS_CACHE_IN_BSS` Kconfig to gate the use of a static
   buffer as the CBFS cache scratchpad.


### Allow romstage to be combined into the bootblock

Having a separate romstage is only desirable:
 - with advanced setups like vboot or normal/fallback
 - boot medium is slow at startup (some ARM SOCs)
 - bootblock is limited in size (Intel APL 32K)

When this is not the case there is no need for the extra complexity
that romstage brings. Including the romstage sources inside the
bootblock substantially reduces the total code footprint. Often the
resulting code is 10-20k smaller.

This is controlled via a Kconfig option.


### soc/intel/cmn/gfx: Add API to report presence of external display

This implements an API to report the presence of an external display on
Intel silicon. The API uses information from the transcoder and
framebuffer to determine if an external display is connected.

For example, if the transcoder is attached to any DDI ports other than
DDI-A (eDP), and the framebuffer is initialized, then it is likely that
an external display is present.

This information can be used by payloads to determine whether or not to
power on the display, even if eDP is not initialized.


### device/pci_rom: Set VBIOS checksum when filling VFCT table

AMD's Windows display drivers validate the checksum of the VBIOS data
in the VFCT table (which gets modified by the FSP GOP driver), so
ensure it is set correctly after copying the VBIOS into the table if
the FSP GOP driver was run. Without the correct checksum, the Windows
GPU drivers will fail to load with a code 43 error in Device Manager.



Additional coreboot changes
---------------------------

* Move all 'select' statements from Kconfig.name files to Kconfig
* acpigen now generates variable-length PkgLength fields instead of a
  fixed 3-byte size to improve compatibility and to bring it in line
  with IASL
* Work to allow Windows to run on more Chromebooks
* General cleanup and reformatting
* Add initial AMD openSIL implementation
* Add ACPI table generation for ARM64
* Stop resetting CMOS during s3 resume even if marked as invalid
* Comply with ACPI specification by making _STR Unicode strings
* Fix SMM get_save_state calculation, which was broken when STM was
  enabled
* SNB+MRC boards: Migrate MRC settings to devicetree
* Work on chipset devicetrees for all platforms



Changes to external resources
-----------------------------

### Toolchain updates

* Upgrade GMP from 6.2.1 to 6.3.0
* Upgrade binutils from 2.40 to 2.41
* Upgrade MPFR from 4.2.0 to 4.2.1


### Git submodule pointers

* amd_blobs: Update from commit id 6a1e1457af to e4519efca7 (16
  commits)
* arm-trusted-firmware: Update from commit id 37366af8d4 to 88b2d81345
  (214 commits)
* fsp: Update from commit id 3beceb01f9 to 481ea7cf0b (15 commits)
* intel-microcode: Update from commit id 6f36ebde45 to 6788bb07eb (1
  commit)
* vboot: Update from commit id 0c11187c75 to 24cb127a5e (24 commits)
* genoa_poc/opensil: New submodule updated to 0411c75e17 (41 commits)


### External payloads

* U-Boot: Use github mirror and the latest version
* edk2: Update default branch for MrChromebox repo to 2023-09



Platform Updates
----------------

### Added 17 mainboards

* AMD Onyx
* Google: Anraggar
* Google: Brox
* Google: Chinchou
* Google: Ciri
* Google: Deku
* Google: Deku4ES
* Google: Dexi
* Google: Dochi
* Google: Nokris
* Google: Quandiso
* Google: Rex4ES EC ISH
* Intel: Meteorlake-P RVP with Chrome EC for non-Prod Silicon
* Purism Librem 11
* Purism Librem L1UM v2
* Siemens FA EHL
* Supermicro X11SSW-F


### Added 1 SoC

* src/soc/amd/genoa



Statistics from the 4.21 to the 4.22 release
--------------------------------------------

* Total Commits: 977
* Average Commits per day: 10.98
* Total lines added: 62993
* Average lines added per commit: 64.48
* Number of patches adding more than 100 lines: 60
* Average lines added per small commit: 37.55
* Total lines removed: 30042
* Average lines removed per commit: 30.75
* Total difference between added and removed: 32951
* Total authors: 135
* New authors: 14



Significant Known and Open Issues
---------------------------------

Issues from the coreboot bugtracker: <https://ticket.coreboot.org/>

### Payload-specific issues

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


### Platform-specific issues

```{eval-rst}
+-----+-----------------------------------------------------------------+
| #   | Subject                                                         |
+=====+=================================================================+
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



Plans for the next release
--------------------------

* Finish adding chipset device trees for all SOCs
* Improve code for options/setup
* Start reformatting C files with clang-format
* Add warning/error step for Makefiles at the end



coreboot Links and Contact Information
--------------------------------------

* Main Website: <https://www.coreboot.org>
* Downloads: <https://coreboot.org/downloads.html>
* Source control: <https://review.coreboot.org>
* Documentation: <https://doc.coreboot.org>
* Issue tracker: <https://ticket.coreboot.org/projects/coreboot>
* Donations: <https://coreboot.org/donate.html>
