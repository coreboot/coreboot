/*
 * Copyright 2011, Marvell Semiconductor Inc.
 * Lei Wen <leiwen@marvell.com>
 *
 * Copyright 2017 Intel Corporation
 *
 * SD host controller specific definitions
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __DEVICE_SDHCI_H__
#define __DEVICE_SDHCI_H__

#include <device/sd_mmc_ctrlr.h>

/* Driver specific capabilities */
#define DRVR_CAP_1V8_VDD			0x00010000
#define DRVR_CAP_32BIT_DMA_ADDR			0x00020000
#define DRVR_CAP_BROKEN_R1B			0x00040000
#define DRVR_CAP_NO_CD				0x00080000
#define DRVR_CAP_NO_HISPD_BIT			0x00100000
#define DRVR_CAP_NO_SIMULT_VDD_AND_POWER	0x00200000
#define DRVR_CAP_REG32_RW			0x00400000
#define DRVR_CAP_SPI				0x00800000
#define DRVR_CAP_WAIT_SEND_CMD			0x01000000

/* ADMA packet descriptor */
struct sdhci_adma {
	u16     attributes;
	u16     length;
	u32     addr;
};

struct sdhci_adma64 {
	u16     attributes;
	u16     length;
	u32     addr;
	u32     addr_hi;
};

struct sdhci_ctrlr {
	struct sd_mmc_ctrlr sd_mmc_ctrlr;
	void *ioaddr;
	uint32_t b_max;

	/*
	 * Dynamically allocated array of ADMA descriptors to use for data
	 * transfers
	 */
	struct sdhci_adma *adma_descs;
	struct sdhci_adma64 *adma64_descs;

	/* Number of ADMA descriptors currently in the array. */
	int adma_desc_count;
};

int add_sdhci(struct sdhci_ctrlr *sdhci_ctrlr);
int sdhci_controller_init(struct sdhci_ctrlr *sdhci_ctrlr, void *ioaddr);
void sdhci_update_pointers(struct sdhci_ctrlr *sdhci_ctrlr);
void sdhci_display_setup(struct sdhci_ctrlr *sdhci_ctrlr);

/* Add SDHCI controller from PCI */
struct sd_mmc_ctrlr *new_pci_sdhci_controller(uint32_t dev);

/* Add SDHCI controller with memory address */
struct sd_mmc_ctrlr *new_mem_sdhci_controller(void *ioaddr);

#endif /* __DEVICE_SDHCI_H__ */
