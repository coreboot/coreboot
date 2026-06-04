/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot_device.h>
#include <console/console.h>
#include <fmap.h>
#include <commonlib/region.h>
#include <stdbool.h>
#include <soc/cdt.h>
#include <soc/symbols_common.h>

static const struct platform_id_cdt *plat_id;
static bool cdt_initialized;

static void cdt_init_platform_id(void)
{
	const struct cdt_header *hdr = (const struct cdt_header *)_cdt_data;
	const struct cdb_meta *meta;

	cdt_initialized = true;

	if (hdr->magic != CDT_MAGIC) {
		printk(BIOS_ERR, "CDT: Invalid magic 0x%08x\n", hdr->magic);
		return;
	}

	meta = (const struct cdb_meta *)(_cdt_data + sizeof(struct cdt_header));
	plat_id = (const struct platform_id_cdt *)
		(_cdt_data + meta[CDT_BLOCK_INDEX_PLATFORM_ID].offset);
}

ssize_t cdt_read(void *buffer, size_t buffer_size)
{
	struct region_device rdev;
	ssize_t bytes_read;
	size_t read_size;

	if (!buffer || buffer_size == 0) {
		printk(BIOS_ERR, "CDT: Invalid buffer parameters\n");
		return 0;
	}

	if (fmap_locate_area_as_rdev(CDT_REGION_NAME, &rdev) < 0) {
		printk(BIOS_ERR, "CDT: Could not locate '%s' region\n", CDT_REGION_NAME);
		return 0;
	}

	read_size = MIN(buffer_size, region_device_sz(&rdev));

	bytes_read = rdev_readat(&rdev, buffer, 0, read_size);
	if (bytes_read < 0) {
		printk(BIOS_ERR, "CDT: Failed to read data from flash\n");
		return 0;
	}

	printk(BIOS_INFO, "CDT: Read %zd bytes from '%s'\n", bytes_read, CDT_REGION_NAME);

	return bytes_read;
}

uint8_t cdt_get_platform_id(void)
{
	if (!cdt_initialized)
		cdt_init_platform_id();

	return plat_id ? plat_id->platform : 0;
}

uint32_t cdt_get_hw_version(void)
{
	if (!cdt_initialized)
		cdt_init_platform_id();

	return plat_id ? plat_id->hw_version_major : 0;
}
