/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2017 Kyösti Mälkki
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

#include <lib.h>
#include <reset.h>
#include <arch/stages.h>
#include <cpu/amd/agesa/s3_resume.h>

#include <console/console.h>
#include <cpu/amd/car.h>

#include <northbridge/amd/agesa/agesawrapper.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/agesa/state_machine.h>

#include "northbridge/amd/agesa/family10/reset_test.h"
#include <nb_cimx.h>
#include <sb_cimx.h>

void asmlinkage early_all_cores(void)
{
	amd_initmmio();
}

void platform_once(struct sysinfo *cb)
{
	/*
	 * SR5650/5670/5690 RD890 chipset, read pci config space hang at POR,
	 * Disable all Pcie Bridges to work around It.
	 */
	sr56x0_rd890_disable_pcie_bridge();

	nb_Poweron_Init();

	sb_Poweron_Init();

	board_BeforeAgesa(cb);
}

void agesa_main(struct sysinfo *cb)
{
	post_code(0x37);
	agesawrapper_amdinitreset();

	post_code(0x3B);
	agesawrapper_amdinitearly();

	post_code(0x3C);

	nb_Ht_Init();
	post_code(0x3D);
	/* Reset for HT, FIDVID, PLL and ucode patch(errata) changes to take affect. */
	if (!warm_reset_detect(0)) {
		printk(BIOS_INFO, "...WARM RESET...\n\n\n");
		distinguish_cpu_resets(0);
		soft_reset();
		die("After soft_reset_x - shouldn't see this message!!!\n");
	}

	post_code(0x40);
	agesawrapper_amdinitpost();

	post_code(0x41);
	agesawrapper_amdinitenv();
	post_code(0x42);

	post_code(0x50);
	print_debug("Disabling cache as ram ");
	disable_cache_as_ram();
	print_debug("done\n");

	post_code(0x51);
	copy_and_run();

	/* Not reached */
}
