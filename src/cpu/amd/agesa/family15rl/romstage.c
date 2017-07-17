/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#include <console/console.h>
#include <cpu/amd/car.h>

#include <northbridge/amd/agesa/agesawrapper.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/agesa/agesa_helper.h>

void platform_once(struct sysinfo *cb)
{
	board_BeforeAgesa(cb);
}

void agesa_main(struct sysinfo *cb)
{
	post_code(0x37);
	agesawrapper_amdinitreset();

	post_code(0x39);
	agesawrapper_amdinitearly();

	if (!cb->s3resume) {
		printk(BIOS_INFO, "Normal boot\n");

		post_code(0x40);
		agesawrapper_amdinitpost();
	} else {
		printk(BIOS_INFO, "S3 detected\n");

		post_code(0x60);
		agesawrapper_amdinitresume();
	}

}
void agesa_postcar(struct sysinfo *cb)
{
	if (!cb->s3resume) {
		printk(BIOS_INFO, "Normal boot postcar\n");

		post_code(0x41);
		agesawrapper_amdinitenv();
	} else {
		printk(BIOS_INFO, "S3 resume postcar\n");

		post_code(0x61);
		amd_initcpuio();

		post_code(0x62);
		agesawrapper_amds3laterestore();
	}
}
