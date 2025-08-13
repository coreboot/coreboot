/* SPDX-License-Identifier: GPL-2.0-only */

/* CACHE_ROM_SIZE defined here. */
#include <cpu/x86/mtrr.h>
#include <memlayout.h>

/* This file is included inside a SECTIONS block */
. = CONFIG_DCACHE_RAM_BASE;
.car.data . (NOLOAD) : {
	_car_region_start = . ;
	. += CONFIG_FSP_M_RC_HEAP_SIZE;
#if CONFIG(FSP_SPEC_VIOLATION_XEON_SP_HEAP_WORKAROUND)
	REGION(fspm_heap, ., CONFIG_FSP_TEMP_RAM_SIZE, 16)
#endif

#if CONFIG(PAGING_IN_CACHE_AS_RAM)
	/* Page table pre-allocation. CONFIG_DCACHE_RAM_BASE should be 4KiB
	 * aligned when using this option. */
	REGION(pagetables, ., 4K * CONFIG_NUM_CAR_PAGE_TABLE_PAGES, 4K)
#endif
#if CONFIG(VBOOT_STARTS_IN_BOOTBLOCK)
	/* Vboot work buffer only needs to be available when verified boot
	 * starts in bootblock. */
	VBOOT2_WORK(., VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE)
#endif
#if CONFIG(TPM_MEASURED_BOOT)
	/* Vboot measured boot TPM log measurements.
	 * Needs to be transferred until CBMEM is available */
	TPM_LOG(., 2K)
#endif
	/* Stack for CAR stages. Since it persists across all stages that
	 * use CAR it can be reused. The chipset/SoC is expected to provide
	 * the stack size. */
	REGION(car_stack, ., CONFIG_DCACHE_BSP_STACK_SIZE, 4)
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
	REGION(pdpt, ., 32, 32)
#endif

	TIMESTAMP(., 0x200)

#if !CONFIG(NO_CBFS_MCACHE)
	CBFS_MCACHE(., CONFIG_CBFS_MCACHE_SIZE)
#endif
#if !CONFIG(NO_FMAP_CACHE)
	FMAP_CACHE(., FMAP_SIZE)
#endif

	. = ALIGN(CONFIG_CBFS_CACHE_ALIGN);
	CBFS_CACHE(., CONFIG_PRERAM_CBFS_CACHE_SIZE)

	/* Reserve sizeof(struct ehci_dbg_info). */
	REGION(car_ehci_dbg_info, ., 80, 1)

	/* _bss and _ebss provide symbols to per-stage
	 * variables that are not shared like the timestamp and the pre-ram
	 * cbmem console. This is useful for clearing this area on a per-stage
	 * basis when more than one stage uses cache-as-ram. */

#if ENV_SEPARATE_DATA_AND_BSS
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_bss = .;
	/* Allow global uninitialized variables for stages without CAR teardown. */
	*(.bss)
	*(.bss.*)
	*(.lbss)
	*(.lbss.*)
	*(.sbss)
	*(.sbss.*)
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_ebss = .;
	RECORD_SIZE(bss)
#endif

#if ENV_SEPARATE_ROMSTAGE && CONFIG(ASAN_IN_ROMSTAGE)
	_shadow_size = (_ebss - _car_region_start) >> 3;
	REGION(asan_shadow, ., _shadow_size, ARCH_POINTER_ALIGN_SIZE)
#endif
}

#if ENV_SEPARATE_DATA_AND_BSS
/* This symbol defines the load address of the Cache-As-RAM .data
 * section. It should be right at the end of the .text section (_etext)
 * and ARCH_POINTER_ALIGN_SIZE aligned. */
_data_load = _etext;

_bogus = ASSERT(_etext == ALIGN(_etext, ARCH_POINTER_ALIGN_SIZE), "Cache-As-RAM load address is improperly defined.");

.data ALIGN(ARCH_POINTER_ALIGN_SIZE) : AT (_data_load) {
	_data = .;
	*(.data);
	*(.data.*);
	*(.ldata);
	*(.ldata.*);
	*(.sdata);
	*(.sdata.*);
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_edata = .;
	RECORD_SIZE(data)
} : data_segment
#endif

_car_unallocated_start = .;
_car_region_end = . + CONFIG_DCACHE_RAM_SIZE - (. - _car_region_start)
		  - CONFIG_FSP_T_RESERVED_SIZE;

. = _car_region_end;
.car.mrc_var . (NOLOAD) : {
	. += CONFIG_DCACHE_RAM_MRC_VAR_SIZE;
}
.car.fspt_reserved . (NOLOAD) : {
	. +=  CONFIG_FSP_T_RESERVED_SIZE;
}

#if ENV_BOOTBLOCK
. = ALIGN(64);
_car_mtrr_end = .;
_car_mtrr_start = _car_region_start;

_car_mtrr_size = _car_mtrr_end - _car_mtrr_start;
_car_mtrr_sz_log2 = 1 << LOG2CEIL(_car_mtrr_size);
_car_mtrr_mask = ~(MAX(4096, _car_mtrr_sz_log2) - 1);

#if !CONFIG(NO_XIP_EARLY_STAGES)
_xip_program_sz_log2 = 1 << LOG2CEIL(_ebootblock - _bootblock);
_xip_mtrr_mask = ~(MAX(4096, _xip_program_sz_log2) - 1);
#endif

_rom_mtrr_mask = ~(CACHE_ROM_SIZE - 1);
_rom_mtrr_base = _rom_mtrr_mask;
#endif

#if ENV_SEPARATE_DATA_AND_BSS
_bogus = ASSERT((CONFIG_DCACHE_RAM_SIZE == 0) || (SIZEOF(.car.data) + SIZEOF(.data) <= CONFIG_DCACHE_RAM_SIZE), "Cache as RAM area is too full");
#else
_bogus = ASSERT((CONFIG_DCACHE_RAM_SIZE == 0) || (SIZEOF(.car.data) <= CONFIG_DCACHE_RAM_SIZE), "Cache as RAM area is too full");
#endif
#if CONFIG(PAGING_IN_CACHE_AS_RAM)
_bogus2 = ASSERT(_pagetables == ALIGN(_pagetables, 4096), "_pagetables aren't 4KiB aligned");
#endif
_bogus3 = ASSERT(CONFIG_DCACHE_BSP_STACK_SIZE > 0x0, "BSP stack size not configured");
#if CONFIG(NO_XIP_EARLY_STAGES) && (ENV_SEPARATE_ROMSTAGE || ENV_SEPARATE_VERSTAGE)
_bogus4 = ASSERT(_eprogram <= _car_region_end, "Stage end too high !");
_bogus5 = ASSERT(_program >= _car_unallocated_start, "Stage start too low!");
#endif
