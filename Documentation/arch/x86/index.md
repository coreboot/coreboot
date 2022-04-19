# x86 architecture documentation

This section contains documentation about coreboot on x86 architecture.

* [x86 PAE support](pae.md)

## State of x86_64 support
At the moment there's only experimental x86_64 support.
The `emulation/qemu-i440fx` and `emulation/qemu-q35` boards do support
*ARCH_RAMSTAGE_X86_64* , *ARCH_POSTCAR_X86_64* and *ARCH_ROMSTAGE_X86_64*.

In order to add support for x86_64 the following assumptions were made:
* The CPU supports long mode
* All memory returned by malloc must be below 4GiB in physical memory
* All code that is to be run must be below 4GiB in physical memory
* The high dword of pointers is always zero
* The reference implementation is qemu
* The CPU supports 1GiB hugepages
* x86 payloads are loaded below 4GiB in physical memory and are jumped
  to in *protected mode*

## Assumptions for all stages using the reference implementation
* 0-4GiB are identity mapped using 2MiB-pages as WB
* Memory above 4GiB isn't accessible
* page tables reside in memory mapped ROM
* A stage can install new page tables in RAM

## Page tables
A `pagetables` cbfs file is generated based on an assembly file.

To generate the static page tables it must know the physical address where to
place the file.

The page tables contains the following structure:
* PML4E pointing to PDPE
* PDPE with *$n* entries each pointing to PDE
* *$n* PDEs with 512 entries each

At the moment *$n* is 4, which results in identity mapping the lower 4 GiB.

## Basic x86_64 support
Basic support for x86_64 has been implemented for QEMU mainboard target.

## Reference implementation
The reference implementation is
* [QEMU i440fx](../../mainboard/emulation/qemu-i440fx.md)
* [QEMU Q35](../../mainboard/emulation/qemu-q35.md)

## TODO
* Identity map memory above 4GiB in ramstage

## Future work

1. Fine grained page tables for SMM:
   * Must not have execute and write permissions for the same page.
   * Must allow only that TSEG pages can be marked executable
   * Must reside in SMRAM
2. Support 64bit PCI BARs above 4GiB
3. Place and run code above 4GiB

## Porting other boards
* Fix compilation errors
* Test how well CAR works with x86_64 and paging
* Improve mode switches
* Test libgfxinit / VGA Option ROMs / FSP

## Known bugs on real hardware

According to Intel x86_64 mode hasn't been validated in CAR environments.
Until now it could be verified on various Intel platforms and no issues have
been found.

## Known bugs on KVM enabled qemu

The `x86_64` reference code runs fine in qemu soft-cpu, but has serious issues
when using KVM mode on some machines. The workaround is to *not* place
page-tables in ROM, as done in
[CB:49228](https://review.coreboot.org/c/coreboot/+/49228).

Here's a list of known issues:

* After entering long mode, the FPU doesn't work anymore, including accessing
  MMX registers. It works fine before entering long mode. It works fine when
  switching back to protected mode. Other registers, like SSE registers, are
  working fine.
* Reading from virtual memory, when the page tables are stored in ROM, causes
  the MMU to abort the "page table walking" mechanism when the lower address
  bits of the virtual address to be translated have a specific pattern.
  Instead of loading the correct physical page, the one containing the
  page tables in ROM will be loaded and used, which breaks code and data as
  the page table doesn't contain the expected data. This in turn leads to
  undefined behaviour whenever the 'wrong' address is being read.
* Disabling paging in compatibility mode crashes the CPU.
* Returning from long mode to compatibility mode crashes the CPU.
* Entering long mode crashes on AMD host platforms.
