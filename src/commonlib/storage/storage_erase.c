/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * MultiMediaCard (MMC), eMMC and Secure Digital (SD) erase support code.
 * This code is controller independent.
 */

#include "sd_mmc.h"
#include "storage.h"

uint64_t storage_block_erase(struct storage_media *media, uint64_t start,
	uint64_t count)
{
	struct mmc_command cmd;
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;

	if (storage_block_setup(media, start, count, 0) == 0)
		return 0;

	cmd.cmdidx = MMC_CMD_ERASE_GROUP_START;
	cmd.resp_type = CARD_RSP_R1;
	cmd.cmdarg = start;
	cmd.flags = 0;

	if (ctrlr->send_cmd(ctrlr, &cmd, NULL))
		return 0;

	cmd.cmdidx = MMC_CMD_ERASE_GROUP_END;
	cmd.cmdarg = start + count - 1;
	cmd.resp_type = CARD_RSP_R1;
	cmd.flags = 0;

	if (ctrlr->send_cmd(ctrlr, &cmd, NULL))
		return 0;

	cmd.cmdidx = MMC_CMD_ERASE;
	cmd.cmdarg = MMC_TRIM_ARG;	/* just unmap blocks */
	cmd.resp_type = CARD_RSP_R1;
	cmd.flags = 0;

	if (ctrlr->send_cmd(ctrlr, &cmd, NULL))
		return 0;

	size_t erase_blocks;
	/*
	 * Timeout for TRIM operation on one erase group is defined as:
	 * TRIM timeout = 300ms x TRIM_MULT
	 *
	 * This timeout is expressed in units of 100us to sd_mmc_send_status.
	 *
	 * Hence, timeout_per_erase_block = TRIM timeout * 1000us/100us;
	 */
	size_t timeout_per_erase_block = (media->trim_mult * 300) * 10;
	int err = 0;

	erase_blocks = ALIGN_UP(count, media->erase_blocks)
		/ media->erase_blocks;

	while (erase_blocks) {
		/*
		 * To avoid overflow of timeout value, loop in calls to
		 * sd_mmc_send_status for erase_blocks number of times.
		 */
		err = sd_mmc_send_status(media, timeout_per_erase_block);

		/* Send status successful, erase action complete. */
		if (err == 0)
			break;

		erase_blocks--;
	}

	/* Total timeout done. Still status not successful. */
	if (err) {
		sd_mmc_error("TRIM operation not successful within timeout.\n");
		return 0;
	}

	return count;
}
