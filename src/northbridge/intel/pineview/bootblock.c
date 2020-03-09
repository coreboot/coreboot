/*
 * This file is part of the coreboot project.
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

#include <arch/bootblock.h>
#include <device/pci_ops.h>
#include "pineview.h"

#define MMCONF_256_BUSSES 16
#define ENABLE 1

void bootblock_early_northbridge_init(void)
{
	pci_io_write_config32(HOST_BRIDGE, PCIEXBAR,
		CONFIG_MMCONF_BASE_ADDRESS | MMCONF_256_BUSSES | ENABLE);
}
