/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Advanced Micro Devices, Inc.
 * Copyright (C) 2008-2010 coresystems GmbH
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

/* We use ELF as output format. So that we can debug the code in some form. */
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)

TARGET(binary)
SECTIONS
{
	. = ROMSTAGE_BASE;

	.rom . : {
		_rom = .;
		*(.rom.text);
		*(.rom.data);
		*(.rodata);
		*(.rodata.*);
		*(.rom.data.*);
		. = ALIGN(16);
		_car_migrate_start = .;
		*(.car.migrate);
		_car_migrate_end = .;
		. = ALIGN(16);
		_erom = .;
	}

	/DISCARD/ : {
		*(.comment)
		*(.note)
		*(.comment.*)
		*(.note.*)
		*(.eh_frame);
	}

	. = CONFIG_DCACHE_RAM_BASE;
	.car.data . (NOLOAD) : {
		_car_data_start = .;
		*(.car.global_data);
		/* The cbmem_console section comes last to take advantage of
		 * a zero-sized array to hold the memconsole contents that
		 * grows to a bound of CONFIG_CONSOLE_CAR_BUFFER_SIZE. However,
		 * collisions within the cache-as-ram region cannot be
		 * statically checked because the cache-as-ram region usage is
		 * cpu/chipset dependent. */
		*(.car.cbmem_console);
		_car_data_end = .;
	}

	_bogus = ASSERT((SIZEOF(.car.data) <= CONFIG_DCACHE_RAM_SIZE), "Cache as RAM area is too full");
}
