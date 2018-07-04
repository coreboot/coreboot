/*
 * Copyright 2008, Freescale Semiconductor, Inc
 * Andy Fleming
 *
 * Copyright 2013 Google Inc.  All rights reserved.
 * Copyright 2017 Intel Corporation
 *
 * MultiMediaCard (MMC), eMMC and Secure Digital (SD) common initialization
 * code which brings the card into the standby state.  This code is controller
 * independent.
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

#include <assert.h>
#include <commonlib/storage.h>
#include <delay.h>
#include <endian.h>
#include "mmc.h"
#include "sd_mmc.h"
#include "storage.h"
#include <string.h>
#include <timer.h>

uint64_t sd_mmc_extract_uint32_bits(const uint32_t *array, int start, int count)
{
	int i;
	uint64_t value = 0;

	for (i = 0; i < count; i++, start++) {
		value <<= 1;
		value |= (array[start / 32] >> (31 - (start % 32))) & 0x1;
	}
	return value;
}

static uint32_t sd_mmc_calculate_transfer_speed(uint32_t csd0)
{
	uint32_t mult, freq;

	/* frequency bases, divided by 10 to be nice to platforms without
	 * floating point */
	static const int fbase[] = {
		10000,
		100000,
		1000000,
		10000000,
	};
	/* Multiplier values for TRAN_SPEED. Multiplied by 10 to be nice
	 * to platforms without floating point. */
	static const int multipliers[] = {
		0,  // reserved
		10,
		12,
		13,
		15,
		20,
		25,
		30,
		35,
		40,
		45,
		50,
		55,
		60,
		70,
		80,
	};

	/* divide frequency by 10, since the mults are 10x bigger */
	freq = fbase[csd0 & 0x7];
	mult = multipliers[(csd0 >> 3) & 0xf];
	return freq * mult;
}

int sd_mmc_go_idle(struct storage_media *media)
{
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;

	// Some cards can't accept idle commands without delay.
	if (ctrlr->mdelay_before_cmd0)
		mdelay(ctrlr->mdelay_before_cmd0);

	struct mmc_command cmd;
	cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
	cmd.cmdarg = 0;
	cmd.resp_type = CARD_RSP_NONE;
	cmd.flags = 0;

	int err = ctrlr->send_cmd(ctrlr, &cmd, NULL);
	if (err)
		return err;

	// Some cards need more than half second to respond to next command (ex,
	// SEND_OP_COND).
	if (ctrlr->mdelay_after_cmd0)
		mdelay(ctrlr->mdelay_after_cmd0);

	return 0;
}

int sd_mmc_send_status(struct storage_media *media, ssize_t tries)
{
	struct mmc_command cmd;
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;

	cmd.cmdidx = MMC_CMD_SEND_STATUS;
	cmd.resp_type = CARD_RSP_R1;
	cmd.cmdarg = media->rca << 16;
	cmd.flags = 0;

	while (tries--) {
		int err = ctrlr->send_cmd(ctrlr, &cmd, NULL);
		if (err)
			return err;
		else if (cmd.response[0] & MMC_STATUS_RDY_FOR_DATA)
			break;
		else if (cmd.response[0] & MMC_STATUS_MASK) {
			sd_mmc_error("Status Error: %#8.8x\n", cmd.response[0]);
			return CARD_COMM_ERR;
		}

		udelay(100);
	}

	sd_mmc_trace("CURR STATE:%d\n",
		  (cmd.response[0] & MMC_STATUS_CURR_STATE) >> 9);

	if (tries < 0) {
		sd_mmc_error("Timeout waiting card ready\n");
		return CARD_TIMEOUT;
	}
	return 0;
}

int sd_mmc_set_blocklen(struct sd_mmc_ctrlr *ctrlr, int len)
{
	struct mmc_command cmd;
	cmd.cmdidx = MMC_CMD_SET_BLOCKLEN;
	cmd.resp_type = CARD_RSP_R1;
	cmd.cmdarg = len;
	cmd.flags = 0;

	return ctrlr->send_cmd(ctrlr, &cmd, NULL);
}

int sd_mmc_enter_standby(struct storage_media *media)
{
	struct mmc_command cmd;
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;
	int err;

	SET_BUS_WIDTH(ctrlr, 1);
	SET_CLOCK(ctrlr, 1);

	/* Reset the Card */
	err = sd_mmc_go_idle(media);
	if (err)
		return err;

	/* Test for SD version 2 */
	err = CARD_TIMEOUT;
	if (IS_ENABLED(CONFIG_COMMONLIB_STORAGE_SD)) {
		err = sd_send_if_cond(media);

		/* Get SD card operating condition */
		if (!err)
			err = sd_send_op_cond(media);
	}

	/* If the command timed out, we check for an MMC card */
	if (IS_ENABLED(CONFIG_COMMONLIB_STORAGE_MMC) && (err == CARD_TIMEOUT)) {
		/* Some cards seem to need this */
		sd_mmc_go_idle(media);

		err = mmc_send_op_cond(media);
		if (err == CARD_IN_PROGRESS)
			err = mmc_complete_op_cond(media);
	}

	if (err) {
		sd_mmc_error(
			"Card did not respond to voltage select!\n");
		return CARD_UNUSABLE_ERR;
	}

	/* Put the Card in Identify Mode */
	cmd.cmdidx = MMC_CMD_ALL_SEND_CID;
	cmd.resp_type = CARD_RSP_R2;
	cmd.cmdarg = 0;
	cmd.flags = 0;
	err = ctrlr->send_cmd(ctrlr, &cmd, NULL);
	if (err)
		return err;
	memcpy(media->cid, cmd.response, sizeof(media->cid));

	/*
	 * For MMC cards, set the Relative Address.
	 * For SD cards, get the Relatvie Address.
	 * This also puts the cards into Standby State
	 */
	cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
	cmd.cmdarg = media->rca << 16;
	cmd.resp_type = CARD_RSP_R6;
	cmd.flags = 0;
	err = ctrlr->send_cmd(ctrlr, &cmd, NULL);
	if (err)
		return err;
	if (IS_SD(media))
		media->rca = (cmd.response[0] >> 16) & 0xffff;

	/* Get the Card-Specific Data */
	cmd.cmdidx = MMC_CMD_SEND_CSD;
	cmd.resp_type = CARD_RSP_R2;
	cmd.cmdarg = media->rca << 16;
	cmd.flags = 0;
	err = ctrlr->send_cmd(ctrlr, &cmd, NULL);

	/* Waiting for the ready status */
	sd_mmc_send_status(media, SD_MMC_IO_RETRIES);
	if (err)
		return err;

	memcpy(media->csd, cmd.response, sizeof(media->csd));
	if (media->version == MMC_VERSION_UNKNOWN) {
		int version = sd_mmc_extract_uint32_bits(media->csd, 2, 4);
		switch (version) {
		case 0:
			media->version = MMC_VERSION_1_2;
			break;
		case 1:
			media->version = MMC_VERSION_1_4;
			break;
		case 2:
			media->version = MMC_VERSION_2_2;
			break;
		case 3:
			media->version = MMC_VERSION_3;
			break;
		case 4:
			media->version = MMC_VERSION_4;
			break;
		default:
			media->version = MMC_VERSION_1_2;
			break;
		}
	}
	media->tran_speed = sd_mmc_calculate_transfer_speed(media->csd[0]);

	/* Determine the read and write block lengths */
	media->read_bl_len = 1 << sd_mmc_extract_uint32_bits(media->csd, 44, 4);
	if (IS_SD(media))
		media->write_bl_len = media->read_bl_len;
	else
		media->write_bl_len =
			1 << sd_mmc_extract_uint32_bits(media->csd, 102, 4);

	sd_mmc_debug("mmc media info: version=%#x, tran_speed=%d\n",
	      media->version, (int)media->tran_speed);

	return 0;
}
