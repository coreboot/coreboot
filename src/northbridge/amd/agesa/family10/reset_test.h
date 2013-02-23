/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * copy from src/northbridge/amd/amdfam10/reset_test.c
 */

#ifndef _RESET_TEST_H_
#define _RESET_TEST_H_

#include "amdfam10.h"	/* NODE_PCI */

#define NODE_ID			0x60
#define HT_INIT_CONTROL		0x6c
#define HTIC_ColdR_Detect	(1<<4)
#define HTIC_BIOSR_Detect	(1<<5)
#define HTIC_INIT_Detect	(1<<6)

static inline u32 warm_reset_detect(u8 nodeid)
{
	u32 htic;
	device_t device;
	device = NODE_PCI(nodeid, 0);
	htic = pci_io_read_config32(device, HT_INIT_CONTROL);
	return (htic & HTIC_ColdR_Detect) && !(htic & HTIC_BIOSR_Detect);
}

static inline void distinguish_cpu_resets(u8 nodeid)
{
	u32 htic;
	device_t device;
	device = NODE_PCI(nodeid, 0);
	htic = pci_io_read_config32(device, HT_INIT_CONTROL);
	htic |= HTIC_ColdR_Detect | HTIC_BIOSR_Detect | HTIC_INIT_Detect;
	pci_io_write_config32(device, HT_INIT_CONTROL, htic);
}

#endif
