/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/sdhci.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <stdint.h>
#include <string.h>

#include "sd_mmc.h"
#include "storage.h"

/* Initialize an SDHCI port */
int sdhci_controller_init(struct sdhci_ctrlr *sdhci_ctrlr, void *ioaddr)
{
	memset(sdhci_ctrlr, 0, sizeof(*sdhci_ctrlr));
	sdhci_ctrlr->ioaddr = ioaddr;
	return add_sdhci(sdhci_ctrlr);
}

struct sd_mmc_ctrlr *new_mem_sdhci_controller(void *ioaddr)
{
	static bool sdhci_init_done;
	static struct sdhci_ctrlr sdhci_ctrlr;

	if (sdhci_init_done == true) {
		sdhc_error("Error: SDHCI is already initialized.\n");
		return NULL;
	}

	if (sdhci_controller_init(&sdhci_ctrlr, ioaddr)) {
		sdhc_error("Error: SDHCI initialization failed.\n");
		return NULL;
	}

	sdhci_init_done = true;

	return &sdhci_ctrlr.sd_mmc_ctrlr;
}

struct sd_mmc_ctrlr *new_pci_sdhci_controller(pci_devfn_t dev)
{
	uintptr_t addr;

	addr = pci_s_read_config32(dev, PCI_BASE_ADDRESS_0);
	if (addr == ((uint32_t)~0)) {
		sdhc_error("Error: PCI SDHCI not found\n");
		return NULL;
	}

	addr &= ~0xf;
	return new_mem_sdhci_controller((void *)addr);
}
