/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>

__weak void mainboard_update_soc_chip_config(struct soc_intel_pantherlake_config *config)
{
	/* Override settings per board. */
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	/* Override settings per board. */
}

/* Mainboard GPIO Configuration */
__weak void mainboard_silicon_init_params(FSP_S_CONFIG *s_cfg)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
