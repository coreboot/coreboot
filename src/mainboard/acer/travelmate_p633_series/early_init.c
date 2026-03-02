/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pci_ops.h>
#include <southbridge/intel/bd82x6x/pch.h>

void bootblock_mainboard_early_init(void)
{
	pci_write_config16(PCH_LPC_DEV, LPC_EN, 0x3f0f);
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x0010);
}
