/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <superio/nuvoton/npcd378/npcd378.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <device/pci_ops.h>
#include <southbridge/intel/bd82x6x/pch.h>

void bootblock_mainboard_early_init(void)
{
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x82, 0x1408);
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x80, 0x0010);
}
