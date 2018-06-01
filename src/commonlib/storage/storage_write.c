/*
 * Copyright 2008, Freescale Semiconductor, Inc
 * Andy Fleming
 *
 * Copyright 2013 Google Inc.  All rights reserved.
 * Copyright 2017 Intel Corporation
 *
 * MultiMediaCard (MMC), eMMC and Secure Digital (SD) write support code.
 * This code is controller independent.
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

#include "sd_mmc.h"
#include "storage.h"
#include <string.h>

static uint32_t storage_write(struct storage_media *media, uint32_t start,
	uint64_t block_count, const void *src)
{
	struct mmc_command cmd;
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;

	cmd.resp_type = CARD_RSP_R1;
	cmd.flags = 0;

	if (block_count > 1)
		cmd.cmdidx = MMC_CMD_WRITE_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_WRITE_SINGLE_BLOCK;

	if (media->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * media->write_bl_len;

	struct mmc_data data;
	data.src = src;
	data.blocks = block_count;
	data.blocksize = media->write_bl_len;
	data.flags = DATA_FLAG_WRITE;

	if (ctrlr->send_cmd(ctrlr, &cmd, &data)) {
		sd_mmc_error("Write failed\n");
		return 0;
	}

	/* SPI multiblock writes terminate using a special
	 * token, not a STOP_TRANSMISSION request.
	 */
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

uint64_t storage_block_write(struct storage_media *media, uint64_t start,
	uint64_t count, const void *buffer)
{
	const uint8_t *src = (const uint8_t *)buffer;

	if (storage_block_setup(media, start, count, 0) == 0)
		return 0;

	uint64_t todo = count;
	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;
	do {
		uint64_t cur = MIN(todo, ctrlr->b_max);
		if (storage_write(media, start, cur, src) != cur)
			return 0;
		todo -= cur;
		start += cur;
		src += cur * media->write_bl_len;
	} while (todo > 0);
	return count;
}

uint64_t storage_block_fill_write(struct storage_media *media, uint64_t start,
	uint64_t count, uint32_t fill_pattern)
{
	if (storage_block_setup(media, start, count, 0) == 0)
		return 0;

	struct sd_mmc_ctrlr *ctrlr = media->ctrlr;
	uint64_t block_size = media->write_bl_len;
	/*
	 * We allocate max 4 MiB buffer on heap and set it to fill_pattern and
	 * perform mmc_write operation using this 4MiB buffer until requested
	 * size on disk is written by the fill byte.
	 *
	 * 4MiB was chosen after repeating several experiments with the max
	 * buffer size to be used. Using 1 lba i.e. block_size buffer results in
	 * very large fill_write time. On the other hand, choosing 4MiB, 8MiB or
	 * even 128 Mib resulted in similar write times. With 2MiB, the
	 * fill_write time increased by several seconds. So, 4MiB was chosen as
	 * the default max buffer size.
	 */
	uint64_t heap_lba = (4 * MiB) / block_size;
	/*
	 * Actual allocated buffer size is minimum of three entities:
	 * 1) 4MiB equivalent in lba
	 * 2) count: Number of lbas to overwrite
	 * 3) ctrlr->b_max: Max lbas that the block device allows write
	 * operation on at a time.
	 */
	uint64_t buffer_lba = MIN(MIN(heap_lba, count), ctrlr->b_max);

	uint64_t buffer_bytes = buffer_lba * block_size;
	uint64_t buffer_words = buffer_bytes / sizeof(uint32_t);
	uint32_t *buffer = malloc(buffer_bytes);
	uint32_t *ptr = buffer;

	for ( ; buffer_words ; buffer_words--)
		*ptr++ = fill_pattern;

	uint64_t todo = count;
	int ret = 0;

	do {
		uint64_t curr_lba = MIN(buffer_lba, todo);

		if (storage_write(media, start, curr_lba, buffer) != curr_lba)
			goto cleanup;
		todo -= curr_lba;
		start += curr_lba;
	} while (todo > 0);

	ret = count;

cleanup:
	free(buffer);
	return ret;
}
