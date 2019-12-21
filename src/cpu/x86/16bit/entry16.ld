/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

gdtptr16_offset = gdtptr16 & 0xffff;
nullidt_offset = nullidt & 0xffff;

/* Symbol _start16bit must be aligned to 4kB to start AP CPUs with
 * Startup IPI message without RAM.
 */
#if CONFIG(SIPI_VECTOR_IN_ROM)
_bogus = ASSERT((_start16bit & 0xfff) == 0, "Symbol _start16bit is not at 4 KiB boundary");
ap_sipi_vector_in_rom = (_start16bit >> 12) & 0xff;
#endif
