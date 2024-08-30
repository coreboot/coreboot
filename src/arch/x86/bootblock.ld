/* SPDX-License-Identifier: GPL-2.0-only */

gdtptr_offset = gdtptr & 0xffff;
nullidt_offset = nullidt & 0xffff;

#define ID_TOP 0xffffff80
#define INTEL_FIT_POINTER 0xffffffc0
#define RESET_VECTOR 0xfffffff0
#define CBFS_HEADER_POINTER 0xfffffffc

/* Symbol _start16bit must be aligned to 4kB to start AP CPUs with
 * Startup IPI message without RAM.
 */
#if CONFIG(SIPI_VECTOR_IN_ROM)
_bogus = ASSERT((_start16bit & 0xfff) == 0, "Symbol _start16bit is not at 4 KiB boundary");
ap_sipi_vector_in_rom = (_start16bit >> 12) & 0xff;
#endif

/*
 * Keep the start address independent of section sizes. LLD cannot resolve the
 * circular dependency when both .text and .init are positioned using their
 * final sizes. SIPI entry points require 4 KiB alignment; otherwise reserve
 * the last 3 KiB below the fixed metadata for early assembly.
 *
 * TODO: Make EARLY_INIT_BASE configurable if a future platform needs a
 * different amount of space for early assembly.
 */
#if CONFIG(SIPI_VECTOR_IN_ROM)
#define EARLY_INIT_BASE 0xfffff000
#else
#define EARLY_INIT_BASE 0xfffff400
#endif

SECTIONS {

#if CONFIG(FIXED_BOOTBLOCK_SIZE)
	. = _ebootblock - CONFIG_C_ENV_BOOTBLOCK_SIZE;
#else
	. = EARLY_INIT_BASE - PROGRAM_SZ;
	/* Page tables need to be at a 4K boundary so align the bootblock downwards */
	. = ALIGN(4096);
	. -= 4096;
#endif

	_bootblock = .;

	INCLUDE "bootblock/lib/program.ld"

	PROGRAM_SZ = SIZEOF(.text) + SIZEOF(.data) + CONFIG_X86_BOOTBLOCK_EXTRA_PROGRAM_SZ;

	. = EARLY_INIT_BASE;
	.init (.) : {
		*(.init._start);
		*(.init);
		*(.init.*);
	}

#if CONFIG_ECFW_PTR_ADDR
	. = CONFIG_ECFW_PTR_ADDR;
	.ecfw_ptr (.): {
		ASSERT((SIZEOF(.ecfw_ptr) == CONFIG_ECFW_PTR_SIZE), "Size of ecfw_ptr is incorrect");
		KEEP(*(.ecfw_ptr));
	}
#endif

	/* Flashrom and FILO search for the ID below 0xffffff80. */
	. = ID_TOP - SIZEOF(.id);
	.id (.): {
		KEEP(*(.id));
	}

	. = INTEL_FIT_POINTER;
	.fit_pointer (.): {
		KEEP(*(.fit_pointer));
	}

	. = RESET_VECTOR;
	.reset . : {
		*(.reset);
	}

	. = CBFS_HEADER_POINTER;
	.header_pointer . : {
		KEEP(*(.header_pointer));
	}
	_ebootblock = .;
}

_bogus1 = ASSERT(_bootblock & 0x80000000, "_bootblock too low, invalid ld script");
_bogus2 = ASSERT(_start16bit & 0x80000000, "_start16bit too low, invalid ld script");
_bogus3 = ASSERT(_start16bit >= 0xffff0000, "_start16bit too low. Please report.");
_bogus4 = ASSERT(_ebootblock - _bootblock <= CONFIG_C_ENV_BOOTBLOCK_SIZE,
		 "_bootblock too low, increase C_ENV_BOOTBLOCK_SIZE");
