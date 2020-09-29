/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <assert.h>
#include <boot_device.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <fmap.h>
#include <soc/mrc_cache.h>
#include <spi_flash.h>
#include <stdint.h>
#include <string.h>

#define DEFAULT_MRC_CACHE "RW_MRC_CACHE"
/* PSP requires this value to be 64KiB */
#define DEFAULT_MRC_CACHE_SIZE 0x10000

#if !CONFIG_PSP_APOB_DRAM_ADDRESS
#error Incorrect APOB configuration setting(s)
#endif

#define APOB_SIGNATURE 0x424F5041	/* 'APOB' */

/* APOB_BASE_HEADER from AGESA */
struct apob_base_header {
	uint32_t   signature;			/* APOB signature */
	uint32_t   version;			/* Version */
	uint32_t   size;			/* APOB Size */
	uint32_t   offset_of_first_entry;	/* APOB Header Size */
};

static bool apob_header_valid(const struct apob_base_header *apob_header_ptr, const char *where)
{
	if (apob_header_ptr->signature != APOB_SIGNATURE) {
		printk(BIOS_WARNING, "Invalid %s APOB signature %x\n",
			where, apob_header_ptr->signature);
		return false;
	}

	if (apob_header_ptr->size == 0 || apob_header_ptr->size > DEFAULT_MRC_CACHE_SIZE) {
		printk(BIOS_WARNING, "%s APOB data is too large %x > %x\n",
			where, apob_header_ptr->size, DEFAULT_MRC_CACHE_SIZE);
		return false;
	}

	return true;
}

static void *get_apob_dram_address(void)
{
	/*
	 * TODO: Find the APOB destination by parsing the PSP's tables
	 * (once vboot is implemented).
	 */
	void *apob_src_ram = (void *)(uintptr_t)CONFIG_PSP_APOB_DRAM_ADDRESS;

	if (apob_header_valid(apob_src_ram, "RAM") == false)
		return NULL;

	return apob_src_ram;
}

static int get_nv_region(struct region *r)
{
	if  (fmap_locate_area(DEFAULT_MRC_CACHE, r) < 0) {
		printk(BIOS_ERR, "Error: No APOB NV region is found in flash\n");
		return -1;
	}

	return 0;
}

static void *get_apob_from_nv_region(struct region *region)
{
	struct region_device read_rdev;
	struct apob_base_header apob_header;

	if (boot_device_ro_subregion(region, &read_rdev) < 0) {
		printk(BIOS_ERR, "Failed boot_device_ro_subregion\n");
		return NULL;
	}

	if (rdev_readat(&read_rdev, &apob_header, 0, sizeof(apob_header)) < 0) {
		printk(BIOS_ERR, "Couldn't read APOB header!\n");
		return NULL;
	}

	if (apob_header_valid(&apob_header, "ROM") == false) {
		printk(BIOS_ERR, "No APOB NV data!\n");
		return NULL;
	}

	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));
	return rdev_mmap_full(&read_rdev);
}

/* Save APOB buffer to flash */
void soc_update_mrc_cache(void)
{
	struct apob_base_header *apob_rom;
	struct region_device write_rdev;
	struct region region;
	bool update_needed = false;
	const struct apob_base_header *apob_src_ram;

	/* Nothing to update in case of S3 resume. */
	if (acpi_is_wakeup_s3())
		return;

	apob_src_ram = get_apob_dram_address();
	if (apob_src_ram == NULL)
		return;

	if (get_nv_region(&region) != 0)
		return;

	apob_rom = get_apob_from_nv_region(&region);
	if (apob_rom == NULL) {
		update_needed = true;
	} else if (memcmp(apob_src_ram, apob_rom, apob_src_ram->size)) {
		printk(BIOS_INFO, "APOB RAM copy differs from flash\n");
		update_needed = true;
	} else
		printk(BIOS_DEBUG, "APOB valid copy is already in flash\n");

	if (!update_needed)
		return;

	printk(BIOS_SPEW, "Copy APOB from RAM 0x%p/0x%x to flash 0x%zx/0x%zx\n",
		apob_src_ram, apob_src_ram->size,
		region_offset(&region), region_sz(&region));

	if (boot_device_rw_subregion(&region, &write_rdev) < 0) {
		printk(BIOS_ERR, "Failed boot_device_rw_subregion\n");
		return;
	}

	/* write data to flash region */
	if (rdev_eraseat(&write_rdev, 0, DEFAULT_MRC_CACHE_SIZE) < 0) {
		printk(BIOS_ERR, "Error: APOB flash region erase failed\n");
		return;
	}

	if (rdev_writeat(&write_rdev, apob_src_ram, 0, apob_src_ram->size) < 0) {
		printk(BIOS_ERR, "Error: APOB flash region update failed\n");
		return;
	}

	printk(BIOS_INFO, "Updated APOB in flash\n");
}

static void *get_apob_nv_address(void)
{
	struct region region;

	if (get_nv_region(&region) != 0)
		return NULL;

	return get_apob_from_nv_region(&region);
}

void *soc_fill_mrc_cache(void)
{
	/* If this is non-S3 boot, then use the APOB data placed by PSP in DRAM. */
	if (!acpi_is_wakeup_s3())
		return get_apob_dram_address();

	/*
	 * In case of S3 resume, PSP does not copy APOB data to DRAM. Thus, coreboot needs to
	 * provide the APOB NV data from RW_MRC_CACHE on SPI flash so that FSP can use it
	 * without having to traverse the BIOS directory table.
	 */
	return get_apob_nv_address();
}
