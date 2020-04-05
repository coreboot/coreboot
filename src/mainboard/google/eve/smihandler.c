/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <cpu/x86/smm.h>
#include <ec/google/chromeec/smm.h>
#include <gpio.h>
#include <intelblocks/smihandler.h>
#include "ec.h"
#include "gpio.h"

void mainboard_smi_espi_handler(void)
{
	chromeec_smi_process_events();
}

static void mainboard_gpio_smi_sleep(u8 slp_typ)
{
	/* Power down the rails on any sleep type */
	gpio_set(EN_PP3300_DX_CAM, 0);

	/* Assert TOUSHCREEN_STOP_L */
	gpio_set(GPP_E11, 0);

	/* Turn off touchscreen power */
	gpio_set(GPP_C22, 0);
}

void mainboard_smi_sleep(u8 slp_typ)
{
	chromeec_smi_sleep(slp_typ, MAINBOARD_EC_S3_WAKE_EVENTS,
			   MAINBOARD_EC_S5_WAKE_EVENTS);
	chromeec_smi_device_event_sleep(slp_typ,
					MAINBOARD_EC_S3_DEVICE_EVENTS, 0);
	mainboard_gpio_smi_sleep(slp_typ);
}

int mainboard_smi_apmc(u8 apmc)
{
	chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS,
			  MAINBOARD_EC_SMI_EVENTS);
	return 0;
}
