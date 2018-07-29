Upcoming release - coreboot 4.9
==========================

The 4.9 release is planned for November 2018

Update this document with changes that should be in the release
notes.
* Please use Markdown.
* See the [4.7](coreboot-4.7-relnotes.md) and [4.8](coreboot-4.8.1-relnotes.md)
 release notes for the general format.
* The chip and board additions and removals will be updated right
before the release, so those do not need to be added.



General changes
---------------

* Various code cleanups
 * Removed `device_t` in favor of `struct device*` in ramstage code
 * Improve adherence to coding style
* Expand use of the postcar stage
* Add bootblock compression capability: on systems that copy the bootblock
  from very slow flash to ERAM, allow adding a stub that decompresses the
  bootblock into ERAM to minimize the amount of flash reads

Toolchain
---------

* Update IASL to version 10280531
