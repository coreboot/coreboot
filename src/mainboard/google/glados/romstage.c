/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/variant.h>
#include <device/dram/ddr3.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <soc/romstage.h>
#include <variant/ec.h>
#include <variant/gpio.h>

#include "spd/spd_util.h"
#include "spd/spd.h"

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg = &mupd->FspmConfig;

#ifdef EC_ENABLE_KEYBOARD_BACKLIGHT
	/* Turn on keyboard backlight to indicate we are booting */
	if (!acpi_is_wakeup_s3())
		google_chromeec_kbbacklight(25);
#endif

	/* Get SPD index */
	const gpio_t spd_gpios[] = {
		GPIO_MEM_CONFIG_0,
		GPIO_MEM_CONFIG_1,
		GPIO_MEM_CONFIG_2,
		GPIO_MEM_CONFIG_3,
	};
	const int spd_idx = gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));

	mem_cfg->MemorySpdDataLen = SPD_SIZE_MAX_DDR3;
	mem_cfg->DqPinsInterleaved = FALSE;

	spd_memory_init_params(mupd, spd_idx);
	variant_memory_init_params(mupd, spd_idx);
}
