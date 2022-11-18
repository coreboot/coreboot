/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Secure Digital (SD) Host Controller interface DMA support code
 */

#include <commonlib/sdhci.h>
#include <commonlib/storage.h>
#include <console/console.h>
#include <delay.h>
#include <endian.h>
#include <stdlib.h>
#include <string.h>

#include "sdhci.h"
#include "sd_mmc.h"
#include "storage.h"

static void sdhci_alloc_adma_descs(struct sdhci_ctrlr *sdhci_ctrlr,
	u32 need_descriptors)
{
	if (sdhci_ctrlr->adma_descs) {
		if (sdhci_ctrlr->adma_desc_count < need_descriptors) {
			/* Previously allocated array is too small */
			free(sdhci_ctrlr->adma_descs);
			sdhci_ctrlr->adma_desc_count = 0;
			sdhci_ctrlr->adma_descs = NULL;
		}
	}

	/* use dma_malloc() to make sure we get the coherent/uncached memory */
	if (!sdhci_ctrlr->adma_descs) {
		sdhci_ctrlr->adma_descs = malloc(need_descriptors
			* sizeof(*sdhci_ctrlr->adma_descs));
		if (sdhci_ctrlr->adma_descs == NULL)
			die("fail to malloc adma_descs\n");
		sdhci_ctrlr->adma_desc_count = need_descriptors;
	}

	memset(sdhci_ctrlr->adma_descs, 0, sizeof(*sdhci_ctrlr->adma_descs)
		* need_descriptors);
}

static void sdhci_alloc_adma64_descs(struct sdhci_ctrlr *sdhci_ctrlr,
	u32 need_descriptors)
{
	if (sdhci_ctrlr->adma64_descs) {
		if (sdhci_ctrlr->adma_desc_count < need_descriptors) {
			/* Previously allocated array is too small */
			free(sdhci_ctrlr->adma64_descs);
			sdhci_ctrlr->adma_desc_count = 0;
			sdhci_ctrlr->adma64_descs = NULL;
		}
	}

	/* use dma_malloc() to make sure we get the coherent/uncached memory */
	if (!sdhci_ctrlr->adma64_descs) {
		sdhci_ctrlr->adma64_descs = malloc(need_descriptors
			* sizeof(*sdhci_ctrlr->adma64_descs));
		if (sdhci_ctrlr->adma64_descs == NULL)
			die("fail to malloc adma64_descs\n");

		sdhci_ctrlr->adma_desc_count = need_descriptors;
	}

	memset(sdhci_ctrlr->adma64_descs, 0, sizeof(*sdhci_ctrlr->adma64_descs)
		* need_descriptors);
}

int sdhci_setup_adma(struct sdhci_ctrlr *sdhci_ctrlr, struct mmc_data *data)
{
	int i, togo, need_descriptors;
	int dma64;
	char *buffer_data;
	u16 attributes;

	togo = data->blocks * data->blocksize;
	if (!togo) {
		sdhc_error("%s: MmcData corrupted: %d blocks of %d bytes\n",
		       __func__, data->blocks, data->blocksize);
		return -1;
	}

	need_descriptors = 1 +  togo / SDHCI_MAX_PER_DESCRIPTOR;
	dma64 = sdhci_ctrlr->sd_mmc_ctrlr.caps & DRVR_CAP_DMA_64BIT;
	if (dma64)
		sdhci_alloc_adma64_descs(sdhci_ctrlr, need_descriptors);
	else
		sdhci_alloc_adma_descs(sdhci_ctrlr, need_descriptors);
	buffer_data = data->dest;

	/* Now set up the descriptor chain. */
	for (i = 0; togo; i++) {
		unsigned int desc_length;

		if (togo < SDHCI_MAX_PER_DESCRIPTOR)
			desc_length = togo;
		else
			desc_length = SDHCI_MAX_PER_DESCRIPTOR;
		togo -= desc_length;

		attributes = SDHCI_ADMA_VALID | SDHCI_ACT_TRAN;
		if (togo == 0)
			attributes |= SDHCI_ADMA_END;

		if (dma64) {
			sdhci_ctrlr->adma64_descs[i].addr =
				(uintptr_t)buffer_data;
			sdhci_ctrlr->adma64_descs[i].addr_hi = 0;
			sdhci_ctrlr->adma64_descs[i].length = desc_length;
			sdhci_ctrlr->adma64_descs[i].attributes = attributes;

		} else {
			sdhci_ctrlr->adma_descs[i].addr =
				(uintptr_t)buffer_data;
			sdhci_ctrlr->adma_descs[i].length = desc_length;
			sdhci_ctrlr->adma_descs[i].attributes = attributes;
		}

		buffer_data += desc_length;
	}

	if (dma64)
		sdhci_writel(sdhci_ctrlr, (uintptr_t)sdhci_ctrlr->adma64_descs,
			     SDHCI_ADMA_ADDRESS);
	else
		sdhci_writel(sdhci_ctrlr, (uintptr_t)sdhci_ctrlr->adma_descs,
			     SDHCI_ADMA_ADDRESS);

	return 0;
}

int sdhci_complete_adma(struct sdhci_ctrlr *sdhci_ctrlr,
	struct mmc_command *cmd)
{
	int retry;
	u32 stat = 0, mask;

	mask = SDHCI_INT_RESPONSE | SDHCI_INT_ERROR;

	retry = 10000; /* Command should be done in way less than 10 ms. */
	while (--retry) {
		stat = sdhci_readl(sdhci_ctrlr, SDHCI_INT_STATUS);
		if (stat & mask)
			break;
		udelay(1);
	}

	sdhci_writel(sdhci_ctrlr, SDHCI_INT_RESPONSE, SDHCI_INT_STATUS);

	if (retry && !(stat & SDHCI_INT_ERROR)) {
		/* Command OK, let's wait for data transfer completion. */
		mask = SDHCI_INT_DATA_END |
			SDHCI_INT_ERROR | SDHCI_INT_ADMA_ERROR;

		/* Transfer should take 10 seconds tops. */
		retry = 10 * 1000 * 1000;
		while (--retry) {
			stat = sdhci_readl(sdhci_ctrlr, SDHCI_INT_STATUS);
			if (stat & mask)
				break;
			udelay(1);
		}

		sdhci_writel(sdhci_ctrlr, stat, SDHCI_INT_STATUS);
		if (retry && !(stat & SDHCI_INT_ERROR)) {
			sdhci_cmd_done(sdhci_ctrlr, cmd);
			return 0;
		}
	}

	sdhc_error("%s: transfer error, stat %#x, adma error %#x, retry %d\n",
	       __func__, stat, sdhci_readl(sdhci_ctrlr, SDHCI_ADMA_ERROR),
		retry);

	sdhci_reset(sdhci_ctrlr, SDHCI_RESET_CMD);
	sdhci_reset(sdhci_ctrlr, SDHCI_RESET_DATA);

	if (stat & SDHCI_INT_TIMEOUT)
		return CARD_TIMEOUT;
	return CARD_COMM_ERR;
}
