coreboot 4.7 release notes
==========================

The 4.7 release covers commit 0a4a4f7ae4 to commit fd470f7163
Since the last release in April 2017, the coreboot project had 2573 commits by 150 authors.

There is a pgp signed 4.7 tag in the git repository, and a branch will be created as needed.


New chipsets
------------

* AMD Stoney Ridge
* Intel i82801jx Southbridge (ICH10)
* Intel Denverton and Denverton-NS
* Work has started on Intel Cannon Lake

Added 47 mainboards & variants:
-------------------

* Acer Chromebook 14 CB3-431 [google/edgar]           Intel Braswell
* Acer Chromebook 15 CB3-532 [google/banon]           Intel Braswell
* Acer Chromebook N7 C731 [google/relm]               Intel Braswell
* ASRock B75 Pro3-M                                   Intel Ivy Bridge
* ASRock G41C-GS R2.0                                 Intel G41/ICH7
* Asus AM1I-A                                         AMD Kabini
* Asus Chromebook C202SA/C300SA/C301SA (google/terra) Intel Braswell
* Biostar A68N-5200                                   AMD Kabini
* Compulab Intense-PC                                 Intel Ivy Bridge
* Dell Chromebook 11 3180/3189 (google/kefka)         Intel Braswell
* Foxconn G41S-K                                      Intel G41/ICH7
* Google Coral                                        Intel Apollo Lake
* Google Grunt                                        AMD Stoney Ridge
* Google Kahlee                                       AMD Stoney Ridge
* Google Meowth                                       Intel Cannon Lake
* Google Nami                                         Intel Kaby Lake
* Google Nautilus                                     Intel Kaby Lake
* Google Nefario                                      Rockchip RK3399
* Google Rainier                                      Rockchip RK3399
* Google Soraka                                       Intel Kaby Lake
* Google Zoombini                                     Intel Cannon Lake
* HP Chromebook 11 G5 (google/setzer)                 Intel Braswell
* HP EliteBook 2570p                                  Intel Ivy Bridge
* HP EliteBook 2760p                                  Intel Sandy Bridge
* HP EliteBook 8460p                                  Intel Sandy Bridge
* HP EliteBook 8470p                                  Intel Ivy Bridge
* HP EliteBook Revolve 810 G1                         Intel Ivy Bridge
* Intel Cannnlake RVPU                                Intel Cannon Lake
* Intel Cannonlake RVPY                               Intel Cannon Lake
* Intel D410PT                                        Intel Atom D410
* Intel DG43GT                                        Intel G43/ICH10
* Intel GLKRVP                                        Intel Gemini Lake
* Intel Harcuvar                                      Intel Denverton
* Intel NUC DCP847SKE                                 Intel Sandy Bridge
* Intel Saddle Brook reference board                  Intel Skylake
* Lenovo N22/N42 Chromebook (google/reks)             Intel Braswell
* Lenovo T430                                         Intel Ivy Bridge
* Lenovo Thinkpad 11e/Yoga Chromebook G3
  (google/ultima)                                     Intel Braswell
* Lenovo ThinkPad X131e                               Intel Sandy Bridge
* Lenovo Z61T                                         Intel i945/ICH7
* PC Engines APU3                                     AMD Steppe Eagle
* PC Engines APU4                                     AMD Steppe Eagle
* PC Engines APU5                                     AMD Steppe Eagle
* Purism Librem 13 v2                                 Intel Skylake
* Purism Librem 15 v3                                 Intel Skylake
* Samsung Chromebook 3 (google/celes)                 Intel Braswell
* White label Chromebook (google/wizpig)              Intel Braswell
* WinNET G170                                         VIA CN700

Removed 2 mainboards
--------------

* Biferos Bifferboard
* Google Cosmos

New Embedded Controller
-----------------------

* KBC1126 used in HP EliteBooks

General changes
---------------

* Integrate me_cleaner
* Add flashconsole implementation
* Build edk2 UEFI payload from upstream source
* Remove CMOS NVRAM configurable baud rates
* A common mrc_cache driver to store romstage settings in SPI flash

Google ChromeOS devices:
------------------------

* Add ACPI USB port definitions for many boards
* Fix preprocessor guards for LPC TPM
* Remove non-existent IRQ for LPC TPM
* Fix LED control for mccloud
* Enable keyboard backlight at boot on equipped boards
* Fix ACPI data for non-google EC's to improve Windows compatibility
* Add missing SPD files for chell, fixing support for > 4GB boards

Lenovo Thinkpads:
-----------------

* Add support for passive cooling
* Add ACPI fan control
* Add BDC detection and power saving
* Unify hybrid graphics and improved power saving

Intel Braswell:
---------------

* Add support for all outstanding Braswell ChromeOS devices
* Update FSP 1.1 header to v1.1.7.0
* Adjust FSP header revision check to be less stringent
* Upstream numerous commits from Chromium tree
* Fix ACPI scope for I2C devices
* Fix SPI write after flash lockdown set

Legacy Intel Boards:
--------------------

* Unify Intel VBT handling
* Add support for loading external VBT
* Provide the VBT through Intel OpRegion method on all platforms
* Fix low memory corruption on S3 resume path

Intel Sandy Bridge:
------------------

* Add a Kconfig option to ignore XMP max DIMMs
* Add Kconfig option for max. DRAM frequency fuses
* Advertise correct DRAM frequency on Ivy Bridge
* Improve CAS/frequency selection
* Use command rate 2T on channels with two DIMMs installed for improved
stability

Intel X4X:
----------

* Fix booting with FSB800 DDR667 combination
* Rework ram DQS receiver enable training sequence
* Rework and fix SPD reading and decoding
* Allow external GPU to take VGA cycles

Intel GM45:
-----------

* Improve compatibility with mixed DIMMs
* Add romstage timings
* Set the display backlight PWM correctly

Intel Pineview:
---------------

* Enable remapping of memory to allow for 4G or more memory

Intel I440BX
------------

* Implement early CBMEM support
* Fix RAM init programming

AMD AGESA
---------

* Move boards to early CBMEM and add timestamps
* Refactor boards away from using agesawrapper
* Wipe unused sources under vendorcode
* Re-enable ACPI S3 after fixing low memory corruptions

AMD binaryPI
------------

* Move boards to early CBMEM
* Continue work on cleaning up headers

libgfxinit
----------

* Support new hardware: Broxton/APL (DP and HDMI only), Skylake
* Handle framebuffer mapping in the library
* Make DP training more compatible and tolerant
* Enhance compatibility for VGA adaptors

intelmetool
-----------

* Add support for Sunrise Point LP
* Add Intel Boot Guard detection

Toolchain
---------

* buildgcc now verifies downloaded files against hashes
* Improve GNAT detection
* Update binutils to 2.29.1
