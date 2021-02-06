/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <gpio.h>
#include <spd_bin.h>
#include <string.h>
#include <variant/gpio.h>
#include <amdblocks/dimm_spd.h>

uint8_t __weak variant_memory_sku(void)
{
	gpio_t pads[] = {
		[3] = MEM_CONFIG3,
		[2] = MEM_CONFIG2,
		[1] = MEM_CONFIG1,
		[0] = MEM_CONFIG0,
	};

	return gpio_base2_value(pads, ARRAY_SIZE(pads));
}

int __weak variant_mainboard_read_spd(uint8_t spdAddress,
							char *buf, size_t len)
{
	u8 spd_index = variant_memory_sku();

	printk(BIOS_INFO, "%s SPD index %d\n", __func__, spd_index);

	void *spd = (void *)spd_cbfs_map(spd_index);
	if (!spd) {
		printk(BIOS_ERR, "Error: spd.bin not found\n");
		return -1;
	}

	if (len != CONFIG_DIMM_SPD_SIZE) {
		printk(BIOS_ERR, "Error: spd.bin is not the correct size\n");
		return -1;
	}

	memcpy(buf, spd, len);

	return 0;
}
