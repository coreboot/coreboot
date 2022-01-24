/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boot_device.h>
#include <commonlib/region.h>
#include <intelbasecode/debug_feature.h>
#include <console/console.h>
#include <fmap.h>

#define SI_DESC_OEM_SECTION_OFFSET	0xF00
#define PRE_MEM_FEATURE_CTRL_OFFSET	SI_DESC_OEM_SECTION_OFFSET
#define PRE_MEM_FEATURE_CTRL_SZ		64
#define SI_DESC_REGION_SZ		4096

struct pre_mem_ft {
	uint8_t reserved[64];
};

static struct pre_mem_ft pre_mem_debug;

_Static_assert(sizeof(struct pre_mem_ft) % 64 == 0 && sizeof(struct pre_mem_ft) <= 256,
		"sizeof(struct pre_mem_ft) must be a multiple of 64 bytes and up to 256 bytes");

uint8_t pre_mem_debug_init(void)
{
	struct region_device desc_rdev;
	const struct region_device *boot_device = boot_device_ro();

	if (!boot_device) {
		printk(BIOS_ERR, "Failed to get RW boot device\n");
		return 1;
	}

	if (rdev_chain(&desc_rdev, boot_device, 0, SI_DESC_REGION_SZ)) {
		printk(BIOS_ERR, "Failed to get description region device\n");
		return 1;
	}

	if (rdev_readat(&desc_rdev, &pre_mem_debug, PRE_MEM_FEATURE_CTRL_OFFSET,
				PRE_MEM_FEATURE_CTRL_SZ) != PRE_MEM_FEATURE_CTRL_SZ) {
		printk(BIOS_ERR, "Failed to read Descriptor Region from SPI Flash\n");
		return 1;
	}
	return 0;
}
