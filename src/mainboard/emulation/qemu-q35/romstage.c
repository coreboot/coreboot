/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Stefan Reinauer
 * Copyright (C) 2018 Patrick Rudolph <siro@das-labor.org>
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

#include <arch/cpu.h>
#include <stdint.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/intel/romstage.h>
#include <timestamp.h>
#include <southbridge/intel/i82801ix/i82801ix.h>
#include <program_loading.h>
#include <device/pci_ops.h>

#define D0F0_PCIEXBAR_LO 0x60

static void mainboard_machine_check(void)
{
	/* Check that MCFG is active. If it's not qemu was started for machine PC */
	if (!CONFIG(BOOTBLOCK_CONSOLE) &&
	    (pci_read_config32(PCI_DEV(0, 0, 0), D0F0_PCIEXBAR_LO) !=
	     (CONFIG_MMCONF_BASE_ADDRESS | 1)))
		die("You must run qemu for machine Q35 (-M q35)");
}

asmlinkage void car_stage_entry(void)
{
	struct postcar_frame pcf;
	i82801ix_early_init();
	console_init();

	mainboard_machine_check();

	cbmem_recovery(0);

	timestamp_add_now(TS_START_ROMSTAGE);

	if (postcar_frame_init(&pcf, 0))
		die("Unable to initialize postcar frame.\n");

	/**
	 * Run postcar to tear down CAR and load relocatable ramstage.
	 * There's no CAR on qemu, but for educational purposes and
	 * testing the postcar stage is used on qemu, too.
	 */

	run_postcar_phase(&pcf);
}
