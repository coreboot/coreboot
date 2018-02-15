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
#include <soc/northbridge.h>
#include <soc/southbridge.h>
#include <amdblocks/psp.h>
#include <timestamp.h>

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

	/* TSC cannot be relied upon. Override the TSC value passed in. */
	bootblock_main_with_timestamp(timestamp_get());
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

/*
 * This step is in bootblock because the SMU FW1 must be loaded prior to
 * issuing any reset to the system.  Set up just enough to get the command
 * to the PSP.  A side effect of placing this step here is we will always
 * load a RO version of FW1 and never a RW version.
 *
 * todo: If AMD develops a more robust methodology, move this function to
 *       romstage.
 */
static void load_smu_fw1(void)
{
	u32 base, limit, cmd;

	/* Open a posted hole from 0x80000000 : 0xfed00000-1 */
	base = (0x80000000 >> 8) | MMIO_WE | MMIO_RE;
	limit = (ALIGN_DOWN(HPET_BASE_ADDRESS - 1, 64 * KiB) >> 8);
	pci_write_config32(SOC_ADDR_DEV, D18F1_MMIO_LIMIT0_LO, limit);
	pci_write_config32(SOC_ADDR_DEV, D18F1_MMIO_BASE0_LO, base);

	/* Preload a value into "BAR3" and enable it */
	pci_write_config32(SOC_PSP_DEV, PSP_MAILBOX_BAR, PSP_MAILBOX_BAR3_BASE);
	pci_write_config32(SOC_PSP_DEV, PSP_BAR_ENABLES, PSP_MAILBOX_BAR_EN);

	/* Enable memory access and master */
	cmd = pci_read_config32(SOC_PSP_DEV, PCI_COMMAND);
	cmd |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
	pci_write_config32(SOC_PSP_DEV, PCI_COMMAND, cmd);

	psp_load_named_blob(MBOX_BIOS_CMD_SMU_FW, "smu_fw");
}

void bootblock_soc_init(void)
{
	if (IS_ENABLED(CONFIG_STONEYRIDGE_UART))
		assert(CONFIG_UART_FOR_CONSOLE >= 0
					&& CONFIG_UART_FOR_CONSOLE <= 1);

	u32 val = cpuid_eax(1);
	printk(BIOS_DEBUG, "Family_Model: %08x\n", val);

	if (boot_cpu() && IS_ENABLED(CONFIG_SOC_AMD_PSP_SELECTABLE_SMU_FW))
		load_smu_fw1();

	post_code(0x37);
	do_agesawrapper(agesawrapper_amdinitreset, "amdinitreset");

	post_code(0x38);
	/* APs will not exit amdinitearly */
	do_agesawrapper(agesawrapper_amdinitearly, "amdinitearly");

	/* Initialize any early i2c buses. */
	i2c_soc_early_init();
}
