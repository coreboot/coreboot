/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot_device.h>
#include <symbols.h>
#include <console/console.h>
#include <assert.h>
#include <commonlib/storage/sd_mmc.h>
#include <cbfs.h>

#include <soc/ti/am335x/mmc.h>
#include <soc/ti/am335x/header.h>

// Where the coreboot image is expected to be located on the SD card
// Only certain locations are allowed - check the AM335x technical reference
// manual for more details.
#define COREBOOT_IMAGE_OFFSET (128 * KiB)

#define SD_BLOCK_SIZE 512

static struct am335x_mmc_host sd_host;
static struct storage_media media;

static size_t partial_block_read(uint8_t *dest, uint64_t block, uint32_t offset, uint32_t count)
{
	static uint8_t overflow_block[SD_BLOCK_SIZE];

	uint64_t blocks_read = storage_block_read(&media, block, 1, &overflow_block);

	if (blocks_read != 1) {
		printk(BIOS_ERR, "Expected to read 1 block but read: %llu\n", blocks_read);
		return 0;
	}

	assert((offset + count) <= SD_BLOCK_SIZE);

	int dest_index = 0;
	for (int overflow_index = offset; overflow_index < (offset + count); overflow_index++)
		dest[dest_index++] = overflow_block[overflow_index];

	return count;
}

// This supports reads from a SD card that aren't necessarily aligned to the
// sd block size
static ssize_t sd_readat(const struct region_device *rdev, void *dest, size_t offset,
			 size_t count)
{
	uint8_t *buffer = (uint8_t *)dest;

	uint64_t block_start = offset / SD_BLOCK_SIZE;
	uint64_t block_end = (offset + count) / SD_BLOCK_SIZE;
	uint64_t blocks = block_end - block_start + 1;

	// Read the last first, which might not be aligned on a SD block
	uint32_t first_block_offset = offset % SD_BLOCK_SIZE;
	size_t first_block_to_read = MIN(SD_BLOCK_SIZE - first_block_offset, count);
	size_t bytes_read = partial_block_read(buffer, block_start, first_block_offset,
					       first_block_to_read);

	if (blocks == 1)
		return bytes_read;

	buffer += bytes_read;

	if (blocks > 2) {
		// Read all the "whole" blocks between the start and end blocks
		uint64_t to_read = blocks - 2;
		uint64_t blocks_read =
			storage_block_read(&media, block_start + 1, to_read, (void *)buffer);

		if (blocks_read != to_read) {
			printk(BIOS_ERR, "Expecting to read %llu blocks but only read %llu\n",
			       to_read, blocks_read);
			return blocks_read * SD_BLOCK_SIZE;
		}

		buffer += to_read * SD_BLOCK_SIZE;
		bytes_read += to_read * SD_BLOCK_SIZE;
	}

	// Read the last block, which might not be aligned on a SD block
	bytes_read += partial_block_read(buffer, block_end, 0, count - bytes_read);

	return bytes_read;
}

static const struct region_device_ops am335x_sd_ops = {
	.mmap = mmap_helper_rdev_mmap,
	.munmap = mmap_helper_rdev_munmap,
	.readat = sd_readat,
};

extern struct omap_image_headers headers;

static struct mmap_helper_region_device sd_mdev = MMAP_HELPER_DEV_INIT(
	&am335x_sd_ops, COREBOOT_IMAGE_OFFSET + sizeof(headers), CONFIG_ROM_SIZE,
	&cbfs_cache);

static bool init_done = false;

void boot_device_init(void)
{
	if (init_done)
		return;

	sd_host.sd_clock_hz = 96000000;
	sd_host.reg = (void *)MMCHS0_BASE;
	am335x_mmc_init_storage(&sd_host);
	storage_setup_media(&media, &sd_host.sd_mmc_ctrlr);
	storage_display_setup(&media);

	init_done = true;
}

const struct region_device *boot_device_ro(void)
{
	return &sd_mdev.rdev;
}
