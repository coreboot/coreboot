/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018, The Linux Foundation.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/cache.h>
#include <cbfs.h>
#include <console/console.h>
#include <soc/mmu.h>
#include <soc/aop.h>
#include <soc/clock.h>

void aop_fw_load_reset(void)
{
	bool aop_fw_entry;

	struct prog aop_fw_prog =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/aop");

	if (prog_locate(&aop_fw_prog))
		die("SOC image: AOP_FW not found");

	aop_fw_entry = selfload(&aop_fw_prog);
	if (!aop_fw_entry)
		die("SOC image: AOP load failed");

	clock_reset_aop();

	printk(BIOS_DEBUG, "\nSOC:AOP brought out of reset.\n");
}
