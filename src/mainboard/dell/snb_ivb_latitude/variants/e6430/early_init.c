/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pci_ops.h>
#include <ec/dell/mec5035/mec5035.h>
#include <southbridge/intel/bd82x6x/pch.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 1, 0 },
	{ 1, 1, 0 },
	{ 1, 1, 1 },
	{ 1, 1, 1 },
	{ 1, 0, 2 },
	{ 1, 1, 2 },
	{ 1, 1, 3 },
	{ 1, 1, 3 },
	{ 1, 1, 4 },
	{ 1, 1, 4 },
	{ 1, 1, 5 },
	{ 1, 1, 5 },
	{ 1, 2, 6 },
	{ 1, 2, 6 },
};

void bootblock_mainboard_early_init(void)
{
	pci_write_config16(PCH_LPC_DEV, LPC_EN, CNF1_LPC_EN | MC_LPC_EN
			| KBC_LPC_EN | FDD_LPC_EN | LPT_LPC_EN
			| COMB_LPC_EN | COMA_LPC_EN);
	mec5035_early_init();
}
