/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <device/pci_ops.h>
#include <types.h>

#include "me.h"

#define PCH_LPC_DEV		PCI_DEV(0, 0x1f, 0)

#define ETR3			0xac
#define  ETR3_CWORWRE		(1 << 18)
#define  ETR3_CF9GR		(1 << 20)
#define  ETR3_CF9LOCK		(1 << 31)

void set_global_reset(const bool enable)
{
	u32 etr3 = pci_read_config32(PCH_LPC_DEV, ETR3);

	/* Clear CF9 Without Resume Well Reset Enable */
	etr3 &= ~ETR3_CWORWRE;

	/* CF9GR indicates a Global Reset */
	if (enable)
		etr3 |= ETR3_CF9GR;
	else
		etr3 &= ~ETR3_CF9GR;

	pci_write_config32(PCH_LPC_DEV, ETR3, etr3);
}
