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

### Native refcode implementation for Bay Trail

Bay Trail no longer needs a refcode binary to function properly. The refcode
was reimplemented as coreboot code, which should be functionally equivalent.
Thus, coreboot only needs to run the MRC.bin to successfully boot Bay Trail.

### Unusual config files to build test more code

There's some new highly-unusual config files, whose only purpose is to coerce
Jenkins into build-testing several disabled-by-default coreboot config options.
This prevents them from silently decaying over time because of build failures.

### Initial support for Intel Trusted eXecution Technology

coreboot now supports enabling Intel TXT. Though it's not feature-complete yet,
the code allows successfully launching tboot, a Measured Launch Environment. It
was tested on Haswell using an Asrock B85M Pro4 mainboard with TPM 2.0 on LPC.
Though support for other platforms is still not ready, it is being worked on.
The Haswell MRC.bin needs to be patched so as to enable DPR. Given that the MRC
binary cannot be redistributed, the best long-term solution is to replace it.

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

A new version of the SMM loader which accommodates platforms with over 32
CPU threads.  The existing version of SMM loader uses a 64K code/data
segment and only a limited number of CPU threads can fit into one segment
(because of save state, STM, other features, etc). This loader extends beyond
the 64K segment to accommodate additional CPUs and in theory allows as many
CPU threads as possible limited only by SMRAM space and not by 64K. By default
this loader version is disabled. Please see cpu/x86/Kconfig for more info.

### Address Sanitizer

coreboot now has an in-built Address Sanitizer, a runtime memory debugger
designed to find out-of-bounds access and use-after-scope bugs. It is made
available on all x86 platforms in ramstage and on QEMU i440fx, Intel Apollo
Lake, and Haswell in romstage. Further, it can be enabled in romstage on other
x86 platforms as well. Refer [ASan documentation](../technotes/asan.md) for
more info.

### Initial support for x86_64

The x86_64 code support has been revived and enabled for QEMU. While it started
as PoC and the only supported platform is an emulator, there's interest in
enabling additional platforms. It would allow to access more than 4GiB of memory
at runtime and possibly brings optimised code for faster execution times.
It still needs changes in assembly, fixed integer to pointer conversions in C,
wrappers for blobs, support for running Option ROMs, among other things.

### Preparations to minimize enabling PCI bus mastering

For security reasons, bus mastering should be enabled as late as possible. In
coreboot, it's usually not necessary and payloads should only enable it for
devices they use. Since not all payloads enable bus mastering properly yet,
some Kconfig options were added as an intermediate step to give some sort of
"backwards compatibility", which allow enabling or disabling bus mastering by
groups.

Currently available groups are:

* PCI bridges
* Any devices

For now, "Any devices" is enabled by default to keep the traditional behaviour,
which also includes all other options. This is currently necessary, for instance,
for libpayload-based payloads as the drivers don't enable bus mastering for PCI
bridges.

Exceptional cases, that may still need early bus master enabling in the future,
should get their own per-reason Kconfig option. Ideally before the next release.

### Early runtime configurability of the console log level

Traditionally, we didn't allow the log level of the `romstage` console
to be changed at runtime (e.g. via `get_option()`). It turned out that
the technical constraints for this (no global variables in `romstage`)
vanished long ago, though. The new behaviour is to query `get_option()`
now from the second stage that uses the console on. In other words, if
the `bootblock` already enables the console, the `romstage` log level
can be changed via `get_option()`. Keeping the log level of the first
console static ensures that we can see console output even if there's
a bug in the more involved code to query options.

### Resource allocator v4

A new revision of resource allocator v4 is now added to coreboot that supports
mutiple ranges for allocating resources. Unlike the previous allocator (v3), it does
not use the topmost available window for allocation. Instead, it uses the first
window within the address space that is available and satisfies the resource request.
This allows utilization of the entire available address space and also allows
allocation above the 4G boundary. The old resource allocator v3 is still retained for
some AMD platforms that do not conform to the requirements of the allocator.

### Add significant changes here

Deprecations
------------

### PCI bus master configuration options

In order to minimize the usage of PCI bus mastering, the options we introduced in
this release will be dropped in a future release again. For more details, please
see [Preparations to minimize enabling PCI bus mastering](#preparations-to-minimize-enabling-pci-bus-mastering-in-coreboot).

### Resource allocator v3

Resource allocator v3 is retained in coreboot tree because the following platforms
do not conform to the requirements of the resource allocation i.e. not all the fixed
resources of the platform are provided during the `read_resources()` operation:

* northbridge/amd/pi/00630F01
* northbridge/amd/pi/00730F01
* northbridge/amd/pi/00660F01
* northbridge/amd/agesa/family14
* northbridge/amd/agesa/family15tn
* northbridge/amd/agesa/family16kb

In order to have a single unified allocator in coreboot, this notice is being added
to ensure that the platforms listed above are fixed before the next release. If there
is interest in maintaining support for these platforms beyond the next release,
please ensure that the platforms are fixed to conform to the expectations of resource
allocation.
