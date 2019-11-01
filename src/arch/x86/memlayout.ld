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

#include <memlayout.h>
#include <arch/header.ld>

/* Pull in the either CAR or early DRAM rules. */
#if ENV_ROMSTAGE_OR_BEFORE
#if ENV_CACHE_AS_RAM
#define EARLY_MEMLAYOUT	"car.ld"
#else
#error "Early DRAM environment for x86 is work-in-progress. */
#endif
#endif

SECTIONS
{
	/*
	 * It would be good to lay down RAMSTAGE, ROMSTAGE, etc consecutively
	 * like other architectures/chipsets it's not possible because of
	 * the linking games played during romstage creation by trying
	 * to find the final landing place in CBFS for XIP. Therefore,
	 * conditionalize with macros.
	 */
#if ENV_RAMSTAGE
	RAMSTAGE(CONFIG_RAMBASE, (CONFIG(RELOCATABLE_RAMSTAGE) ? 8M :
					CONFIG_RAMTOP - CONFIG_RAMBASE))

#elif ENV_ROMSTAGE
	/* The 1M size is not allocated. It's just for basic size checking.
	 * Link at 32MiB address and rely on cbfstool to relocate to XIP. */
	ROMSTAGE(CONFIG_ROMSTAGE_ADDR, 1M)

	#include EARLY_MEMLAYOUT
#elif ENV_VERSTAGE
	/* The 1M size is not allocated. It's just for basic size checking.
	 * Link at 32MiB address and rely on cbfstool to relocate to XIP. */
	VERSTAGE(CONFIG_VERSTAGE_ADDR, 1M)

	#include EARLY_MEMLAYOUT
#elif ENV_BOOTBLOCK
	/* arch/x86/bootblock.ld contains the logic for the ROMCC_BOOTBLOCK linking. */
	BOOTBLOCK(CONFIG_X86_RESET_VECTOR - CONFIG_C_ENV_BOOTBLOCK_SIZE + 0x10,
			CONFIG_C_ENV_BOOTBLOCK_SIZE)

	#include EARLY_MEMLAYOUT

#elif ENV_POSTCAR
	POSTCAR(32M, 1M)
#endif
}

#if ENV_BOOTBLOCK
/* Bootblock specific scripts which provide more SECTION directives. */
#include <cpu/x86/16bit/entry16.ld>
#include <cpu/x86/16bit/reset16.ld>
#include <arch/x86/id.ld>
#if CONFIG(CPU_INTEL_FIRMWARE_INTERFACE_TABLE)
#include <cpu/intel/fit/fit.ld>
#endif
#endif  /* ENV_BOOTBLOCK */
