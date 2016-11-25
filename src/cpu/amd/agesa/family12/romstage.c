/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

#include <arch/stages.h>

#include <console/console.h>
#include <cpu/amd/car.h>

#include <northbridge/amd/agesa/agesawrapper.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/agesa/agesa_helper.h>

#include "sb_cimx.h"
#include "SbPlatform.h"
#include "platform_cfg.h"

void asmlinkage early_all_cores(void)
{
	amd_initmmio();
}

void platform_once(struct sysinfo *cb)
{
	gpioEarlyInit();

	sb_poweron_init();

	board_BeforeAgesa(cb);
}

void agesa_main(struct sysinfo *cb)
{
	post_code(0x36);
	agesawrapper_amdinitreset();

	post_code(0x37);
	agesawrapper_amdinitearly();

	post_code(0x38);
	agesawrapper_amdinitpost();

	post_code(0x39);
	printk(BIOS_DEBUG, "sb_before_pci_init ");
	sb_before_pci_init();
	printk(BIOS_DEBUG, "passed.\n");

	post_code(0x40);
	agesawrapper_amdinitenv();

	post_code(0x43);
	copy_and_run();
}
