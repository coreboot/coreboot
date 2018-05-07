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
#include <amdblocks/agesawrapper.h>
#include <amdblocks/agesawrapper_call.h>
#include <soc/pci_devs.h>
#include <soc/cpu.h>
#include <soc/northbridge.h>
#include <soc/southbridge.h>
#include <amdblocks/psp.h>
#include <timestamp.h>
#include <halt.h>

#if CONFIG_PI_AGESA_TEMP_RAM_BASE < 0x100000
#error "Error: CONFIG_PI_AGESA_TEMP_RAM_BASE must be >= 1MB"
#endif
#if CONFIG_PI_AGESA_CAR_HEAP_BASE < 0x100000
#error "Error: CONFIG_PI_AGESA_CAR_HEAP_BASE must be >= 1MB"
#endif

/* Set the MMIO Configuration Base Address, Bus Range, and misc MTRRs. */
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
	 *
	 *       Be careful not to use get_free_var_mtrr/set_var_mtrr pairs
	 *       where all cores execute the path.  Both cores within a compute
	 *       unit share MTRRs.  Programming core0 has the appearance of
	 *       modifying core1 too.  Using the pair again will create
	 *       duplicate copies.
	 */
	mtrr = (mtrr_cap.lo & MTRR_CAP_VCNT) - SOC_EARLY_VMTRR_FLASH;
	set_var_mtrr(mtrr, FLASH_BASE_ADDR, CONFIG_ROM_SIZE, MTRR_TYPE_WRPROT);

	mtrr = (mtrr_cap.lo & MTRR_CAP_VCNT) - SOC_EARLY_VMTRR_CAR_HEAP;
	set_var_mtrr(mtrr, CONFIG_PI_AGESA_CAR_HEAP_BASE,
			CONFIG_PI_AGESA_HEAP_SIZE, MTRR_TYPE_WRBACK);

	mtrr = (mtrr_cap.lo & MTRR_CAP_VCNT) - SOC_EARLY_VMTRR_TEMPRAM;
	set_var_mtrr(mtrr, CONFIG_PI_AGESA_TEMP_RAM_BASE,
			CONFIG_PI_AGESA_HEAP_SIZE, MTRR_TYPE_UNCACHEABLE);
}

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	amd_initmmio();
	/*
	 * Call lib/bootblock.c main with BSP, shortcut for APs
	 */
	if (!boot_cpu()) {
		void (*ap_romstage_entry)(void) =
				(void (*)(void))get_ap_entry_ptr();

		ap_romstage_entry(); /* execution does not return */
		halt();
	}

	/* TSC cannot be relied upon. Override the TSC value passed in. */
	bootblock_main_with_timestamp(timestamp_get(), NULL, 0);
}

void bootblock_soc_early_init(void)
{
	bootblock_fch_early_init();
	post_code(0x90);
}

void bootblock_soc_init(void)
{
	if (IS_ENABLED(CONFIG_STONEYRIDGE_UART))
		assert(CONFIG_UART_FOR_CONSOLE >= 0
					&& CONFIG_UART_FOR_CONSOLE <= 1);

	u32 val = cpuid_eax(1);
	printk(BIOS_DEBUG, "Family_Model: %08x\n", val);

	/* Initialize any early i2c buses. */
	i2c_soc_early_init();
}
