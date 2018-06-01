/*
 * Copyright 2013 Google Inc.
 * Copyright 2017 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <rules.h>
#if ENV_RAMSTAGE
#define __SIMPLE_DEVICE__		1
#endif

#include <assert.h>
#include <commonlib/sdhci.h>
#include <device/pci.h>
#include "sd_mmc.h"
#include "storage.h"
#include <string.h>

/* Initialize an SDHCI port */
int sdhci_controller_init(struct sdhci_ctrlr *sdhci_ctrlr, void *ioaddr)
{
	memset(sdhci_ctrlr, 0, sizeof(*sdhci_ctrlr));
	sdhci_ctrlr->ioaddr = ioaddr;
	return add_sdhci(sdhci_ctrlr);
}

struct sd_mmc_ctrlr *new_mem_sdhci_controller(void *ioaddr)
{
	struct sdhci_ctrlr *sdhci_ctrlr;

	sdhci_ctrlr = malloc(sizeof(*sdhci_ctrlr));
	if (sdhci_ctrlr == NULL)
		return NULL;

	if (sdhci_controller_init(sdhci_ctrlr, ioaddr)) {
		free(sdhci_ctrlr);
		sdhci_ctrlr = NULL;
	}
	return &sdhci_ctrlr->sd_mmc_ctrlr;
}

struct sd_mmc_ctrlr *new_pci_sdhci_controller(uint32_t dev)
{
	uint32_t addr;

	addr = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	if (addr == ((uint32_t)~0)) {
		sdhc_error("Error: PCI SDHCI not found\n");
		return NULL;
	}

	addr &= ~0xf;
	return new_mem_sdhci_controller((void *)addr);
}
