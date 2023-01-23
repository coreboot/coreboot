/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci.h>
#include <intelblocks/cfg.h>
#include <soc/lockdown.h>
#include <soc/pci_devs.h>

void sata_lockdown_config(int chipset_lockdown)
{
	if (chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT) {
		pci_or_config32(PCH_DEV_SATA, SATAGC, SATAGC_REGLOCK);
		pci_or_config32(PCH_DEV_SSATA, SATAGC, SATAGC_REGLOCK);
	}
}

void spi_lockdown_config(int chipset_lockdown)
{
}
