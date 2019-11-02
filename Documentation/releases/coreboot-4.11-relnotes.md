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

Significant changes
-------------------

### Add significant changes here

### `__PRE_RAM__` is deprecated

Preprocessor use of `defined(__PRE_RAM_)` have been mostly replaced with
`if (ENV_ROMSTAGE_OR_BEFORE)` or the inverse `if (ENV_RAMSTAGE)`.

The remaining cases and `-D__PRE_RAM__` are to be removed soon after release.

### `CAR_GLOBAL` is removed where possible

For all platform code with `NO_CAR_GLOBAL_MIGRATION=y`, any `CAR_GLOBAL`
attributes have been removed. Remaining cases from common code are to be
removed soon after release.

### `TSEG` and  `cbmem_top()` mapping

Significant refactoring has bee done to achieve some consistency across platforms
and to reduce code duplication.
