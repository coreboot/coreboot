/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

gdtptr16_offset = gdtptr16 & 0xffff;
nullidt_offset = nullidt & 0xffff;

/* Symbol _start16bit must be aligned to 4kB to start AP CPUs with
 * Startup IPI message without RAM.
 */
#if CONFIG(SIPI_VECTOR_IN_ROM)
_bogus = ASSERT((_start16bit & 0xfff) == 0, "Symbol _start16bit is not at 4 KiB boundary");
ap_sipi_vector_in_rom = (_start16bit >> 12) & 0xff;
#endif
