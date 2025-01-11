/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <southbridge/intel/common/pmbase.h>
#include <southbridge/intel/bd82x6x/pch.h>

void mainboard_pch_lpc_setup(void)
{
	u16 reg16;
	reg16 = pci_read_config16(PCI_DEV(0, 0x1f, 0), 0xa4);
	reg16 |= (1 << 13); // WOL Enable Override (WOL_EN_OVRD)
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0xa4, reg16);
}
