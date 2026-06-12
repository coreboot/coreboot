/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <ec/ec.h>
#include <mainboard/ramstage.h>
#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	/*
	 * Disable C10 reporting over eSPI to prevent LED flicker
	 * during S0ix entry/exit on laptops.
	 */
	params->PchEspiHostC10ReportEnable = 0;

	/* Auto-notify iGPU of display changes on USB-C (for hotplug) */
	params->TcNotifyIgd = 2;

	/*
	 * CNVi RF_RESET# and CLKREQ routed to GPP_F04/GPP_F05, not the FSP
	 * default of GPP_A8/GPP_A9, see FspsUpd.h for defaults and values.
	 */
	params->CnviRfResetPinMux = 0x194CE404; /* GPP_F04 */
	params->CnviClkreqPinMux = 0x394CE605;  /* GPP_F05 */
}

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
	mainboard_ec_init();
}

struct chip_operations mainboard_ops = {.init = mainboard_init};
