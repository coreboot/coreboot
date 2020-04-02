/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <commonlib/sdhci.h>
#include <commonlib/storage.h>

void soc_sd_mmc_controller_quirks(struct sd_mmc_ctrlr *ctrlr)
{
	/* Specify the additional driver support */
	ctrlr->caps |= DRVR_CAP_REMOVABLE;

	/* ADMA currently not working on Quark */
	ctrlr->caps &= ~DRVR_CAP_AUTO_CMD12;

	/* Set initialization clock frequency */
	ctrlr->f_min = 100 * CLOCK_KHZ;

	/* Set the initialization delays */
	ctrlr->mdelay_before_cmd0 = 1;
	ctrlr->mdelay_after_cmd0 = 2;
}
