/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "harcuvar_boardid.h"
#include "gpio.h"
#include "spd/spd.h"
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/soc_binding.h>

/*
 * Define platform specific Memory Down Configure structure.
 *
 * If CONFIG(ENABLE_FSP_MEMORY_DOWN) is enabled, the MEMORY_DOWN_CONFIG
 * structure should be customized to match the design.
 *
 * .SlotState indicates the memory down state of the specific channel/DIMM.
 *
 * SlotState options:
 *
 *     STATE_MEMORY_DOWN: Memory down.
 *     STATE_MEMORY_SLOT: Physical memory slot.
 *
 * .SpdDataLen should always be MAX_SPD_BYTES/512.
 *
 * .SpdDataPtr is pointing to the SPD data structure when memory modules
 *             are memory down.
 *
 * SpdDataPtr options:
 *
 *     Non-NULL: Pointing to SPD data structure.
 *     NULL: Physical memory slot, no SPD data used.
 *
 * DIMM Mapping of SlotState & SpdDataPtr:
 *
 *     {{CH0DIMM0, CH0DIMM1}, {CH1DIMM0, CH1DIMM1}}
 *
 * Sample: Channel 0 is memory down and channel 1 is physical slot.
 *
 *	const MEMORY_DOWN_CONFIG mMemoryDownConfig = {
 *		.SlotState = {
 *			{STATE_MEMORY_DOWN, STATE_MEMORY_DOWN},
 *			{STATE_MEMORY_SLOT, STATE_MEMORY_SLOT}
 *		},
 *		.SpdDataLen = MAX_SPD_BYTES,
 *		.SpdDataPtr = {
 *			{(void *)CONFIG_SPD_LOC, (void *)CONFIG_SPD_LOC},
 *			{(void *)NULL, (void *)NULL}
 *		}
 *	}
 */

const MEMORY_DOWN_CONFIG mMemoryDownConfig = {
	.SlotState = {
		{STATE_MEMORY_SLOT, STATE_MEMORY_SLOT},
		{STATE_MEMORY_SLOT, STATE_MEMORY_SLOT}
	},
	.SpdDataLen = MAX_SPD_BYTES,
	.SpdDataPtr = {
		{(void *)NULL, (void *)NULL},
		{(void *)NULL, (void *)NULL}
	}
};

void mainboard_config_gpios(void);
void mainboard_memory_init_params(FSPM_UPD *mupd);

/*
* Configure GPIO depend on platform
*/
void mainboard_config_gpios(void)
{
	size_t num;
	const struct dnv_pad_config *table;
	uint32_t boardid = board_id();

	/* Configure pads prior to SiliconInit() in case there's any
	* dependencies during hardware initialization.
	*/
	switch (boardid) {
	case BoardIdHarcuvar:
		table = harcuvar_gpio_table;
		num = ARRAY_SIZE(harcuvar_gpio_table);
		break;
	default:
		table = NULL;
		num = 0;
		break;
	}

	if ((!table) || (!num)) {
		printk(BIOS_ERR, "ERROR: No valid GPIO table found!\n");
		return;
	}

	printk(BIOS_INFO, "GPIO table: 0x%x, entry num:  0x%x!\n",
	       (uint32_t)table, (uint32_t)num);
	gpio_configure_dnv_pads(table, num);
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	if (!CONFIG(ENABLE_FSP_MEMORY_DOWN))
		return;

	uint8_t *spd_data_ptr = NULL;

	/* Get SPD data pointer */
	spd_data_ptr = mainboard_find_spd_data();

	if (spd_data_ptr != NULL) {
		printk(BIOS_DEBUG, "Memory Down function is enabled!\n");

		/* Enable Memory Down function, set Memory
		 * Down Configure structure pointer.
		 */
		mupd->FspmConfig.PcdMemoryDown = 1;
		mupd->FspmConfig.PcdMemoryDownConfigPtr =
			(uint32_t)&mMemoryDownConfig;
	} else {
		printk(BIOS_DEBUG, "Memory Down function is disabled!\n");

		/* Disable Memory Down function */
		mupd->FspmConfig.PcdMemoryDown = 0;
		mupd->FspmConfig.PcdMemoryDownConfigPtr = 0;
	}
}
