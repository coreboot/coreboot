/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <mainboard/ramstage.h>
#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	/*
	 * Disable C10 reporting over eSPI to prevent LED flicker
	 * during S0ix entry/exit on MTL laptops.
	 */
	params->PchEspiHostC10ReportEnable = 0;

	/* Auto-notify iGPU of display changes on USB-C (for hotplug) */
	params->TcNotifyIgd = 2;
}

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init
};
