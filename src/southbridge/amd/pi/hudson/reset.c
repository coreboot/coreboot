/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <device/pci_ops.h>
#include <cf9_reset.h>
#include <reset.h>

#define HT_INIT_CONTROL     0x6c
#define HTIC_ColdR_Detect  (1<<4)
#define HTIC_BIOSR_Detect  (1<<5)
#define HTIC_INIT_Detect   (1<<6)

void cf9_reset_prepare(void)
{
	u32 htic;
	htic = pci_s_read_config32(PCI_DEV(0, 0x18, 0), HT_INIT_CONTROL);
	htic &= ~HTIC_BIOSR_Detect;
	pci_s_write_config32(PCI_DEV(0, 0x18, 0), HT_INIT_CONTROL, htic);
}

void do_board_reset(void)
{
	system_reset();
}
