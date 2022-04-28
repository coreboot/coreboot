/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelbasecode/debug_feature.h>
#include <console/console.h>
#include <spi_flash.h>

#define SI_DESC_OEM_SECTION_OFFSET	0xF00
#define DEBUG_FEATURE_CTRL_OFFSET	SI_DESC_OEM_SECTION_OFFSET
#define DEBUG_FEATURE_CTRL_SZ		64
#define SI_DESC_REGION_SZ		4096

#define DEBUG_FEATURE_UNDEFINED		0xff
struct debug_feature_cntrl {
	uint8_t cse_fw_update_disable; /* Byte location: 0xF00 */

	/*
	 * Supported CPU Trace Hub modes:
	 * 0: Disable, 1: Target Debugger Mode, 2: Host Debugger Mode
	 */
	uint8_t cpu_tracehub_mode;     /* Byte location: 0xF01 */

	/*
	 * Supported PCH Trace Hub modes:
	 * 0: Disable, 1:Target Debugger Mode, 2:Host Debugger Mode
	 */
	uint8_t pch_tracehub_mode;     /* Byte location: 0xF02 */
	uint8_t reserved[61];
};

static struct debug_feature_cntrl dbg_feature_cntrl;

_Static_assert(sizeof(struct debug_feature_cntrl) % 64 == 0
		&& sizeof(struct debug_feature_cntrl) <= 256,
		"sizeof(struct debug_feature_cntrl) must be a multiple of 64 bytes and up to 256 bytes");

void debug_get_pch_cpu_tracehub_modes(uint8_t *cpu_tracehub_mode, uint8_t *pch_tracehub_mode)
{
	if (dbg_feature_cntrl.pch_tracehub_mode != DEBUG_FEATURE_UNDEFINED)
		*pch_tracehub_mode = dbg_feature_cntrl.pch_tracehub_mode;

	if (dbg_feature_cntrl.cpu_tracehub_mode != DEBUG_FEATURE_UNDEFINED)
		*cpu_tracehub_mode = dbg_feature_cntrl.cpu_tracehub_mode;

	printk(BIOS_DEBUG, "rt_debug: CPU Trace Hub Mode: %d PCH Trace Hub Mode: %d\n",
			*pch_tracehub_mode, *cpu_tracehub_mode);
}

bool is_debug_cse_fw_update_disable(void)
{
	printk(BIOS_DEBUG, "rt_debug: dbg_feature_cntrl.cse_fw_update_disable=%d\n",
			dbg_feature_cntrl.cse_fw_update_disable);

	return dbg_feature_cntrl.cse_fw_update_disable == 1;
}

enum cb_err dbg_feature_cntrl_init(void)
{
	const struct spi_flash *spi_flash_dev = boot_device_spi_flash();

	if (spi_flash_dev == NULL) {
		printk(BIOS_ERR, "Failed to Initialize boot device SPI flash\n");
		return CB_ERR;
	}

	if (spi_flash_read(spi_flash_dev, DEBUG_FEATURE_CTRL_OFFSET,
				DEBUG_FEATURE_CTRL_SZ, &dbg_feature_cntrl)) {
		printk(BIOS_ERR, "Failed to read Descriptor Region from SPI Flash\n");
		return CB_ERR;
	}
	return CB_SUCCESS;
}
