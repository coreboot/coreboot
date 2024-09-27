/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot_device.h>
#include <bootstate.h>
#include <commonlib/bsd/ipchksum.h>
#include <stdlib.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <fmap.h>
#include <lib.h>
#include <spi-generic.h>
#include <spi_flash.h>
#include <string.h>

#include "fsp_hob.h"
#include "kti_cache.h"

struct kti_metadata {
	uint16_t data_size;
	uint16_t data_checksum;
} __packed;

void *kti_cache_load(size_t *size)
{
	struct region region;
	struct region_device read_rdev;
	struct kti_metadata md;
	void *data;
	uint16_t checksum;

	if (fmap_locate_area(CONFIG_KTI_CACHE_FMAP_NAME, &region) != 0) {
		printk(BIOS_ERR, "Region %s doesn't exist!\n", CONFIG_KTI_CACHE_FMAP_NAME);
		return NULL;
	}

	if (boot_device_ro_subregion(&region, &read_rdev) < 0)
		return NULL;

	if (rdev_readat(&read_rdev, &md, 0, sizeof(struct kti_metadata)) < 0) {
		printk(BIOS_ERR, "Couldn't read KTI metadata\n");
		return NULL;
	}

	if (md.data_size == 0xFFFF) {
		printk(BIOS_INFO, "KTI cache not found!\n");
		return NULL;
	}

	data = rdev_mmap(&read_rdev, sizeof(struct kti_metadata), md.data_size);
	if (data == NULL) {
		printk(BIOS_ERR, "Map KTI cache failed.\n");
		return NULL;
	}

	checksum = ipchksum(data, md.data_size);
	rdev_munmap(&read_rdev, data);
	if (checksum != md.data_checksum) {
		printk(BIOS_ERR, "KTI cache checksum mismatch: %x vs %x\n", md.data_checksum,
		       checksum);
		return NULL;
	}

	if (size)
		*size = md.data_size;

	return data;
}

static void kti_cache_protect(void)
{
	struct region region;

	if (fmap_locate_area(CONFIG_KTI_CACHE_FMAP_NAME, &region) < 0) {
		return;
	}

	if (spi_flash_ctrlr_protect_region(boot_device_spi_flash(), &region, WRITE_PROTECT) <
	    0) {
		printk(BIOS_ERR, "Set Flash Protected Range for %s failed.\n",
		       CONFIG_KTI_CACHE_FMAP_NAME);
		return;
	}

	printk(BIOS_INFO, "Enable Flash Protected Range on %s.\n", CONFIG_KTI_CACHE_FMAP_NAME);
}

static void kti_cache_save(void *unused)
{
	size_t kti_data_size;
	const void *kti_data;
	struct kti_metadata *md;
	struct region region;
	struct region_device write_rdev;

	printk(BIOS_INFO, "Save KTI starts...\n");

	kti_data = fsp_hob_get_kti_cache(&kti_data_size);
	if (!kti_data) {
		printk(BIOS_WARNING, "Couldn't find KTI cache hob!\n");
		return;
	}

	hexdump(kti_data, kti_data_size);

	md = malloc(sizeof(struct kti_metadata) + kti_data_size);
	if (md == NULL) {
		printk(BIOS_ERR, "Allocate KTI metadata failed!\n");
		return;
	}

	memset(md, 0, sizeof(struct kti_metadata));
	md->data_size = kti_data_size;
	md->data_checksum = ipchksum(kti_data, kti_data_size);
	memcpy(md + 1, kti_data, kti_data_size);

	if (fmap_locate_area(CONFIG_KTI_CACHE_FMAP_NAME, &region) != 0)
		goto ret;

	if (boot_device_rw_subregion(&region, &write_rdev) < 0)
		goto ret;

	if (rdev_eraseat(&write_rdev, 0, region_device_sz(&write_rdev)) < 0) {
		printk(BIOS_ERR, "Erase stale KTI cache failed.\n");
		goto ret;
	}

	if (rdev_writeat(&write_rdev, md, 0, sizeof(struct kti_metadata) + kti_data_size) < 0) {
		printk(BIOS_ERR, "Write KTI cache failed.\n");
		goto ret;
	}

	kti_cache_protect();

	printk(BIOS_INFO, "Save KTI ends.\n");

ret:
	free(md);
}

/**
 * Ensures kti data is stored into SPI after PCI enumeration is done during
 * BS_DEV_ENUMERATE-BS_ON_EXIT and lock down SPI protected ranges during
 * BS_DEV_RESOURCES-BS_ON_EXIT.
 */
BOOT_STATE_INIT_ENTRY(BS_DEV_ENUMERATE, BS_ON_EXIT, kti_cache_save, NULL);
