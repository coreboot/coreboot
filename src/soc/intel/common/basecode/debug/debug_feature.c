/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelbasecode/debug_feature.h>
#include <console/console.h>
#include <spi_flash.h>

#define SI_DESC_OEM_SECTION_OFFSET	0xF00
#define PRE_MEM_FEATURE_CTRL_OFFSET	SI_DESC_OEM_SECTION_OFFSET
#define PRE_MEM_FEATURE_CTRL_SZ		64
#define SI_DESC_REGION_SZ		4096

struct pre_mem_ft {
	uint8_t cse_fw_update_disable; /* Byte location: 0xF00 */
	uint8_t reserved[63];
};

static struct pre_mem_ft pre_mem_debug;

_Static_assert(sizeof(struct pre_mem_ft) % 64 == 0 && sizeof(struct pre_mem_ft) <= 256,
		"sizeof(struct pre_mem_ft) must be a multiple of 64 bytes and up to 256 bytes");

bool is_debug_cse_fw_update_disable(void)
{
	printk(BIOS_DEBUG, "rt_debug: pre_mem_debug.cse_fw_update_disable=%d\n",
			pre_mem_debug.cse_fw_update_disable);

	return pre_mem_debug.cse_fw_update_disable == 1;
}

enum cb_err pre_mem_debug_init(void)
{
	if (spi_flash_read(boot_device_spi_flash(), PRE_MEM_FEATURE_CTRL_OFFSET,
				PRE_MEM_FEATURE_CTRL_SZ, &pre_mem_debug)) {
		printk(BIOS_ERR, "Failed to read Descriptor Region from SPI Flash\n");
		return CB_ERR;
	}
	return CB_SUCCESS;
}
