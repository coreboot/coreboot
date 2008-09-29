/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Advanced Micro Devices, Inc.
 * Copyright (C) 2007 Stefan Reinauer
 * Copyright (C) 2008 Ronald G. Minnich <rminnich@gmail.com>
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
#include <mainboard.h>
#include <types.h>
#include <lib.h>
#include <console.h>
#include <cpu.h>
#include <globalvars.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <amd/k8/k8.h>
#include <mc146818rtc.h>
#include <spd.h>
#include <lapic.h>

/**
 * Stop all APs
 * @param bsp_apicid The BSP apic id, to make sure we don't send ourselves the stop
 */
void allow_all_aps_stop(unsigned bsp_apicid)
{
	lapic_write(LAPIC_MSG_REG, (bsp_apicid << 24) | 0x44);	// allow aps to stop
}

/**
 * stop_ap. Called from stage1. 
 */
void stop_ap(void)
{
	/* well, at time this is called, bsp_apicid *is* zero. It might change later.
	 * we need to figure this out. I don't know how. Global variable? 
	 */
	allow_all_aps_stop(0);
	post_code(POST_STAGE1_STOP_AP);
}

