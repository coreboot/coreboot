/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pci_ops.h>
#include <southbridge/intel/bd82x6x/pch.h>

void bootblock_mainboard_early_init(void)
{
	pci_write_config16(PCH_LPC_DEV, LPC_EN, CNF2_LPC_EN | CNF1_LPC_EN
		| MC_LPC_EN | KBC_LPC_EN | GAMEH_LPC_EN | GAMEL_LPC_EN);
}
