/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <drivers/vpd/vpd.h>
#include <drivers/ocp/include/vpd.h>
#include "include/spr_ws_2_gpio.h"
#include <bootstate.h>

void mainboard_silicon_init_params(FSPS_UPD *params)
{
	/* configure Emmitsburg PCH GPIO controller after FSP-M */
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

static void finalize_boot(void *unused)
{
	printk(BIOS_DEBUG, "FM_BIOS_POST_CMPLT_N cleared.\n");
	/* Clear FM_BIOS_POST_CMPLT_N */
	gpio_output(GPPC_C17, 0);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, finalize_boot, NULL);
