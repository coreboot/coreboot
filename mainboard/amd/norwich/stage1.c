/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <amd_geodelx.h>
#include <southbridge/amd/cs5536/cs5536.h>

void hardware_stage1(void)
{
	post_code(POST_START_OF_MAIN);

	geodelx_msr_init();

	cs5536_stage1();

	/*
	 * NOTE: Must do this AFTER the early_setup! It is counting on some
	 * early MSR setup for the CS5536. We do this early for debug. 
	 * Real setup should be done in chipset init via dts settings.
	 */
	cs5536_setup_onchipuart(1);
}

void mainboard_pre_payload(void)
{
	geode_pre_payload();
	banner(BIOS_DEBUG, "mainboard_pre_payload: done");
}
