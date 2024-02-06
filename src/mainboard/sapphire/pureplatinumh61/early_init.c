/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <southbridge/intel/bd82x6x/pch.h>

void mainboard_pch_lpc_setup(void)
{
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0xac, 0x00010000);
}
