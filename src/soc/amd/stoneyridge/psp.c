/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <soc/pci_devs.h>
#include <soc/northbridge.h>
#include <soc/southbridge.h>
#include <amdblocks/psp.h>

void soc_enable_psp_early(void)
{
	u32 base, limit;
	u16 cmd;

	/* Open a posted hole from 0x80000000 : 0xfed00000-1 */
	base = (0x80000000 >> 8) | MMIO_WE | MMIO_RE;
	limit = (ALIGN_DOWN(HPET_BASE_ADDRESS - 1, 64 * KiB) >> 8);
	pci_write_config32(SOC_ADDR_DEV, D18F1_MMIO_LIMIT0_LO, limit);
	pci_write_config32(SOC_ADDR_DEV, D18F1_MMIO_BASE0_LO, base);

	/* Preload a value into BAR and enable it */
	pci_write_config32(SOC_PSP_DEV, PSP_MAILBOX_BAR, PSP_MAILBOX_BAR3_BASE);
	pci_write_config32(SOC_PSP_DEV, PSP_BAR_ENABLES, PSP_MAILBOX_BAR_EN);

	/* Enable memory access and master */
	cmd = pci_read_config16(SOC_PSP_DEV, PCI_COMMAND);
	cmd |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
	pci_write_config16(SOC_PSP_DEV, PCI_COMMAND, cmd);
};
