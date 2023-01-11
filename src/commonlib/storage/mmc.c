/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * MultiMediaCard (MMC) and eMMC specific support code
 * This code is controller independent
 */

#include <cbmem.h>
#include <commonlib/storage.h>
#include <delay.h>
#include "mmc.h"
#include "sd_mmc.h"
#include "storage.h"
#include <string.h>
#include <timer.h>

/* We pass in the cmd since otherwise the init seems to fail */
static int mmc_send_op_cond_iter(struct storage_media *media,
	struct mmc_command *cmd, int use_arg)
{
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;

	cmd->cmdidx = MMC_CMD_SEND_OP_COND;
	cmd->resp_type = CARD_RSP_R3;

	/* Set the controller's operating conditions */
	if (use_arg) {
		uint32_t mask = media->op_cond_response &
			(OCR_VOLTAGE_MASK | OCR_ACCESS_MODE);
		cmd->cmdarg = ctrlr->voltages & mask;

		/* Always request high capacity if supported by the
		 * controller
		 */
		if (ctrlr->caps & DRVR_CAP_HC)
			cmd->cmdarg |= OCR_HCS;
	}
	cmd->flags = 0;
	int err = ctrlr->send_cmd(ctrlr, cmd, NULL);
	if (err)
		return err;

	media->op_cond_response = cmd->response[0];
	return 0;
}

int mmc_send_op_cond(struct storage_media *media)
{
	struct mmc_command cmd;
	int max_iters = 2;

	/* Ask the card for its operating conditions */
	cmd.cmdarg = 0;
	for (int i = 0; i < max_iters; i++) {
		int err = mmc_send_op_cond_iter(media, &cmd, i != 0);
		if (err)
			return err;

		// OCR_BUSY is active low, this bit set means
		// "initialization complete".
		if (media->op_cond_response & OCR_BUSY)
			return 0;
	}
	return CARD_IN_PROGRESS;
}

int mmc_complete_op_cond(struct storage_media *media)
{
	struct mmc_command cmd;
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, MMC_INIT_TIMEOUT_US_MS);
	while (1) {
		// CMD1 queries whether initialization is done.
		int err = mmc_send_op_cond_iter(media, &cmd, 1);
		if (err)
			return err;

		// OCR_BUSY means "initialization complete".
		if (media->op_cond_response & OCR_BUSY)
			break;

		// Check if init timeout has expired.
		if (stopwatch_expired(&sw))
			return CARD_UNUSABLE_ERR;

		udelay(100);
	}

	media->version = MMC_VERSION_UNKNOWN;
	media->ocr = cmd.response[0];

	media->high_capacity = ((media->ocr & OCR_HCS) == OCR_HCS);
	media->rca = 0;
	return 0;
}

int mmc_send_ext_csd(struct sd_mmc_ctrlr *ctrlr, unsigned char *ext_csd)
{
	struct mmc_command cmd;
	struct mmc_data data;
	int rv;

	/* Get the Card Status Register */
	cmd.cmdidx = MMC_CMD_SEND_EXT_CSD;
	cmd.resp_type = CARD_RSP_R1;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	data.dest = (char *)ext_csd;
	data.blocks = 1;
	data.blocksize = 512;
	data.flags = DATA_FLAG_READ;

	rv = ctrlr->send_cmd(ctrlr, &cmd, &data);

	if (!rv && CONFIG(SD_MMC_TRACE)) {
		int i, size;

		size = data.blocks * data.blocksize;
		sd_mmc_trace("\t%p ext_csd:", ctrlr);
		for (i = 0; i < size; i++) {
			if (!(i % 32))
				sd_mmc_trace("\n");
			sd_mmc_trace(" %2.2x", ext_csd[i]);
		}
		sd_mmc_trace("\n");
	}
	return rv;
}

static int mmc_switch(struct storage_media *media, uint8_t index, uint8_t value)
{
	struct mmc_command cmd;
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;

	cmd.cmdidx = MMC_CMD_SWITCH;
	cmd.resp_type = CARD_RSP_R1b;
	cmd.cmdarg = ((MMC_SWITCH_MODE_WRITE_BYTE << 24) |
			   (index << 16) | (value << 8));
	cmd.flags = 0;

	int ret = ctrlr->send_cmd(ctrlr, &cmd, NULL);

	/* Waiting for the ready status */
	sd_mmc_send_status(media, SD_MMC_IO_RETRIES);
	return ret;

}

static void mmc_recalculate_clock(struct storage_media *media)
{
	uint32_t clock;

	clock = CLOCK_26MHZ;
	if (media->caps & DRVR_CAP_HS) {
		if ((media->caps & DRVR_CAP_HS200) ||
		    (media->caps & DRVR_CAP_HS400))
			clock = CLOCK_200MHZ;
		else if (media->caps & DRVR_CAP_HS52)
			clock = CLOCK_52MHZ;
	}
	SET_CLOCK(media->ctrlr, clock);
}

static int mmc_select_hs(struct storage_media *media)
{
	int ret;

	/* Switch the MMC device into high speed mode */
	ret = mmc_switch(media, EXT_CSD_HS_TIMING, EXT_CSD_TIMING_HS);
	if (ret) {
		sd_mmc_error("Timing switch to high speed failed\n");
		return ret;
	}
	sdhc_debug("SDHCI switched MMC to high speed\n");

	/* Increase the controller clock speed */
	SET_TIMING(media->ctrlr, BUS_TIMING_MMC_HS);
	media->caps &= ~(DRVR_CAP_HS200 | DRVR_CAP_HS400);
	media->caps |= DRVR_CAP_HS52 | DRVR_CAP_HS;
	mmc_recalculate_clock(media);
	ret = sd_mmc_send_status(media, SD_MMC_IO_RETRIES);
	return ret;
}

static int mmc_send_tuning_seq(struct sd_mmc_ctrlr *ctrlr, char *buffer)
{
	struct mmc_command cmd;
	struct mmc_data data;

	/* Request the device send the tuning sequence to the host */
	cmd.cmdidx = MMC_CMD_AUTO_TUNING_SEQUENCE;
	cmd.resp_type = CARD_RSP_R1;
	cmd.cmdarg = 0;
	cmd.flags = CMD_FLAG_IGNORE_INHIBIT;

	data.dest = buffer;
	data.blocks = 1;
	data.blocksize = (ctrlr->bus_width == 8) ? 128 : 64;
	data.flags = DATA_FLAG_READ;
	return ctrlr->send_cmd(ctrlr, &cmd, &data);
}

static int mmc_bus_tuning(struct storage_media *media)
{
	ALLOC_CACHE_ALIGN_BUFFER(char, buffer, 128);
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;
	int index;
	int successful;

	/* Request the device send the tuning sequence up to 40 times */
	ctrlr->tuning_start(ctrlr, 0);
	for (index = 0; index < 40; index++) {
		mmc_send_tuning_seq(ctrlr, buffer);
		if (ctrlr->is_tuning_complete(ctrlr, &successful)) {
			if (successful)
				return 0;
			break;
		}
	}
	sd_mmc_error("Bus tuning failed!\n");
	return -1;
}

static int mmc_select_hs400(struct storage_media *media)
{
	uint8_t bus_width;
	uint32_t caps;
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;
	int ret;
	uint32_t timing;

	/* Switch the MMC device into high speed mode */
	ret = mmc_select_hs(media);
	if (ret)
		return ret;

	/* Switch MMC device to 8-bit DDR with strobe */
	bus_width = EXT_CSD_DDR_BUS_WIDTH_8;
	caps = DRVR_CAP_HS400 | DRVR_CAP_HS52 | DRVR_CAP_HS;
	timing = BUS_TIMING_MMC_HS400;
	if ((ctrlr->caps & DRVR_CAP_ENHANCED_STROBE)
		&& (media->caps & DRVR_CAP_ENHANCED_STROBE)) {
		bus_width |= EXT_CSD_BUS_WIDTH_STROBE;
		caps |= DRVR_CAP_ENHANCED_STROBE;
		timing = BUS_TIMING_MMC_HS400ES;
	}
	ret = mmc_switch(media, EXT_CSD_BUS_WIDTH, bus_width);
	if (ret) {
		sd_mmc_error("Switching bus width for HS400 failed\n");
		return ret;
	}
	sdhc_debug("SDHCI switched MMC to 8-bit DDR\n");

	/* Set controller to 8-bit mode */
	SET_BUS_WIDTH(ctrlr, 8);
	media->caps |= EXT_CSD_BUS_WIDTH_8;

	/* Switch MMC device to HS400 */
	ret = mmc_switch(media, EXT_CSD_HS_TIMING, EXT_CSD_TIMING_HS400);
	if (ret) {
		sd_mmc_error("Switch to HS400 timing failed\n");
		return ret;
	}

	/* Set controller to 200 MHz and use receive strobe */
	SET_TIMING(ctrlr, timing);
	media->caps |= caps;
	mmc_recalculate_clock(media);
	ret = sd_mmc_send_status(media, SD_MMC_IO_RETRIES);
	return ret;
}

static int mmc_select_hs200(struct storage_media *media)
{
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;
	int ret;

	/* Switch the MMC device to 8-bit SDR */
	ret = mmc_switch(media, EXT_CSD_BUS_WIDTH, EXT_CSD_BUS_WIDTH_8);
	if (ret) {
		sd_mmc_error("Switching bus width for HS200 failed\n");
		return ret;
	}

	/* Set controller to 8-bit mode */
	SET_BUS_WIDTH(ctrlr, 8);
	media->caps |= EXT_CSD_BUS_WIDTH_8;

	/* Switch to HS200 */
	ret = mmc_switch(media, EXT_CSD_HS_TIMING, EXT_CSD_TIMING_HS200);

	if (ret) {
		sd_mmc_error("Switch to HS200 failed\n");
		return ret;
	}
	sdhc_debug("SDHCI switched MMC to 8-bit SDR\n");

	/* Set controller to 200 MHz */
	SET_TIMING(ctrlr, BUS_TIMING_MMC_HS200);
	media->caps |= DRVR_CAP_HS200 | DRVR_CAP_HS52 | DRVR_CAP_HS;
	mmc_recalculate_clock(media);

	/* Tune the receive sampling point for the bus */
	if ((!ret) && (ctrlr->caps & DRVR_CAP_HS200_TUNING))
		ret = mmc_bus_tuning(media);
	return ret;
}

int mmc_change_freq(struct storage_media *media)
{
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;
	int err;
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, ext_csd, 512);

	media->caps = 0;

	/* Only version 4 supports high-speed */
	if (media->version < MMC_VERSION_4)
		return 0;

	err = mmc_send_ext_csd(ctrlr, ext_csd);
	if (err)
		return err;

	/* Determine if the device supports enhanced strobe */
	media->caps |= ext_csd[EXT_CSD_STROBE_SUPPORT]
		? DRVR_CAP_ENHANCED_STROBE : 0;

	if ((ctrlr->caps & DRVR_CAP_HS400) &&
	    (ext_csd[EXT_CSD_CARD_TYPE] & MMC_HS400))
		err = mmc_select_hs400(media);
	else if ((ctrlr->caps & DRVR_CAP_HS200) &&
		 (ext_csd[EXT_CSD_CARD_TYPE] & MMC_HS_200MHZ))
		err = mmc_select_hs200(media);
	else
		err = mmc_select_hs(media);

	return err;
}

int mmc_set_bus_width(struct storage_media *media)
{
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;
	int err;
	int width;

	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, ext_csd, EXT_CSD_SIZE);
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, test_csd, EXT_CSD_SIZE);

	/* Set the bus width */
	err = 0;
	for (width = EXT_CSD_BUS_WIDTH_8; width >= 0; width--) {
		/* If HS200 is switched, Bus Width has been 8-bit */
		if ((media->caps & DRVR_CAP_HS200) ||
		    (media->caps & DRVR_CAP_HS400))
			break;

		/* Set the card to use 4 bit*/
		err = mmc_switch(media, EXT_CSD_BUS_WIDTH, width);
		if (err)
			continue;

		if (!width) {
			SET_BUS_WIDTH(ctrlr, 1);
			break;
		}
		SET_BUS_WIDTH(ctrlr, 4 * width);

		err = mmc_send_ext_csd(ctrlr, test_csd);
		if (!err &&
		    (ext_csd[EXT_CSD_PARTITIONING_SUPPORT] ==
		    test_csd[EXT_CSD_PARTITIONING_SUPPORT]) &&
		    (ext_csd[EXT_CSD_ERASE_GROUP_DEF] ==
		    test_csd[EXT_CSD_ERASE_GROUP_DEF]) &&
		    (ext_csd[EXT_CSD_REV] ==
		    test_csd[EXT_CSD_REV]) &&
		    (ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE] ==
		    test_csd[EXT_CSD_HC_ERASE_GRP_SIZE]) &&
		    memcmp(&ext_csd[EXT_CSD_SEC_CNT],
			   &test_csd[EXT_CSD_SEC_CNT], 4) == 0) {
			media->caps |= width;
			break;
		}
	}
	return err;
}

int mmc_update_capacity(struct storage_media *media)
{
	uint64_t capacity;
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;
	int err;
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, ext_csd, EXT_CSD_SIZE);
	uint32_t erase_size;
	uint32_t hc_erase_size;
	uint64_t hc_wp_size;
	int index;

	if (media->version < MMC_VERSION_4)
		return 0;

	/* check  ext_csd version and capacity */
	err = mmc_send_ext_csd(ctrlr, ext_csd);
	if (err)
		return err;

	if (ext_csd[EXT_CSD_REV] < 2)
		return 0;

	/* Determine the eMMC device information */
	media->partition_config = ext_csd[EXT_CSD_PART_CONF]
		& EXT_CSD_PART_ACCESS_MASK;

	/* Determine the user partition size
	 *
	 * According to the JEDEC Standard, the value of
	 * ext_csd's capacity is valid if the value is
	 * more than 2GB
	 */
	capacity = (uint32_t)(ext_csd[EXT_CSD_SEC_CNT + 0] << 0 |
		    ext_csd[EXT_CSD_SEC_CNT + 1] << 8 |
		    ext_csd[EXT_CSD_SEC_CNT + 2] << 16 |
		    ext_csd[EXT_CSD_SEC_CNT + 3] << 24);
	capacity *= 512;
	if ((capacity >> 20) > 2 * 1024)
		media->capacity[MMC_PARTITION_USER] = capacity;

	/* Determine the boot partition sizes */
	hc_erase_size = ext_csd[224] * 512 * KiB;
	capacity = ext_csd[EXT_CSD_BOOT_SIZE_MULT] * 128 * KiB;
	media->capacity[MMC_PARTITION_BOOT_1] = capacity;
	media->capacity[MMC_PARTITION_BOOT_2] = capacity;

	/* Determine the RPMB size */
	hc_wp_size = ext_csd[EXT_CSD_HC_WP_GRP_SIZE] * hc_erase_size;
	capacity = 128 * KiB * ext_csd[EXT_CSD_RPMB_SIZE_MULT];
	media->capacity[MMC_PARTITION_RPMB] = capacity;

	/* Determine the general partition sizes */
	capacity = (ext_csd[EXT_CSD_GP_SIZE_MULT_GP0 + 2] << 16)
		| (ext_csd[EXT_CSD_GP_SIZE_MULT_GP0 + 1] << 8)
		| ext_csd[EXT_CSD_GP_SIZE_MULT_GP0];
	capacity *= hc_wp_size;
	media->capacity[MMC_PARTITION_GP1] = capacity;

	capacity = (ext_csd[EXT_CSD_GP_SIZE_MULT_GP1 + 2] << 16)
		| (ext_csd[EXT_CSD_GP_SIZE_MULT_GP1 + 1] << 8)
		| ext_csd[EXT_CSD_GP_SIZE_MULT_GP1];
	capacity *= hc_wp_size;
	media->capacity[MMC_PARTITION_GP2] = capacity;

	capacity = (ext_csd[EXT_CSD_GP_SIZE_MULT_GP2 + 2] << 16)
		| (ext_csd[EXT_CSD_GP_SIZE_MULT_GP2 + 1] << 8)
		| ext_csd[EXT_CSD_GP_SIZE_MULT_GP2];
	capacity *= hc_wp_size;
	media->capacity[MMC_PARTITION_GP3] = capacity;

	capacity = (ext_csd[EXT_CSD_GP_SIZE_MULT_GP3 + 2] << 16)
		| (ext_csd[EXT_CSD_GP_SIZE_MULT_GP3 + 1] << 8)
		| ext_csd[EXT_CSD_GP_SIZE_MULT_GP3];
	capacity *= hc_wp_size;
	media->capacity[MMC_PARTITION_GP4] = capacity;

	/* Determine the erase size */
	erase_size = (sd_mmc_extract_uint32_bits(media->csd,
		81, 5) + 1) *
		(sd_mmc_extract_uint32_bits(media->csd, 86, 5)
		+ 1);
	for (index = MMC_PARTITION_BOOT_1; index <= MMC_PARTITION_GP4;
		index++) {
		if (media->capacity[index] != 0) {
			/* Enable the partitions  */
			err = mmc_switch(media, EXT_CSD_ERASE_GROUP_DEF,
				EXT_CSD_PARTITION_ENABLE);
			if (err) {
				sdhc_error("Failed to enable partition access\n");
				return err;
			}

			/* Use HC erase group size */
			erase_size = hc_erase_size / media->write_bl_len;
			break;
		}
	}
	media->erase_blocks = erase_size;
	media->trim_mult = ext_csd[EXT_CSD_TRIM_MULT];

	return 0;
}

int mmc_set_partition(struct storage_media *media,
	unsigned int partition_number)
{
	uint8_t partition_config;

	/* Validate the partition number */
	if ((partition_number > MMC_PARTITION_GP4)
		|| (!media->capacity[partition_number]))
		return -1;

	/* Update the partition register */
	partition_config = media->partition_config;
	partition_config &= ~EXT_CSD_PART_ACCESS_MASK;
	partition_config |= partition_number;

	/* Select the new partition */
	int ret = mmc_switch(media, EXT_CSD_PART_CONF, partition_config);
	if (!ret)
		media->partition_config = partition_config;

	return ret;
}

const char *mmc_partition_name(struct storage_media *media,
	unsigned int partition_number)
{
	static const char *const partition_name[8] = {
		"User",		/* 0 */
		"Boot 1",	/* 1 */
		"Boot 2",	/* 2 */
		"RPMB",		/* 3 */
		"GP 1",		/* 4 */
		"GP 2",		/* 5 */
		"GP 3",		/* 6 */
		"GP 4"		/* 7 */
	};

	if (partition_number >= ARRAY_SIZE(partition_name))
		return "";
	return partition_name[partition_number];
}

void mmc_set_early_wake_status(int32_t status)
{
	int32_t *ms_cbmem;

	ms_cbmem = cbmem_add(CBMEM_ID_MMC_STATUS, sizeof(status));

	if (!ms_cbmem) {
		printk(BIOS_ERR,
		       "%s: Failed to add early mmc wake status to cbmem!\n",
		       __func__);
		return;
	}

	*ms_cbmem = status;
}

int mmc_send_cmd1(struct storage_media *media)
{
	int err;

	/* Reset emmc, send CMD0 */
	if (sd_mmc_go_idle(media))
		goto out_err;

	/* Send CMD1 */
	err = mmc_send_op_cond(media);
	if (err == 0) {
		if (media->op_cond_response & OCR_HCS)
			mmc_set_early_wake_status(MMC_STATUS_CMD1_READY_HCS);
		else
			mmc_set_early_wake_status(MMC_STATUS_CMD1_READY);
	} else if (err == CARD_IN_PROGRESS) {
		mmc_set_early_wake_status(MMC_STATUS_CMD1_IN_PROGRESS);
	} else {
		goto out_err;
	}

	return 0;

out_err:
	mmc_set_early_wake_status(MMC_STATUS_NEED_RESET);
	return -1;
}
