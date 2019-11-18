Upcoming release - coreboot 4.11
================================

The 4.11 release is planned for October 2019

Update this document with changes that should be in the release
notes.
* Please use Markdown.
* See the [4.9](coreboot-4.9-relnotes.md) and [4.10](coreboot-4.10-relnotes.md)
  release notes for the general format.
* The chip and board additions and removals will be updated right
  before the release, so those do not need to be added.

Clean Up
--------
Because there was only a single developer board (AMD Torpedo)
using AGESA family 12h, and because there were multiple,
unique Coverity issues with it, the associated vendorcode will
be removed shortly after this release.

Support for the MIPS architecture will also be removed shortly after
this release as the only board in the tree was a discontinued development
board and no other work has picked up MIPS support, so it's very likely
broken already.

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

Significant refactoring has bee done to achieve some consistency across platforms
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

### Added VBOOT support to the following platforms:
* intel/gm45
* intel/nehalem

### Moved the following platforms to C_ENVIRONMENT_BOOTBLOCK:
* intel/i945
* intel/x4x
* intel/gm45
* intel/nehalem
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
