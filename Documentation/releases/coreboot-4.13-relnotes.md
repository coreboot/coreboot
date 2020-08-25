Upcoming release - coreboot 4.13
================================

The 4.13 release is planned for November 2020.

Update this document with changes that should be in the release notes.

* Please use Markdown.
* See the past few release notes for the general format.
* The chip and board additions and removals will be updated right
  before the release, so those do not need to be added.

Significant changes
-------------------

### Hidden PCI devices

This new functionality takes advantage of the existing 'hidden' keyword in the
devicetree. Since no existing boards were using the keyword, its usage was
repurposed to make dealing with some unique PCI devices easier. The particular
case here is Intel's PMC (Power Management Controller). During the FSP-S run,
the PMC device is made hidden, meaning that its config space looks as if there
is no device there (Vendor ID reads as 0xFFFF_FFFF). However, the device does
have fixed resources, both MMIO and I/O. These were previously recorded in
different places (MMIO was typically an SA fixed resource, and I/O was treated
as an LPC resource). With this change, when a device in the tree is marked as
'hidden', it is not probed (`pci_probe_dev()`) but rather assumed to exist so
that its resources can be placed in a more natural location. This also adds the
ability for the device to participate in SSDT generation.

### Tools for generating SPDs for LP4x memory on TGL and JSL

A set of new tools `gen_spd.go` and `gen_part_id.go` are added to automate the
process of generating SPDs for LP4x memory and assigning hardware strap IDs for
memory parts used on TGL and JSL based boards. The SPD data obtained from memory
part vendors has to be massaged to format it correctly as per JEDEC and Intel MRC
expectations. These tools take a list of memory parts describing their physical
attributes as per their datasheet and convert those attributes into SPD files for
the platforms. More details about the tools are added in
[README.md](https://review.coreboot.org/plugins/gitiles/coreboot/+/refs/heads/master/util/spd_tools/intel/lp4x/README.md).

### New version of SMM loader

A new version of the SMM loader which accomodates platforms with over 32 CPU
CPU threads.  The existing version of SMM loader uses a 64K code/data
segment and only a limited number of CPU threads can fit into one segment
(because of save state, STM, other features, etc). This loader extends beyond
the 64K segment to accomodate additional CPUs and in theory allows as many
CPU threads as possible limited only by SMRAM space and not by 64K. By default
this loader version is disabled. Please see cpu/x86/Kconfig for more info.

### Initial support for x86_64

The x86_64 code support has been revived and enabled for qemu. While it started
as PoC and the only supported platform is an emulator, there's interest in
enabling additional platforms. It would allow to access more than 4GiB of memory
at runtime and possibly brings optimised code for faster execution times.
It still needs changes in assembly, fixed integer to pointer conversions in C,
wrappers for blobs, support for running Option ROMs, among other things.

### Add significant changes here
