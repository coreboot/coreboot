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
	/* Trigger an error if I have an unusable start address */
	_bogus = ASSERT(_start16bit >= 0xffff0000, "_start16bit too low. Please report.");

	. = _ID_SECTION;
	.id (.): {
		KEEP(*(.id));
	}

	/* Flashrom and FILO have two alternatives for the location of .id section. */
	_ID_SECTION_END = SIZEOF(.fit_pointer) && SIZEOF(.id) > 0x28 ? 0xffffff80 : _X86_RESET_VECTOR;
	_ID_SECTION = _ID_SECTION_END - SIZEOF(.id);

	. = _FIT_POINTER;
	.fit_pointer (.): {
		KEEP(*(.fit_pointer));
	}
	_FIT_POINTER = SIZEOF(.fit_pointer) ? 0xffffffc0 : _X86_RESET_VECTOR;

	. = 0xfffffff0;
	_X86_RESET_VECTOR = .;
	.reset . : {
		*(.reset);
		. = 15;
		BYTE(0x00);
	}
}
