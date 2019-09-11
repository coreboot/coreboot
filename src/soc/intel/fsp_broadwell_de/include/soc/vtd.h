/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Facebook, Inc.
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

#ifndef _BROADWELL_VTD_H_
#define _BROADWELL_VTD_H_

#include <device/pci_ops.h>
#include <soc/pci_devs.h>

#define VTD_CPUBUSNO                      0x108
#define   VTD_CPUBUSNO_BUS0_MASK          0xff
#define   VTD_CPUBUSNO_BUS0_SHIFT         0
#define   VTD_CPUBUSNO_BUS1_MASK          0xff
#define   VTD_CPUBUSNO_BUS1_SHIFT         8
#define   VTD_CPUBUSNO_ISVALID            (1u << 16)

#define VTD_DFX1                          0x804
#define   VTD_DFX1_RANGE_3F8_DISABLE      (1u << 29)
#define   VTD_DFX1_RANGE_2F8_DISABLE      (1u << 30)

static inline uint8_t get_busno1(void)
{
	uint32_t reg32;

	/* Figure out what bus number is assigned for CPUBUSNO(1) */
	reg32 = pci_mmio_read_config32(VTD_PCI_DEV, VTD_CPUBUSNO);
	return ((reg32 >> VTD_CPUBUSNO_BUS1_SHIFT) & VTD_CPUBUSNO_BUS1_MASK);
}

#endif
