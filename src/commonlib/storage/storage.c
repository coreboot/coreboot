/*
 * Copyright 2008, Freescale Semiconductor, Inc
 * Andy Fleming
 *
 * Copyright 2013 Google Inc.  All rights reserved.
 * Copyright 2017 Intel Corporation
 *
 * MultiMediaCard (MMC), eMMC and Secure Digital (SD) common code which
 * transitions the card from the standby state to the transfer state.  The
 * common code supports read operations, erase and write operations are in
 * a separate modules.  This code is controller independent.
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
#include "sd_mmc.h"
#include "storage.h"
#include <string.h>

#define DECIMAL_CAPACITY_MULTIPLIER	1000ULL
#define HEX_CAPACITY_MULTIPLIER		1024ULL

struct capacity {
	const char *const units;
	uint64_t bytes;
};

static void display_capacity(struct storage_media *media, int partition_number)
{
	uint64_t capacity;
	uint64_t decimal_divisor;
	const char *decimal_units;
	uint64_t hex_divisor;
	const char *hex_units;
	int index;
	const char *name;
	const char *separator;
	const struct capacity decimal_list[] = {
		{"TB", DECIMAL_CAPACITY_MULTIPLIER * DECIMAL_CAPACITY_MULTIPLIER
			* DECIMAL_CAPACITY_MULTIPLIER
			* DECIMAL_CAPACITY_MULTIPLIER},
		{"GB", DECIMAL_CAPACITY_MULTIPLIER * DECIMAL_CAPACITY_MULTIPLIER
			* DECIMAL_CAPACITY_MULTIPLIER},
		{"MB", DECIMAL_CAPACITY_MULTIPLIER
			* DECIMAL_CAPACITY_MULTIPLIER},
		{"KB", DECIMAL_CAPACITY_MULTIPLIER},
		{"B", 1}
	};
	const struct capacity hex_list[] = {
		{"TiB", HEX_CAPACITY_MULTIPLIER * HEX_CAPACITY_MULTIPLIER
			* HEX_CAPACITY_MULTIPLIER * HEX_CAPACITY_MULTIPLIER},
		{"GiB", HEX_CAPACITY_MULTIPLIER * HEX_CAPACITY_MULTIPLIER
			* HEX_CAPACITY_MULTIPLIER},
		{"MiB", HEX_CAPACITY_MULTIPLIER * HEX_CAPACITY_MULTIPLIER},
		{"KiB", HEX_CAPACITY_MULTIPLIER},
		{"B", 1}
	};

	/* Get the partition name */
	capacity = media->capacity[partition_number];
	name = storage_partition_name(media, partition_number);
	separator = "";
	if (IS_ENABLED(CONFIG_COMMONLIB_STORAGE_MMC) && !IS_SD(media))
		separator = ": ";

	/* Determine the decimal divisor for the capacity */
	for (index = 0; index < ARRAY_SIZE(decimal_list) - 1; index++) {
		if (capacity >= decimal_list[index].bytes)
			break;
	}
	decimal_divisor = decimal_list[index].bytes;
	decimal_units = decimal_list[index].units;

	/* Determine the hex divisor for the capacity */
	for (index = 0; index < ARRAY_SIZE(hex_list) - 1; index++) {
		if (capacity >= hex_list[index].bytes)
			break;
	}
	hex_divisor = hex_list[index].bytes;
	hex_units = hex_list[index].units;

	/* Display the capacity */
	sdhc_debug("%3lld.%03lld %sytes (%3lld.%03lld %sytes)%s%s\n",
		capacity / decimal_divisor,
		(capacity / (decimal_divisor / 1000)) % 1000,
		decimal_units,
		capacity / hex_divisor,
		((capacity / (hex_divisor / 1024)) * 1000 / 1024) % 1000,
		hex_units,
		separator,
		name);
}

void storage_display_setup(struct storage_media *media)
{
	int partition_number;

	/* Display the device info */
	sd_mmc_debug("Man %06x Snr %u ",
	       media->cid[0] >> 24,
	       (((media->cid[2] & 0xffff) << 16) |
		((media->cid[3] >> 16) & 0xffff)));
	sd_mmc_debug("Product %c%c%c%c", media->cid[0] & 0xff,
	       (media->cid[1] >> 24), (media->cid[1] >> 16) & 0xff,
	       (media->cid[1] >> 8) & 0xff);
	if (!IS_SD(media)) /* eMMC product string is longer */
		sd_mmc_debug("%c%c", media->cid[1] & 0xff,
		       (media->cid[2] >> 24) & 0xff);
	sd_mmc_debug(" Revision %d.%d\n", (media->cid[2] >> 20) & 0xf,
	       (media->cid[2] >> 16) & 0xf);

	/* Display the erase block size */
	sdhc_debug("Erase block size: 0x%08x\n", media->erase_blocks
		* media->write_bl_len);

	/* Display the partition capacities */
	if (IS_ENABLED(CONFIG_SDHC_DEBUG)) {
		for (partition_number = 0; partition_number
			< ARRAY_SIZE(media->capacity); partition_number++) {
			if (!media->capacity[partition_number])
				continue;
			display_capacity(media, partition_number);
		}
	}
}

int storage_startup(struct storage_media *media)
{
	int err;
	uint64_t capacity;
	uint64_t cmult, csize;
	struct mmc_command cmd;
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;

	/* Determine the storage capacity */
	if (media->high_capacity) {
		cmult = 8;
		csize = sd_mmc_extract_uint32_bits(media->csd, 58, 22);
	} else {
		csize = sd_mmc_extract_uint32_bits(media->csd, 54, 12);
		cmult = sd_mmc_extract_uint32_bits(media->csd, 78, 3);
	}
	capacity = (csize + 1) << (cmult + 2);
	capacity *= media->read_bl_len;
	media->capacity[0] = capacity;

	/* Limit the block size to 512 bytes */
	if (media->read_bl_len > 512)
		media->read_bl_len = 512;
	if (media->write_bl_len > 512)
		media->write_bl_len = 512;

	/* Get the erase size in blocks */
	media->erase_blocks =
		(sd_mmc_extract_uint32_bits(media->csd, 47, 3) + 1)
		* (sd_mmc_extract_uint32_bits(media->csd, 42, 5) + 1);

	/* Select the card, and put it into Transfer Mode */
	cmd.cmdidx = MMC_CMD_SELECT_CARD;
	cmd.resp_type = CARD_RSP_R1;
	cmd.cmdarg = media->rca << 16;
	cmd.flags = 0;
	err = ctrlr->send_cmd(ctrlr, &cmd, NULL);
	if (err)
		return err;

	/* Increase the bus frequency */
	if (IS_ENABLED(CONFIG_COMMONLIB_STORAGE_SD) && IS_SD(media))
		err = sd_change_freq(media);
	else if (IS_ENABLED(CONFIG_COMMONLIB_STORAGE_MMC)) {
		err = mmc_change_freq(media);
		if (!err)
			mmc_update_capacity(media);
	}
	if (err)
		return err;

	/* Restrict card's capabilities by what the controller can do */
	media->caps &= ctrlr->caps;

	/* Increase the bus width if possible */
	if (IS_ENABLED(CONFIG_COMMONLIB_STORAGE_SD) && IS_SD(media))
		err = sd_set_bus_width(media);
	else if (IS_ENABLED(CONFIG_COMMONLIB_STORAGE_MMC))
		err = mmc_set_bus_width(media);
	if (err)
		return err;

	/* Display the card setup */
	storage_display_setup(media);
	return 0;
}

int storage_setup_media(struct storage_media *media, struct sd_mmc_ctrlr *ctrlr)
{
	int err;

	memset(media, 0, sizeof(*media));
	media->ctrlr = ctrlr;

	err = sd_mmc_enter_standby(media);
	if (err)
		return err;
	return storage_startup(media);
}

static int storage_read(struct storage_media *media, void *dest, uint32_t start,
	uint32_t block_count)
{
	struct mmc_command cmd;
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;

	cmd.resp_type = CARD_RSP_R1;
	cmd.flags = 0;

	if (block_count > 1)
		cmd.cmdidx = MMC_CMD_READ_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_READ_SINGLE_BLOCK;

	if (media->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * media->read_bl_len;

	struct mmc_data data;
	data.dest = dest;
	data.blocks = block_count;
	data.blocksize = media->read_bl_len;
	data.flags = DATA_FLAG_READ;

	if (ctrlr->send_cmd(ctrlr, &cmd, &data))
		return 0;

	if ((block_count > 1) && !(ctrlr->caps
		& DRVR_CAP_AUTO_CMD12)) {
		cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resp_type = CARD_RSP_R1b;
		cmd.flags = CMD_FLAG_IGNORE_INHIBIT;
		if (ctrlr->send_cmd(ctrlr, &cmd, NULL)) {
			sd_mmc_error("Failed to send stop cmd\n");
			return 0;
		}

		/* Waiting for the ready status */
		sd_mmc_send_status(media, SD_MMC_IO_RETRIES);
	}

	return block_count;
}

/////////////////////////////////////////////////////////////////////////////
// BlockDevice utilities and callbacks

int storage_block_setup(struct storage_media *media, uint64_t start,
	uint64_t count, int is_read)
{
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;
	int partition_number;

	if (count == 0)
		return 0;

	uint32_t bl_len = is_read ? media->read_bl_len :
		media->write_bl_len;

	/* Validate the block range */
	partition_number = media->partition_config & EXT_CSD_PART_ACCESS_MASK;
	if (((start * bl_len) > media->capacity[partition_number])
		|| (((start + count) * bl_len) >
		media->capacity[partition_number])) {
		sd_mmc_error("Block range exceeds device capacity\n");
		return 0;
	}

	/*
	 * CMD16 only applies to single data rate mode, and block
	 * length for double data rate is always 512 bytes.
	 */
	if ((ctrlr->timing == BUS_TIMING_UHS_DDR50) ||
	    (ctrlr->timing == BUS_TIMING_MMC_DDR52) ||
	    (ctrlr->timing == BUS_TIMING_MMC_HS400) ||
	    (ctrlr->timing == BUS_TIMING_MMC_HS400ES))
		return 1;
	if (sd_mmc_set_blocklen(ctrlr, bl_len))
		return 0;

	return 1;
}

uint64_t storage_block_read(struct storage_media *media, uint64_t start,
	uint64_t count, void *buffer)
{
	uint8_t *dest = (uint8_t *)buffer;

	if (storage_block_setup(media, start, count, 1) == 0)
		return 0;

	uint64_t todo = count;
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;
	do {
		uint32_t cur = (uint32_t)MIN(todo, ctrlr->b_max);
		if (storage_read(media, dest, start, cur) != cur)
			return 0;
		todo -= cur;
		sd_mmc_trace("%s: Got %d blocks, more %d (total %d) to go.\n",
			  __func__, (int)cur, (int)todo, (int)count);
		start += cur;
		dest += cur * media->read_bl_len;
	} while (todo > 0);
	return count;
}

int storage_set_partition(struct storage_media *media,
	unsigned int partition_number)
{
	int err;

	/* Select the partition */
	err = -1;
	if (IS_ENABLED(CONFIG_COMMONLIB_STORAGE_SD) && IS_SD(media))
		err = sd_set_partition(media, partition_number);
	else if (IS_ENABLED(CONFIG_COMMONLIB_STORAGE_MMC))
		err = mmc_set_partition(media, partition_number);
	if (err)
		sd_mmc_error("Invalid partition number!\n");
	return err;
}

const char *storage_partition_name(struct storage_media *media,
	unsigned int partition_number)
{
	const char *name;

	/* Get the partition name */
	name = NULL;
	if (IS_ENABLED(CONFIG_COMMONLIB_STORAGE_SD) && IS_SD(media))
		name = sd_partition_name(media, partition_number);
	else if (IS_ENABLED(CONFIG_COMMONLIB_STORAGE_MMC))
		name = mmc_partition_name(media, partition_number);
	return name;
}

unsigned int storage_get_current_partition(struct storage_media *media)
{
	return media->partition_config & EXT_CSD_PART_ACCESS_MASK;
}
