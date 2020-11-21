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

	. = (0xffffffff - CONFIG_ID_SECTION_OFFSET) - (__id_end - __id_start) + 1;
	.id (.): {
		KEEP(*(.id));
	}

	. = 0xffffffc0;
	.fit_pointer (.): {
		KEEP(*(.fit_pointer));
	}

	. = 0xfffffff0;
	.reset . : {
		*(.reset);
		. = 15;
		BYTE(0x00);
	}
}
