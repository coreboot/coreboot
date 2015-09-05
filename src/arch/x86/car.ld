/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Advanced Micro Devices, Inc.
 * Copyright (C) 2008-2010 coresystems GmbH
 * Copyright 2015 Google Inc
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

/* This file is included inside a SECTIONS block */
. = CONFIG_DCACHE_RAM_BASE;
.car.data . (NOLOAD) : {
	_car_data_start = .;
#if IS_ENABLED(CONFIG_HAS_PRECBMEM_TIMESTAMP_REGION)
	TIMESTAMP(., 0x100)
#endif
	*(.car.global_data);
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_car_data_end = .;

	PRERAM_CBMEM_CONSOLE(., (CONFIG_LATE_CBMEM_INIT ? 0 : 0xc00))
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
