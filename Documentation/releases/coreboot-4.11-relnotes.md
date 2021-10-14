coreboot 4.11
=============

coreboot 4.11 was released on November 19th.

This release cycle was a bit shorter to get closer to our regular
schedule of releasing in spring and autumn.

Since 4.10 there were 1630 new commits by over 130 developers.
Of these, about 30 contributed to coreboot for the first time.

Thank you to all contributors who made 4.11 what it is and welcome
to the project to all new contributors!

Clean Up
--------

The past few months saw lots of cleanup across the source tree:

The included headers in source files were stripped down to avoid reading
unused headers, and unused code fragments, duplicate preprocessor symbols
and configuration options were eliminated. Even ACPI got its share
of attention, making our tables and bytecode more standards compliant
than ever.

The code across Intel's chipsets was unified some more into drivers for
common function blocks, an effort we're more confident will succeed now
that Intel itself is driving it.

Chipset work
------------

Most activity in the last couple months was on Intel support,
specifically the Kaby Lake and Cannon Lake drivers were extended
for the generations following them.

On ARM, the Mediatek 8173 chipset support saw significant work while
the AMD side worked on getting Picasso support in.

But everything else also saw some action, the relatively old
(e.g. Intel GM45, Via VX900), the tiny (RISC-V) and the obscure
(Quark).

Verified Boot
-------------

The vboot feature that Chromebooks brought into coreboot was extended
to work on devices that weren't specially adapted for it: In addition
to its original device family it's now supported on various Lenovo
laptops, Open Compute Project systems and Siemens industrial machines.

Eltan's support for measured boot continues to be integrated with
vboot, sharing data structures and generally working together where
possible.

New devices
-----------

With 4.11 there's the beginning of support for Intel Tiger Lake and
Qualcomm's SC7180 SoCs, while we removed the unmaintained support
for Allwinner's A10 SoC.

There are also 25 new mainboards in our tree:

* AMD PADMELON
* ASUS P5QL-EM
* EMULATION QEMU-AARCH64
* GOOGLE AKEMI
* GOOGLE ARCADA CML
* GOOGLE DAMU
* GOOGLE DOOD
* GOOGLE DRALLION
* GOOGLE DRATINI
* GOOGLE JACUZZI
* GOOGLE JUNIPER
* GOOGLE KAKADU
* GOOGLE KAPPA
* GOOGLE PUFF
* GOOGLE SARIEN CML
* GOOGLE TREEYA
* GOOGLE TROGDOR
* LENOVO R60
* LENOVO T410
* LENOVO THINKPAD T440P
* LENOVO X301
* RAZER BLADE-STEALTH KBL
* SIEMENS MC-APL6
* SUPERMICRO X11SSH-TF
* SUPERMICRO X11SSM-F

In addition to the Cubieboard (which uses the A10 SoC), we also
removed Google Hatch WHL.

Deprecations
------------

Because there was only a single developer board (AMD Torpedo)
using AGESA family 12h, and because there were multiple,
unique Coverity issues with it, the associated vendorcode will
be removed shortly after this release.

Support for the MIPS architecture will also be removed shortly after
this release as the only board in the tree was a discontinued development
board and no other work has picked up MIPS support, so it's very likely
broken already.

After more than a year of planning and following the announcement in
coreboot 4.10, platforms not using relocatable ramstage, a C bootblock
and, on systems using Cache as RAM, a postcar stage, won't be supported
going forward.

Significant changes
-------------------

### `__PRE_RAM__` is deprecated

Preprocessor use of `defined(__PRE_RAM__)` have been mostly replaced with
`if (ENV_ROMSTAGE_OR_BEFORE)` or the inverse `if (ENV_RAMSTAGE)`.

The remaining cases and `-D__PRE_RAM__` are to be removed soon after release.

### `__BOOTBLOCK__` et.al. are converted

This applies to all `ENV_xxx` definitions found in `<rules.h>`.

Write code without preprocessor directives whenever possible, replacing
`#ifdef __BOOTBLOCK__` with  `if (ENV_BOOTBLOCK)`

In cases where preprocessor is needed use `#if ENV_BOOTBLOCK` instead.

### `CAR_GLOBAL` is removed where possible

For all platform code with `NO_CAR_GLOBAL_MIGRATION=y`, any `CAR_GLOBAL`
attributes have been removed. Remaining cases from common code are to be
removed soon after release.

### `TSEG` and  `cbmem_top()` mapping

Significant refactoring has been done to achieve some consistency across platforms
and to reduce code duplication.

### Build system amenities ###

The build system now has an `all` class of source files to remove the need to
list source files for each and every source class (romstage, ramstage, ...)

The site-local/ mechanism became more robust.

### Stricter coding standards to improve security ###

The build now fails on variable length arrays (that make it way too easy to
smash a stack) and case statements falling through without a note that it is
intentional.

### Shorter file headers ###

This project is still under way, but we started moving author information
from individual files into the global AUTHORS file (and there's the git
history for more details).

In the future, we also want to replace the license headers (lots of lines)
in each file with spdx identifiers (one line) and so we added a LICENSES/
directory that contains the full text of all the licenses that are used
throughout our tree.

### Variant creation scripts ###

To ease the creation of variant boards, `util/mainboard/` now contains
scripts to generate a new variant to a given board. These are still
specific to google/hatch at this time, but they're written with the idea
of becoming more generally useful.

### Payloads ###

Payload integration has been updated, coreinfo learned to cope with
UPPER CASE commands and libpayload knows how to deal with USB3 hubs.

### Added vboot support to the following platforms:

* intel/gm45
* intel/nehalem

### Moved the following platforms to C_ENVIRONMENT_BOOTBLOCK:

* intel/i945
* intel/x4x
* intel/gm45
* intel/nehalem
* intel/sandybridge
* intel/braswell

### libgfxinit ###

Most notable, dynamic CDClk configuration was added to libgfxinit,
to support higher resolution displays without changes in the static
configuration. It also received some fixes for better DP and eDP
compatibility, better error recovery for Intel's fickle GMBus and
updated platform support:
* Correct HDMI clock limit for G45.
* DP support for Ibex Peak (Ironlake graphics).
* Fixed scaling on eDP for Broadwell.
* Support for ULX variants of Haswell and later.
* Support for Kaby, Amber, Coffee and Whiskey Lake.

### Other
* Did cleanups around TSC timer
* Improved automatic VR configuration on SKL/KBL
* Filled additional fields in SMBIOS type 4
* Removed magic value replay from Intel Nehalem/ibexpeak code base
* Added OpenSBI on RISCV platforms
* Did more preparations for Intel TXT support
* Did more preparations for x86_64 stage support
* Added SSDT generator for arbitrary SuperIO chips based on devicetree.cb
