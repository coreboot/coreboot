# x86 architecture documentation

This section contains documentation about coreboot on x86 architecture.

## State of x86_64 support
At the moment there's no single board that supports x86_64 or to be exact
`ARCH_RAMSTAGE_X86_64` and `ARCH_ROMSTAGE_X86_64`.

In order to add support for x86_64 the following assumptions are made:
* The CPU supports long mode
* All memory returned by malloc must be below 4GiB in physical memory
* All code that is to be run must be below 4GiB in physical memory
* The high dword of pointers is always zero
* The reference implementation is qemu
* The CPU supports 1GiB hugepages

## Assuptions for ARCH_ROMSTAGE_X86_64 reference implementation
* 0-4GiB are identity mapped using 1GiB huge-pages
* Memory above 4GiB isn't accessible
* pagetables reside in _pagetables
* Romstage must install new pagetables in CBMEM after RAMINIT

## Assuptions for ARCH_RAMSTAGE_X86_64 reference implementation
* Romstage installed pagetables according to memory layout
* Memory above 4GiB is accessible

## Steps to add basic support for x86_64
* Add x86_64 toolchain support - *DONE*
* Fix compilation errors - *DONE*
* Fix linker errors - *TODO*
* Add x86_64 rmodule support - *ONGERRIT*
* Add x86_64 exception handlers - *TODO*
* Setup page tables for long mode - *TODO*
* Add assembly code for long mode - *TODO*
* Add assembly code to return to protected mode - *TODO*
* Implement reference code for mainboard `emulation/qemu-q35` - *TODO*

## Porting other boards
* Fix compilation errors
* Test how well CAR works with x86_64 and paging
* Improve mode switches
* Test libgfxinit / VGA Option ROMs / FSP
