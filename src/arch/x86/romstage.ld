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
 * Foundation, Inc.
 */

TARGET(binary)
SECTIONS
{
	. = ROMSTAGE_BASE;

	.rom . : {
		_rom = .;
		*(.rom.text);
		*(.rom.data);
		*(.text);
		*(.text.*);
		. = ALIGN(4);
		_cbmem_init_hooks = .;
		KEEP(*(.rodata.cbmem_init_hooks));
		_ecbmem_init_hooks = .;
		*(.rodata);
		*(.rodata.*);
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
#if IS_ENABLED(CONFIG_HAS_PRECBMEM_TIMESTAMP_REGION)
		_timestamp = .;
		. = . + 0x100;
		_etimestamp = .;
#endif
		*(.car.global_data);
		_car_data_end = .;
		/* The preram cbmem console area comes last to take advantage
		 * of a zero-sized array to hold the memconsole contents.
		 * However, collisions within the cache-as-ram region cannot be
		 * statically checked because the cache-as-ram region usage is
		 * cpu/chipset dependent. */
		_preram_cbmem_console = .;
		_epreram_cbmem_console = . + (CONFIG_LATE_CBMEM_INIT ? 0 : 0xc00);
	}

	/* Global variables are not allowed in romstage
	 * This section is checked during stage creation to ensure
	 * that there are no global variables present
	 */

	. = 0xffffff00;
	.illegal_globals . : {
		*(EXCLUDE_FILE ("*/libagesa.*.a:" "*/buildOpts.romstage.o" "*/agesawrapper.romstage.o" "*/vendorcode/amd/agesa/*" "*/vendorcode/amd/cimx/*") .data)
		*(EXCLUDE_FILE ("*/libagesa.*.a:" "*/buildOpts.romstage.o" "*/agesawrapper.romstage.o" "*/vendorcode/amd/agesa/*" "*/vendorcode/amd/cimx/*") .data.*)
		*(.bss)
		*(.bss.*)
		*(.sbss)
		*(.sbss.*)
	}

	_bogus = ASSERT((CONFIG_DCACHE_RAM_SIZE == 0) || (SIZEOF(.car.data) + 0xc00 <= CONFIG_DCACHE_RAM_SIZE), "Cache as RAM area is too full");
}
