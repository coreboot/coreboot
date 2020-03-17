/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
