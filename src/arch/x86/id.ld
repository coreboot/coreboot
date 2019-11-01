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

SECTIONS {
	. = (CONFIG_X86_RESET_VECTOR - CONFIG_ID_SECTION_OFFSET) - (__id_end - __id_start) + 0x10;
	.id (.): {
		KEEP(*(.id))
	}
}
