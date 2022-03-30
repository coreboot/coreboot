/* SPDX-License-Identifier: GPL-2.0-only */

gdtptr_offset = gdtptr & 0xffff;
nullidt_offset = nullidt & 0xffff;

/* Symbol _start16bit must be aligned to 4kB to start AP CPUs with
 * Startup IPI message without RAM.
 */
#if CONFIG(SIPI_VECTOR_IN_ROM)
_bogus = ASSERT((_start16bit & 0xfff) == 0, "Symbol _start16bit is not at 4 KiB boundary");
ap_sipi_vector_in_rom = (_start16bit >> 12) & 0xff;
#endif

SECTIONS {

#if CONFIG(FIXED_BOOTBLOCK_SIZE)
	. = _ebootblock - CONFIG_C_ENV_BOOTBLOCK_SIZE;
#else
	. = BOOTBLOCK_TOP - PROGRAM_SZ;
	. = ALIGN(64);
#endif

	_bootblock = .;

	INCLUDE "bootblock/lib/program.ld"

	/*
	 * Allocation reserves extra space here. Alignment requirements
	 * may cause the total size of a section to change when the start
	 * address gets applied.
	 */
	PROGRAM_SZ = SIZEOF(.text) + 512;

	. = MIN(_ECFW_PTR, MIN(_ID_SECTION, _FIT_POINTER)) - EARLYASM_SZ;
	. = CONFIG(SIPI_VECTOR_IN_ROM) ? ALIGN(4096) : ALIGN(16);
	BOOTBLOCK_TOP = .;
	.init (.) : {
		*(.init._start);
		*(.init);
		*(.init.*);
	}

	/*
	 * Allocation reserves extra space here. Alignment requirements
	 * may cause the total size of a section to change when the start
	 * address gets applied.
	 */
	EARLYASM_SZ = SIZEOF(.init) + (CONFIG(SIPI_VECTOR_IN_ROM) ? 4096 : 16);

	. = _ID_SECTION;
	.id (.): {
		KEEP(*(.id));
	}

	/* Flashrom and FILO have two alternatives for the location of .id section. */
	_ID_SECTION_END = SIZEOF(.fit_pointer) && SIZEOF(.id) > 0x28 ? 0xffffff80 : _X86_RESET_VECTOR;
	_ID_SECTION = _ID_SECTION_END - SIZEOF(.id);

	. = _ECFW_PTR;
	.ecfw_ptr (.): {
		ASSERT((SIZEOF(.ecfw_ptr) == CONFIG_ECFW_PTR_SIZE), "Size of ecfw_ptr is incorrect");
		KEEP(*(.ecfw_ptr));
	}
	_ECFW_PTR = SIZEOF(.ecfw_ptr) ? CONFIG_ECFW_PTR_ADDR : _X86_RESET_VECTOR;

	. = _FIT_POINTER;
	.fit_pointer (.): {
		KEEP(*(.fit_pointer));
	}
	_FIT_POINTER = SIZEOF(.fit_pointer) ? 0xffffffc0 : _X86_RESET_VECTOR;

	. = 0xfffffff0;
	_X86_RESET_VECTOR = .;
	.reset . : {
		*(.reset);
		. = _X86_RESET_VECTOR_FILLING;
		BYTE(0);
	}
	. = 0xfffffffc;
	.header_pointer . : {
		KEEP(*(.header_pointer));
	}
	_X86_RESET_VECTOR_FILLING = 15 - SIZEOF(.header_pointer);
	_ebootblock = .;
}

/*
 * Tests _bogus1 and _bogus2 are here to detect case of symbol addresses truncated
 * to 32 bits and intermediate files reaching size of close to 4 GiB.
 */
_bogus1 = ASSERT(_bootblock & 0x80000000, "_bootblock too low, invalid ld script");
_bogus2 = ASSERT(_start16bit & 0x80000000, "_start16bit too low, invalid ld script");
_bogus3 = ASSERT(_start16bit >= 0xffff0000, "_start16bit too low. Please report.");
_bogus4 = ASSERT(_ebootblock - _bootblock <= CONFIG_C_ENV_BOOTBLOCK_SIZE,
		 "_bootblock too low, increase C_ENV_BOOTBLOCK_SIZE");
