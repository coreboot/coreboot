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

### Add significant changes here
