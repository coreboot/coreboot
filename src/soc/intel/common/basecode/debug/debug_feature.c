/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelbasecode/debug_feature.h>
#include <console/console.h>
#include <spi_flash.h>

#define SI_DESC_OEM_SECTION_OFFSET	0xF00
#define DEBUG_FEATURE_CTRL_OFFSET	SI_DESC_OEM_SECTION_OFFSET
#define DEBUG_FEATURE_CTRL_SZ		64
#define SI_DESC_REGION_SZ		4096

struct debug_feature_cntrl {
	uint8_t cse_fw_update_disable; /* Byte location: 0xF00 */
	uint8_t reserved[63];
};

static struct debug_feature_cntrl dbg_feature_cntrl;

_Static_assert(sizeof(struct debug_feature_cntrl) % 64 == 0
		&& sizeof(struct debug_feature_cntrl) <= 256,
		"sizeof(struct debug_feature_cntrl) must be a multiple of 64 bytes and up to 256 bytes");

bool is_debug_cse_fw_update_disable(void)
{
	printk(BIOS_DEBUG, "rt_debug: dbg_feature_cntrl.cse_fw_update_disable=%d\n",
			dbg_feature_cntrl.cse_fw_update_disable);

	return dbg_feature_cntrl.cse_fw_update_disable == 1;
}

enum cb_err dbg_feature_cntrl_init(void)
{
	if (spi_flash_read(boot_device_spi_flash(), DEBUG_FEATURE_CTRL_OFFSET,
				DEBUG_FEATURE_CTRL_SZ, &dbg_feature_cntrl)) {
		printk(BIOS_ERR, "Failed to read Descriptor Region from SPI Flash\n");
		return CB_ERR;
	}
	return CB_SUCCESS;
}
