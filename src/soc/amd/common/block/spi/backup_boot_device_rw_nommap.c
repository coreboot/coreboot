/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/backup_boot_device.h>
#include <amdblocks/psp.h>
#include <boot_device.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <stdint.h>
#include <string.h>

static struct spi_flash sfg;
static bool sfg_init_done;

/*
 * The assumption here is that chip select 0 and CONFIG_BACKUP_BOOT_DEVICE_SPI_CHIP_SELECT have
 * an SPI flash connected to it. Depending on which chip select of these two we booted from, we
 * deem one of them our backup/secondary (the one we didn't boot from) SPI flash.
 */
int backup_boot_device_spi_cs(void)
{
	if (boot_device_spi_cs() == 0)
		return CONFIG_BACKUP_BOOT_DEVICE_SPI_CHIP_SELECT;
	return 0;
}

static ssize_t spi_readat(const struct region_device *rd, void *b,
				size_t offset, size_t size)
{
	if (spi_flash_read(&sfg, offset, size, b))
		return -1;

	return size;
}

static ssize_t spi_writeat(const struct region_device *rd, const void *b,
				size_t offset, size_t size)
{
	if (spi_flash_write(&sfg, offset, size, b))
		return -1;

	return size;
}

static ssize_t spi_eraseat(const struct region_device *rd,
				size_t offset, size_t size)
{
	if (spi_flash_erase(&sfg, offset, size))
		return -1;

	return size;
}

static const struct region_device_ops spi_ops = {
	.readat = spi_readat,
	.writeat = spi_writeat,
	.eraseat = spi_eraseat,
};

static const struct region_device spi_rw =
	REGION_DEV_INIT(&spi_ops, 0, CONFIG_ROM_SIZE);

static void backup_boot_device_rw_init(void)
{
	const int bus = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS;
	const int cs = backup_boot_device_spi_cs();

	if (sfg_init_done == true)
		return;

	/* Cannot access backup boot device if ROM Armor is active */
	if (psp_get_hsti_state_rom_armor_enforced())
		return;

	/* Ensure any necessary setup is performed by the drivers. */
	spi_init();

	if (!spi_flash_probe(bus, cs, &sfg))
		sfg_init_done = true;
}

/**
 * Returns the secondary SPI flash as read-writable region device.
 *
 * @return NULL on error. The region_device on success.
 */
const struct region_device *backup_boot_device_rw(void)
{
	/* Cannot access backup boot device if ROM Armor is active */
	if (psp_get_hsti_state_rom_armor_enforced())
		return NULL;

	/* Probe for the SPI flash device if not already done. */
	backup_boot_device_rw_init();

	if (sfg_init_done != true)
		return NULL;

	return &spi_rw;
}

/**
 * Returns the secondary SPI flash as spi_flash device.
 *
 * @return NULL on error. The region_device on success.
 */
const struct spi_flash *backup_boot_device_spi_flash(void)
{
	/* Cannot access backup boot device if ROM Armor is active */
	if (psp_get_hsti_state_rom_armor_enforced())
		return NULL;

	backup_boot_device_rw_init();

	if (sfg_init_done != true)
		return NULL;

	return &sfg;
}

/**
 * Returns a sub-region of the secondary SPI flash.
 *
 * @param sub   The subregion within the SPI flash
 * @param subrd The region_device to return
 *
 * @return 0 on success.
 */
int backup_boot_device_rw_subregion(const struct region *sub,
				    struct region_device *subrd)
{
	/* Cannot access backup boot device if ROM Armor is active */
	if (psp_get_hsti_state_rom_armor_enforced())
		return -1;

	/* Ensure boot device has been initialized at least once. */
	backup_boot_device_rw_init();

	const struct region_device *parent = backup_boot_device_rw();

	if (!parent)
		return -1;

	return rdev_chain(subrd, parent, region_offset(sub), region_sz(sub));
}

/**
 * Synchronize the SPI flash contents from one chip to the other.
 *
 * @param sub                   The region to synchronize
 * @param direction_prim_to_sec When true synchronize the primary (boot_device_rw())
 *                              to the secondary (backup_boot_device_rw()).
 *                              When false synchronize the secondary (backup_boot_device_rw())
 *                              to the primary (boot_device_rw()):
 *
 * @return 0 on success.
 */
int backup_boot_device_sync_subregion(const struct region *sub,
				      const bool direction_prim_to_sec)
{
	struct region_device prim, sec;
	static uint8_t buffer_prim[64 * KiB], buffer_sec[64 * KiB];
	struct region_device *rdev;
	void *data;
	int ret;

	const struct spi_flash *flash = boot_device_spi_flash();
	if (!flash)
		return -1;

	ret = boot_device_rw_subregion(sub, &prim);
	if (ret)
		return ret;

	ret = backup_boot_device_rw_subregion(sub, &sec);
	if (ret)
		return ret;

	/* Set target and source for transfer */
	rdev = direction_prim_to_sec ? &sec : &prim;
	data = direction_prim_to_sec ? buffer_prim : buffer_sec;

	size_t remaining = region_device_sz(&prim);
	size_t offset = 0, written = 0;
	do {
		size_t chunk = MIN(remaining, ARRAY_SIZE(buffer_prim));
		chunk = MIN(chunk, flash->sector_size);

		if (rdev_readat(&prim, buffer_prim, offset, chunk) != chunk)
			return -1;
		if (rdev_readat(&sec, buffer_sec, offset, chunk) != chunk)
			return -1;

		if (memcmp(buffer_prim, buffer_sec, chunk) != 0) {
			if (rdev_eraseat(rdev, offset, chunk) != chunk)
				return -1;
			if (rdev_writeat(rdev, data, offset, chunk) != chunk)
				return -1;
			written += chunk;
		}

		remaining -= chunk;
		offset += chunk;
	} while (remaining);

	printk(BIOS_INFO, "%s: Synced %zu bytes from SPI flash %s->%s\n", __func__,
	       written, direction_prim_to_sec ? "PRIM" : "SEC",
	       direction_prim_to_sec ? "SEC" : "PRIM");
	return 0;
}
