/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <fsp/api.h>
#include <gpio.h>

void variant_configure_fspm(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *m_cfg = &memupd->FspmConfig;

	m_cfg->ActiveCoreCount = 4;
	m_cfg->ActiveSmallCoreCount = 0;
}

static uint32_t get_board_id(void)
{
	const gpio_t board_id_gpios[] = {
		GPP_D0,
		GPP_D1,
		GPP_D2,
		GPP_D3
	};

	return gpio_base2_value(board_id_gpios, ARRAY_SIZE(board_id_gpios));
}

size_t variant_memory_sku(void)
{
	uint32_t board_id = get_board_id();

	printk(BIOS_INFO, "Board ID: 0x%x\n", board_id);

	switch (board_id) {
	case 0x0:
		return 0;  /* MT62F512M32D1DS-023 AIT:E */
	case 0x1:
		return 1;  /* empty */
	default:
		printk(BIOS_ERR, "Unknown board_id 0x%x, falling back to index 0\n", board_id);
		return 0;
	}
}
