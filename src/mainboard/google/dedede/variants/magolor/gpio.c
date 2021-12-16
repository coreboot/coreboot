/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <bootstate.h>
#include <fw_config.h>
#include <commonlib/helpers.h>

/* Pad configuration of stylus */
static const struct pad_config stylus_det_pads[] = {
	/* C12 : AP_PEN_DET_ODL (external pull-high) */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_C12, NONE, DEEP),
};

static void fw_config_handle(void *unused)
{
	if (fw_config_probe(FW_CONFIG(STYLUS, STYLUS_PRESENT)))
		gpio_configure_pads(stylus_det_pads, ARRAY_SIZE(stylus_det_pads));
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
