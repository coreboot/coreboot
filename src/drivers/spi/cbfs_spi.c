/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file provides a common CBFS wrapper for SPI storage. SPI driver
 * context is expanded with the buffer descriptor used to store data read from
 * SPI.
 */

#include <boot_device.h>
#include <cbfs.h>
#include <console/console.h>
#include <spi_flash.h>
#include <symbols.h>
#include <stdint.h>
#include <timer.h>

static struct spi_flash spi_flash_info;
static bool spi_flash_init_done;

/*
 * SPI speed logging for big transfers available with BIOS_DEBUG. The format is:
 *
 * read SPI 0x62854 0x7db7: 10416 us, 3089 KB/s, 24.712 Mbps
 *
 * The important number is the last one. It should roughly match your SPI
 * clock. If it doesn't, your driver might need a little tuning.
 */
static ssize_t spi_readat(const struct region_device *rd, void *b,
				size_t offset, size_t size)
{
	struct stopwatch sw;
	bool show = size >= 4 * KiB && console_log_level(BIOS_DEBUG);

	if (show)
		stopwatch_init(&sw);
	if (spi_flash_read(&spi_flash_info, offset, size, b))
		return -1;
	if (show) {
		long usecs;

		usecs = stopwatch_duration_usecs(&sw);
		u64 speed;	/* KiB/s */
		int bps;	/* Bits per second */

		speed = size * (u64)1000 / usecs;
		bps = speed * 8;

		printk(BIOS_DEBUG, "read SPI %#zx %#zx: %ld us, %lld KB/s, %d.%03d Mbps\n",
		       offset, size, usecs, speed, bps / 1000, bps % 1000);
	}
	return size;
}

static ssize_t spi_writeat(const struct region_device *rd, const void *b,
				size_t offset, size_t size)
{
	if (spi_flash_write(&spi_flash_info, offset, size, b))
		return -1;
	return size;
}

static ssize_t spi_eraseat(const struct region_device *rd,
				size_t offset, size_t size)
{
	if (spi_flash_erase(&spi_flash_info, offset, size))
		return -1;
	return size;
}

/* Provide all operations on the same device. */
static const struct region_device_ops spi_ops = {
	.mmap = mmap_helper_rdev_mmap,
	.munmap = mmap_helper_rdev_munmap,
	.readat = spi_readat,
	.writeat = spi_writeat,
	.eraseat = spi_eraseat,
};

static struct mmap_helper_region_device mdev =
	MMAP_HELPER_DEV_INIT(&spi_ops, 0, CONFIG_ROM_SIZE, &cbfs_cache);

void boot_device_init(void)
{
	int bus = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS;
	int cs = 0;

	if (spi_flash_init_done == true)
		return;

	if (spi_flash_probe(bus, cs, &spi_flash_info))
		return;

	spi_flash_init_done = true;
}

/* Return the CBFS boot device. */
const struct region_device *boot_device_ro(void)
{
	if (spi_flash_init_done != true)
		return NULL;

	return &mdev.rdev;
}

/* The read-only and read-write implementations are symmetric. */
const struct region_device *boot_device_rw(void)
{
	return boot_device_ro();
}

const struct spi_flash *boot_device_spi_flash(void)
{
	boot_device_init();

	if (spi_flash_init_done != true)
		return NULL;

	return &spi_flash_info;
}
