/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <cbfs.h>
#include <console/console.h>
#include <soc/emi.h>

/*
 * The RAM_CODE ADC on Kukui can support only 12 different levels. Each model
 * can create its own mapping if needed, with an offset (0x10, 0x20, ...,
 * defined as CONFIG_BOARD_SDRAM_TABLE_OFFSET) applied in ram_code().
 */
static const char *const sdram_configs[] = {
	/* Standard table. */
	[0x00] = "sdram-lpddr4x-K4UBE3D4AA-MGCR-4GB",
	[0x01] = "sdram-lpddr4x-H9HCNNNCPMALHR-4GB",
	[0x02] = "sdram-lpddr4x-MT53E1G32D4NQ-4GB",
	[0x03] = "sdram-lpddr4x-KMDH6001DA-B422-4GB",
	[0x04] = "sdram-lpddr4x-KMDP6001DA-B425-4GB",
	[0x05] = "sdram-lpddr4x-MT29VZZZAD8DQKSL-4GB",
	[0x06] = "sdram-lpddr4x-KMDV6001DA-B620-4GB",
	[0x07] = "sdram-lpddr4x-SDADA4CR-128G-4GB",
	[0x08] = "sdram-lpddr4x-K4UBE3D4AA-MGCL-4GB",
	[0x09] = "sdram-lpddr4x-MT53E2G32D4NQ-046-8GB",
	[0x0a] = "sdram-lpddr4x-H9HCNNNCPMMLXR-NEE-4GB",

	/* Table shared by Burnet and its variants, offset = 0x10 */
	[0x10] = "sdram-lpddr4x-K4UBE3D4AA-MGCR-4GB",
	[0x11] = "sdram-lpddr4x-H9HCNNNCPMALHR-4GB",
	[0x12] = "sdram-lpddr4x-MT53E1G32D4NQ-4GB",
	[0x13] = "sdram-lpddr4x-K4UBE3D4AA-MGCL-4GB",
	[0x14] = "sdram-lpddr4x-H9HCNNNCPMMLXR-NEE-4GB",
	[0x16] = "sdram-lpddr4x-MT53E2G32D4NQ-046-8GB",
};

static struct sdram_params params;

const struct sdram_params *get_sdram_config(void)
{
	uint32_t ramcode = ram_code();
	const char *name = NULL;

	if (ramcode < ARRAY_SIZE(sdram_configs))
		name = sdram_configs[ramcode];

	if (!name || cbfs_boot_load_file(name, &params, sizeof(params),
					 CBFS_TYPE_STRUCT) != sizeof(params))
		die("Cannot load SDRAM parameter file for RAM code %#02x: %s!",
		    ramcode, name ? name : "unknown");

	return &params;
}
