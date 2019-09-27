/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015  Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/pci_ops.h>
#include "iomap.h"

/* Just re-define these instead of including x4x.h. It blows up romcc. */
#define D0F0_PCIEXBAR_LO 0x60
#define D0F0_PCIEXBAR_HI 0x64

static void bootblock_northbridge_init(void)
{
	uint32_t reg32;

	/* Disable LaGrande Technology (LT) */
	reg32 = TPM32(0);

	reg32 = CONFIG_MMCONF_BASE_ADDRESS | 16 | 1;
	pci_io_write_config32(PCI_DEV(0, 0, 0), D0F0_PCIEXBAR_LO, reg32);
}
