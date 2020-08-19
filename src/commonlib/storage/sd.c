/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Secure Digital (SD) card specific support code
 * This code is controller independent
 */

#include <commonlib/sd_mmc_ctrlr.h>
#include <commonlib/storage.h>
#include <delay.h>
#include <endian.h>

#include "sd_mmc.h"
#include "storage.h"

int sd_send_if_cond(struct storage_media *media)
{
	struct mmc_command cmd;
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;

	cmd.cmdidx = SD_CMD_SEND_IF_COND;
	/* Set if controller supports voltages between 2.7 and 3.6 V. */
	cmd.cmdarg = ((ctrlr->voltages & 0xff8000) != 0) << 8 | 0xaa;
	cmd.resp_type = CARD_RSP_R7;
	cmd.flags = 0;
	int err = ctrlr->send_cmd(ctrlr, &cmd, NULL);
	if (err)
		return err;

	if ((cmd.response[0] & 0xff) != 0xaa)
		return CARD_UNUSABLE_ERR;
	media->version = SD_VERSION_2;
	return 0;
}

int sd_send_op_cond(struct storage_media *media)
{
	int err;
	struct mmc_command cmd;
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;

	int tries = SD_MMC_IO_RETRIES;
	while (tries--) {
		cmd.cmdidx = MMC_CMD_APP_CMD;
		cmd.resp_type = CARD_RSP_R1;
		cmd.cmdarg = 0;
		cmd.flags = 0;

		err = ctrlr->send_cmd(ctrlr, &cmd, NULL);
		if (err)
			return err;

		cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
		cmd.resp_type = CARD_RSP_R3;

		/*
		 * Most cards do not answer if some reserved bits
		 * in the ocr are set. However, Some controller
		 * can set bit 7 (reserved for low voltages), but
		 * how to manage low voltages SD card is not yet
		 * specified.
		 */
		cmd.cmdarg = (ctrlr->voltages & 0xff8000);

		if (media->version == SD_VERSION_2)
			cmd.cmdarg |= OCR_HCS;

		err = ctrlr->send_cmd(ctrlr, &cmd, NULL);
		if (err)
			return err;

		// OCR_BUSY means "initialization complete".
		if (cmd.response[0] & OCR_BUSY)
			break;

		udelay(100);
	}
	if (tries < 0)
		return CARD_UNUSABLE_ERR;

	if (media->version != SD_VERSION_2)
		media->version = SD_VERSION_1_0;

	media->ocr = cmd.response[0];
	media->high_capacity = ((media->ocr & OCR_HCS) == OCR_HCS);
	media->rca = 0;
	return 0;
}

static int sd_switch(struct sd_mmc_ctrlr *ctrlr, int mode, int group,
	uint8_t value, uint8_t *resp)
{
	/* Switch the frequency */
	struct mmc_command cmd;
	cmd.cmdidx = SD_CMD_SWITCH_FUNC;
	cmd.resp_type = CARD_RSP_R1;
	cmd.cmdarg = (mode << 31) | (0xffffff & ~(0xf << (group * 4))) |
		     (value << (group * 4));
	cmd.flags = 0;

	struct mmc_data data;
	data.dest = (char *)resp;
	data.blocksize = 64;
	data.blocks = 1;
	data.flags = DATA_FLAG_READ;

	return ctrlr->send_cmd(ctrlr, &cmd, &data);
}

static void sd_recalculate_clock(struct storage_media *media)
{
	uint32_t clock = 1;

	if (media->caps & DRVR_CAP_HS)
		clock = CLOCK_50MHZ;
	else
		clock = CLOCK_25MHZ;
	SET_CLOCK(media->ctrlr, clock);
}

int sd_change_freq(struct storage_media *media)
{
	int delay;
	int err, timeout;
	struct mmc_command cmd;
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;
	struct mmc_data data;
	ALLOC_CACHE_ALIGN_BUFFER(uint32_t, scr, 2);
	ALLOC_CACHE_ALIGN_BUFFER(uint32_t, switch_status, 16);

	media->caps = 0;

	/* Read the SCR to find out if this card supports higher speeds */
	cmd.cmdidx = MMC_CMD_APP_CMD;
	cmd.resp_type = CARD_RSP_R1;
	cmd.cmdarg = media->rca << 16;
	cmd.flags = 0;

	err = ctrlr->send_cmd(ctrlr, &cmd, NULL);
	if (err)
		return err;

	cmd.cmdidx = SD_CMD_APP_SEND_SCR;
	cmd.resp_type = CARD_RSP_R1;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	timeout = 3;
	while (timeout--) {
		data.dest = (char *)scr;
		data.blocksize = 8;
		data.blocks = 1;
		data.flags = DATA_FLAG_READ;
		err = ctrlr->send_cmd(ctrlr, &cmd, &data);
		if (!err)
			break;
	}
	if (err) {
		sd_mmc_error("%s returning %d\n", __func__, err);
		return err;
	}

	media->scr[0] = be32toh(scr[0]);
	media->scr[1] = be32toh(scr[1]);

	switch ((media->scr[0] >> 24) & 0xf) {
	case 0:
		media->version = SD_VERSION_1_0;
		break;
	case 1:
		media->version = SD_VERSION_1_10;
		break;
	case 2:
		media->version = SD_VERSION_2;
		break;
	default:
		media->version = SD_VERSION_1_0;
		break;
	}

	if (media->scr[0] & SD_DATA_4BIT)
		media->caps |= DRVR_CAP_4BIT;

	/* Version 1.0 doesn't support switching */
	if (media->version == SD_VERSION_1_0)
		goto out;

	timeout = 4;
	while (timeout--) {
		err = sd_switch(ctrlr, SD_SWITCH_CHECK, 0, 1,
				(uint8_t *)switch_status);
		if (err)
			return err;

		/* The high-speed function is busy.  Try again */
		if (!(ntohl(switch_status[7]) & SD_HIGHSPEED_BUSY))
			break;
	}

	/* If high-speed isn't supported, we return */
	if (!(ntohl(switch_status[3]) & SD_HIGHSPEED_SUPPORTED))
		goto out;

	/*
	 * If the controller doesn't support SD_HIGHSPEED, do not switch the
	 * card to HIGHSPEED mode even if the card support SD_HIGHSPPED.
	 * This can avoid a further problem when the card runs in different
	 * mode than the controller.
	 */
	if (!((ctrlr->caps & DRVR_CAP_HS52) && (ctrlr->caps & DRVR_CAP_HS)))
		goto out;

	/* Give the card time to recover after the switch operation.  Wait for
	 * 9 (>= 8) clock cycles receiving the switch status.
	 */
	delay = (9000000 + ctrlr->bus_hz - 1) / ctrlr->bus_hz;
	udelay(delay);

	/* Switch to high speed */
	err = sd_switch(ctrlr, SD_SWITCH_SWITCH, 0, 1,
			(uint8_t *)switch_status);
	if (err)
		return err;

	/* Give the card time to perform the switch operation.  Wait for 9
	 * (>= 8) clock cycles receiving the switch status.
	 */
	udelay(delay);

	if ((ntohl(switch_status[4]) & 0x0f000000) == 0x01000000) {
		media->caps |= DRVR_CAP_HS;
		SET_TIMING(ctrlr, BUS_TIMING_SD_HS);
	}

out:
	sd_recalculate_clock(media);
	return 0;
}

int sd_set_bus_width(struct storage_media *media)
{
	int err;
	struct mmc_command cmd;
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;

	if (media->caps & DRVR_CAP_4BIT) {
		cmd.cmdidx = MMC_CMD_APP_CMD;
		cmd.resp_type = CARD_RSP_R1;
		cmd.cmdarg = media->rca << 16;
		cmd.flags = 0;

		err = ctrlr->send_cmd(ctrlr, &cmd, NULL);
		if (err)
			return err;

		cmd.cmdidx = SD_CMD_APP_SET_BUS_WIDTH;
		cmd.resp_type = CARD_RSP_R1;
		cmd.cmdarg = 2;
		cmd.flags = 0;
		err = ctrlr->send_cmd(ctrlr, &cmd, NULL);
		if (err)
			return err;

		SET_BUS_WIDTH(ctrlr, 4);
	}
	return 0;
}

int sd_set_partition(struct storage_media *media,
	unsigned int partition_number)
{
	/* Validate the partition number */
	if (partition_number)
		return -1;

	/* Update the partition number */
	media->partition_config = partition_number;
	return 0;
}

const char *sd_partition_name(struct storage_media *media,
	unsigned int partition_number)
{
	return "";
}
