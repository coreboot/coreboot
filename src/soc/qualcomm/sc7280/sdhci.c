/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/sdhci.h>
#include <commonlib/storage.h>
#include <commonlib/storage/sd_mmc.h>
#include <commonlib/storage/sdhci.h>
#include <commonlib/sd_mmc_ctrlr.h>
#include <soc/addressmap.h>
#include <soc/sdhci.h>
#include <soc/sdhci_msm.h>

int qc_emmc_early_init(void)
{
	struct sd_mmc_ctrlr *ctrlr;
	struct storage_media media = {0};

	ctrlr = new_sdhci_msm_host((void *)(uintptr_t)SDC1_HC_BASE);
	if (ctrlr == NULL)
		return -1;

	media.ctrlr = ctrlr;
	SET_BUS_WIDTH(ctrlr, 1);
	SET_CLOCK(ctrlr, 384 * 1000);

	/* Send CMD1 */
	return mmc_send_cmd1(&media);
}
