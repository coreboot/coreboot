/* SPDX-License-Identifier: GPL-2.0-only */

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
