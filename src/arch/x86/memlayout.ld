/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>
#include <arch/header.ld>

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
	RAMSTAGE(CONFIG_RAMBASE, 8M)

#elif ENV_ROMSTAGE
	/* The 1M size is not allocated. It's just for basic size checking.
	 * Link at 32MiB address and rely on cbfstool to relocate to XIP. */
	ROMSTAGE(CONFIG_ROMSTAGE_ADDR, 1M)

	#include "car.ld"
#elif ENV_SEPARATE_VERSTAGE
	/* The 1M size is not allocated. It's just for basic size checking.
	 * Link at 32MiB address and rely on cbfstool to relocate to XIP. */
	VERSTAGE(CONFIG_VERSTAGE_ADDR, 1M)

	#include "car.ld"
#elif ENV_BOOTBLOCK
	BOOTBLOCK(0xffffffff - CONFIG_C_ENV_BOOTBLOCK_SIZE + 1,
			CONFIG_C_ENV_BOOTBLOCK_SIZE)

	#include "car.ld"

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
