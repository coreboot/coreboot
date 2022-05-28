Upcoming release - coreboot 4.17
================================

The 4.17 release is planned for May, 2022.

We are continuing the quarterly release cadence in order to enable others to
release quarterly on a fresher version of coreboot.

Update this document with changes that should be in the release notes.

* Please use Markdown.
* See the past few release notes for the general format.
* The chip and board additions and removals will be updated right
  before the release, so those do not need to be added.

Significant changes
-------------------

### CBMEM init hooks changed

Instead of having per stage x_CBMEM_INIT_HOOK, we now have only 2 hooks:
* CBMEM_CREATION_HOOK: Used only in the first stage that creates cbmem,
  typically romstage. For instance code that migrates data from cache
  as ram to dram would use this hook.
* CBMEM_READY_HOOK: Used in every stage that has cbmeme. An example would
  initializing the cbmem console by appending to what previous stages
  logged.
This reason for this change is improved flexibility with regards to which
stage initializes cbmem.

### Payloads
* SeaBIOS: Update stable release from 1.14.0 to 1.16.0
* iPXE: Update stable release from 2019.3 to 2022.1
* Add new coreDOOM payload, a port of Doom to libpayload
* Add "GRUB2 atop SeaBIOS" aka "SeaGRUB" option, which builds GRUB2 as a
  secondary payload for SeaBIOS with the GRUB2 set as the default boot entry.
  This allows GRUB2 to use BIOS callbacks provided by SeaBIOS as a fallback
  method to access hardware that the native GRUB2 payload cannot access.
* Add option to build SeaBIOS and GRUB2 as secondary payloads

### Add significant changes here
