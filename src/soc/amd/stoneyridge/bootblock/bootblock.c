/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation..
 * Copyright (C) 2017 Advanced Micro Devices
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

#include <stdint.h>
#include <assert.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/amdfam15.h>
#include <smp/node.h>
#include <bootblock_common.h>
#include <agesawrapper.h>
#include <agesawrapper_call.h>
#include <soc/southbridge.h>

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	/*
	 * Call lib/bootblock.c main with BSP, shortcut for APs
	 *  todo: rearchitect AGESA entry points to remove need
	 *        to run amdinitreset, amdinitearly from bootblock.
	 *        Remove AP shortcut.
	 */
	if (!boot_cpu())
		bootblock_soc_early_init(); /* APs will not return */

	bootblock_main_with_timestamp(base_timestamp);
}

/* Set the MMIO Configuration Base Address and Bus Range. */
static void amd_initmmio(void)
{
	msr_t mmconf;
	msr_t mtrr_cap = rdmsr(MTRR_CAP_MSR);
	int mtrr;

	mmconf.hi = 0;
	mmconf.lo = CONFIG_MMCONF_BASE_ADDRESS | MMIO_RANGE_EN
			| fms(CONFIG_MMCONF_BUS_NUMBER) << MMIO_BUS_RANGE_SHIFT;
	wrmsr(MMIO_CONF_BASE, mmconf);

	/*
	 * todo: AGESA currently writes variable MTRRs.  Once that is
	 *       corrected, un-hardcode this MTRR.
	 */
	mtrr = (mtrr_cap.lo & MTRR_CAP_VCNT) - 2;
	set_var_mtrr(mtrr, FLASH_BASE_ADDR, CONFIG_ROM_SIZE, MTRR_TYPE_WRPROT);
}

void bootblock_soc_early_init(void)
{
	amd_initmmio();

	if (!boot_cpu())
		bootblock_soc_init(); /* APs will not return */

	bootblock_fch_early_init();

	post_code(0x90);
	if (CONFIG_STONEYRIDGE_UART)
		configure_stoneyridge_uart();
}

void bootblock_soc_init(void)
{
	if (IS_ENABLED(CONFIG_STONEYRIDGE_UART))
		assert(CONFIG_UART_FOR_CONSOLE >= 0
					&& CONFIG_UART_FOR_CONSOLE <= 1);

	u32 val = cpuid_eax(1);
	printk(BIOS_DEBUG, "Family_Model: %08x\n", val);

	post_code(0x37);
	AGESAWRAPPER(amdinitreset);

	post_code(0x38);
	AGESAWRAPPER(amdinitearly); /* APs will not exit amdinitearly */
}
