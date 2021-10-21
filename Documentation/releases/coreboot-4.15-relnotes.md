Upcoming release - coreboot 4.15
================================

The 4.15 release is planned for October 2021.

Update this document with changes that should be in the release notes.

* Please use Markdown.
* See the past few release notes for the general format.
* The chip and board additions and removals will be updated right
  before the release, so those do not need to be added.

Significant changes
-------------------

### Merged family of Asus mainboards using H61 chipset

By using newer coreboot features like board variants and override devicetrees,
lots of code can now be shared. This should ease maintenance and also make it
easier for newcomers to add support for even more mainboards.

### Changed default setting for Intel chipset lockdown

Previously, the default behaviour for Intel chipset lockdown was to let the FSP
do it. Since all related mainboards used the coreboot mechanisms for chipset
lockdown, the default behaviour was changed to that.

### Payloads unit testing

Libpayload now supports the mock architecture, which can be used for unit testing
payloads. (For examples see
[depthcharge](https://chromium.googlesource.com/chromiumos/platform/depthcharge/)
payload)

### Unit testing infrastructure

Unit testing of libpayload is now possible in the same fashion as in the main
coreboot tree.

### Add significant changes here
