coreboot 4.14
=============

coreboot 4.14 was released on May 10th, 2021.

Since 4.13 there have been 3660 new commits by 215 developers.
Of these, about 50 contributed to coreboot for the first time.
Welcome to the project!

These changes have been all over the place, so that there's no
particular area to focus on when describing this release: We had
improvements to mainboards, to chipsets (including much welcomed
work to open source implementations of what has been blobs before),
to the overall architecture.

Thank you to all developers who made coreboot the great open source
firmware project that it is, and made our code better than ever.

New mainboards
--------------

* AMD Bilby
* AMD Majolica
* GIGABYTE GA-D510UD
* Google Blipper
* Google Brya
* Google Cherry
* Google Collis
* Google Copano
* Google Cozmo
* Google Cret
* Google Drobit
* Google Galtic
* Google Gumboz
* Google Guybrush
* Google Herobrine
* Google Homestar
* Google Katsu
* Google Kracko
* Google Lalala
* Google Makomo
* Google Mancomb
* Google Marzipan
* Google Pirika
* Google Sasuke
* Google Sasukette
* Google Spherion
* Google Storo
* Google Volet
* HP 280 G2
* Intel Alderlake-M RVP
* Intel Alderlake-M RVP with Chrome EC
* Intel Elkhartlake LPDDR4x CRB
* Intel shadowmountain
* Kontron COMe-mAL10
* MSI H81M-P33 (MS-7817 v1.2)
* Pine64 ROCKPro64
* Purism Librem 14
* System76 darp5
* System76 galp3-c
* System76 gaze15
* System76 oryp5
* System76 oryp6

Removed mainboards
------------------

* Google Boldar
* Intel Cannonlake U LPDDR4 RVP
* Intel Cannonlake Y LPDDR4 RVP

Deprecations and incompatible changes
-------------------------------------

### SAR support in VPD for Chrome OS

SAR support in VPD has been deprecated for Chrome OS platforms for > 1
year now. All new Chrome OS platforms have switched to using SAR
tables from CBFS. For the next release, coreboot is updated to align
with the Chrome OS factory changes and hence SAR support in VPD is
deprecated in [CB:51483](https://review.coreboot.org/51483). Starting
with this release, anyone building coreboot for an already released
Chrome OS platform with SAR table in VPD will have to extract the
"wifi_sar" key from VPD and add it as a file to CBFS using following
steps:
 * On DUT, read SAR value using `vpd -i RO_VPD -g wifi_sar`
 * In coreboot repo, generate CBFS SAR file using:
    `echo ${SAR_STRING} > site-local/${BOARD}-sar.hex`
 * Add to site-local/Kconfig:
    ```
     config WIFI_SAR_CBFS_FILEPATH
       string
       default "site-local/${BOARD}-sar.hex"
    ```

### CBFS stage file format change

[CB:46484](https://review.coreboot.org/46484) changed the in-flash
file format of coreboot stages to prepare for per-file signature
verification. As described in the commit message in more details,
when manipulating stages in a CBFS, the cbfstool build must match the
coreboot image so that they're using the same format: coreboot.rom
and cbfstool must be built from coreboot sources that either both
contain this change or both do not contain this change.

Since stages are usually only handled by the coreboot build system
which builds its own cbfstool (and therefore it always matches
coreboot.rom) this shouldn't be a concern in the vast majority of
scenarios.

Significant changes
-------------------

### AMD SoC cleanup and initial Cezanne APU support

There's initial support for the AMD Cezanne APUs in the tree. This code
hasn't started as a copy of the previous generation, but was based on a
slightly modified version of the example/min86 SoC. During the cleanup
of the existing Picasso SoC code the common parts of the code were
moved to the common AMD SoC code, so that they could be used by the
Cezanne code instead of adding another slightly different copy.

### X86 bootblock layout

The static size C_ENV_BOOTBLOCK_SIZE was mostly dropped in favor of
dynamically allocating the stage size; the Kconfig is still available
to use as a fixed size and to enforce a maximum for selected chipsets.
Linker sections are now top-aligned for a reduced flash footprint and to
maintain the requirements of near jump from reset vector.

### ACPI GNVS framework

SMI handlers for APM_CNT_GNVS_UDPATE were dropped; GNVS pointer to SMM is
now passed from within SMM_MODULE_LOADER. Allocation and initialisations
for common ACPI GNVS table entries were largely moved to one centralized
implementation.

### Intel Xeon Scalable Processor support is now considered mature

Intel Xeon Scalable Processor (Xeon-SP) family [1] is designed
primarily to serve the needs of the server market.

coreboot support for Xeon-SP is in src/soc/intel/xeon_sp directory.
This release has support for SkyLake-SP (SKX-SP) which is the 2nd
generation, and for Cooper Lake-SP (CPX-SP) which is the 3rd generation
or the latest generation [2] on market.

With this release, the codebase for multiple generations of Xeon-SP
were unified and optimized:
* SKX-SP SoC code is used in [OCP TiogaPass mainboard][3]. Support for
this board is in Proof Of Concept Status.
* CPX-SP SoC code is used in OCP DeltaLake mainboard. Support for
this board is in DVT (Design Validation Test) exit equivalent status.
Features supported, (performance/stability) test scopes, known issues,
features gaps are described in [4].


[1]: <https://www.intel.com/content/www/us/en/products/details/processors/xeon/scalable.html>
[2]: <https://www.intel.com/content/www/us/en/products/docs/processors/xeon/3rd-gen-xeon-scalable-processors-brief.html>
[3]: <../mainboard/ocp/tiogapass.md>
[4]: <../mainboard/ocp/deltalake.md>
