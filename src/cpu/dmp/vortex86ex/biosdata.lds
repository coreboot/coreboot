/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

SECTIONS {

	/* Vortex86 ROM fixed data areas used range is too big, we need
	 * to move bootblock from default address to another place,
	 * otherwise .dmp_frontdoor section can't be included.
	 * Address before ..fbc00 is available.
	 */
	ROMLOC_MIN = 0xffffbc00 - (_erom - _rom + 16) -
		(CONFIG_SIPI_VECTOR_IN_ROM ? 4096 : 0);

	. = 0xffffbc00;
	.dmp_reserved (.): {
		*(.dmp_reserved)
	}

	. = 0xffffc000;
	.dmp_kbd_fw_part2 (.): {
		*(.dmp_kbd_fw_part2)
	}

	. = 0xffffd000;
	.dmp_mtbf_low_cnt (.): {
		*(.dmp_mtbf_low_cnt)
	}

	. = 0xffffe000;
	.dmp_kbd_fw_part1 (.): {
		*(.dmp_kbd_fw_part1)
	}

	. = 0xfffff000;
	.dmp_spi_flash_disk_driver (.): {
		*(.dmp_spi_flash_disk_driver)
	}

	. = 0xfffff800;
	.dmp_frontdoor (.): {
		*(.dmp_frontdoor)
	}

	. = 0xfffffe00;
	.dmp_isoinfo (.): {
		*(.dmp_isoinfo)
	}

	. = 0xffffffa0;
	.dmp_isodata_checksum (.): {
		*(.dmp_isodata_checksum)
	}

	. = 0xffffffb0;
	.dmp_mac (.): {
		*(.dmp_mac)
	}

	. = 0xffffffb8;
	.dmp_mtbf_limit (.): {
		*(.dmp_mtbf_limit)
	}

	. = 0xffffffc0;
	.dmp_isodata (.): {
		*(.dmp_isodata)
	}
}
