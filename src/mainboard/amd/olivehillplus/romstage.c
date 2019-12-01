/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
#include <amdblocks/acpimmio.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <southbridge/amd/pi/hudson/hudson.h>

static void romstage_main_template(void)
{
	u32 val;

	/*
	 *  In Hudson RRG, PMIOxD2[5:4] is "Drive strength control for
	 *  LpcClk[1:0]".  This following register setting has been
	 *  replicated in every reference design since Parmer, so it is
	 *  believed to be required even though it is not documented in
	 *  the SoC BKDGs.  Without this setting, there is no serial
	 *  output.
	 */
	pm_io_write8(0xd2, 0);

	if (!cpu_init_detectedx && boot_cpu()) {
		post_code(0x30);

		post_code(0x31);
		console_init();
	}

	/* Load MPB */
	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x\n", val);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx\n", cpu_init_detectedx);

	/*
	 * This refers to LpcClkDrvSth settling time.  Without this setting, processor
	 * initialization is slow or incorrect, so this wait has been replicated from
	 * earlier development boards.
	 */
	{
		int i;
		for (i = 0; i < 200000; i++)
			inb(0xCD6);
	}
}

void agesa_postcar(struct sysinfo *cb)
{
	/* After AMD_INIT_ENV -> move to ramstage ? */
	pm_io_write8(0xea, 1);
}
