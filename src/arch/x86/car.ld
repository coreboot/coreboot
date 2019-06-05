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
 */

/* This file is included inside a SECTIONS block */
. = CONFIG_DCACHE_RAM_BASE;
.car.data . (NOLOAD) : {
	_car_region_start = . ;
#if CONFIG(PAGING_IN_CACHE_AS_RAM)
	/* Page table pre-allocation. CONFIG_DCACHE_RAM_BASE should be 4KiB
	 * aligned when using this option. */
	_pagetables = . ;
	.  += 4096 * CONFIG_NUM_CAR_PAGE_TABLE_PAGES;
	_epagetables = . ;
#endif
	/* Vboot work buffer only needs to be available when verified boot
	 * starts in bootblock. */
#if CONFIG(VBOOT_STARTS_IN_BOOTBLOCK)
	VBOOT2_WORK(., 12K)
#endif
	/* Vboot measured boot TCPA log measurements.
	 * Needs to be transferred until CBMEM is available
	 */
#if CONFIG(VBOOT_MEASURED_BOOT)
	VBOOT2_TPM_LOG(., 2K)
#endif
	/* Stack for CAR stages. Since it persists across all stages that
	 * use CAR it can be reused. The chipset/SoC is expected to provide
	 * the stack size. */
#if CONFIG(C_ENVIRONMENT_BOOTBLOCK)
	_car_stack_start = .;
	. += CONFIG_DCACHE_BSP_STACK_SIZE;
	_car_stack_end = .;
#endif
	/* The pre-ram cbmem console as well as the timestamp region are fixed
	 * in size. Therefore place them above the car global section so that
         * multiple stages (romstage and verstage) have a consistent
         * link address of these shared objects. */
	PRERAM_CBMEM_CONSOLE(., CONFIG_PRERAM_CBMEM_CONSOLE_SIZE)
#if CONFIG(PAGING_IN_CACHE_AS_RAM)
	. = ALIGN(32);
	/* Page directory pointer table resides here. There are 4 8-byte entries
	 * totalling 32 bytes that need to be 32-byte aligned. The reason the
	 * pdpt are not colocated with the rest of the page tables is to reduce
	 * fragmentation of the CAR space that persists across stages. */
	_pdpt = .;
	. += 32;
	_epdpt = .;
#endif
	_car_relocatable_data_start = .;
	/* The timestamp implementation relies on this storage to be around
	 * after migration. One of the fields indicates not to use it as the
	 * backing store once cbmem comes online. Therefore, this data needs
	 * to reside in the migrated area (between _car_relocatable_data_start
	 * and _car_relocatable_data_end). */
	TIMESTAMP(., 0x200)
	_car_ehci_dbg_info_start = .;
	/* Reserve sizeof(struct ehci_dbg_info). */
        . += 80;
        _car_ehci_dbg_info_end = .;
	/* _car_global_start and _car_global_end provide symbols to per-stage
	 * variables that are not shared like the timestamp and the pre-ram
	 * cbmem console. This is useful for clearing this area on a per-stage
	 * basis when more than one stage uses cache-as-ram for CAR_GLOBALs. */
	_car_global_start = .;
#if CONFIG(NO_CAR_GLOBAL_MIGRATION)
	/* Allow global unitialized variables when CAR_GLOBALs are not used. */
	*(.bss)
	*(.bss.*)
	*(.sbss)
	*(.sbss.*)
#else
	/* .car.global_data objects only around when
	 * !CONFIG_NO_CAR_GLOBAL_MIGRATION is employed. */
	*(.car.global_data);
#endif
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_car_global_end = .;
	_car_relocatable_data_end = .;

#if !CONFIG(C_ENVIRONMENT_BOOTBLOCK)
	_car_stack_start = .;
	_car_stack_end = _car_region_end;
#endif
	_car_region_end = . + CONFIG_DCACHE_RAM_SIZE - (. - _car_region_start);
}

/* Global variables are not allowed in romstage
 * This section is checked during stage creation to ensure
 * that there are no global variables present
 */

. = 0xffffff00;
.illegal_globals . : {
	*(EXCLUDE_FILE ("*/libagesa.*.a:" "*/romstage*/buildOpts.o" "*/romstage*/agesawrapper.o" "*/vendorcode/amd/agesa/*" "*/vendorcode/amd/cimx/*") .data)
		*(EXCLUDE_FILE ("*/libagesa.*.a:" "*/romstage*/buildOpts.o" "*/romstage*/agesawrapper.o" "*/vendorcode/amd/agesa/*" "*/vendorcode/amd/cimx/*") .data.*)
#if !CONFIG(NO_CAR_GLOBAL_MIGRATION)
	*(.bss)
	*(.bss.*)
	*(.sbss)
	*(.sbss.*)
#else
	/* In case something sneaks through when it shouldn't. */
	*(.car.global_data);
#endif
}

_bogus = ASSERT((CONFIG_DCACHE_RAM_SIZE == 0) || (SIZEOF(.car.data) <= CONFIG_DCACHE_RAM_SIZE), "Cache as RAM area is too full");
#if CONFIG(PAGING_IN_CACHE_AS_RAM)
_bogus2 = ASSERT(_pagetables == ALIGN(_pagetables, 4096), "_pagetables aren't 4KiB aligned");
#endif
#if CONFIG(C_ENVIRONMENT_BOOTBLOCK)
_bogus3 = ASSERT(CONFIG_DCACHE_BSP_STACK_SIZE > 0x0, "BSP stack size not configured");
#endif
