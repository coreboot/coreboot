/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <device/device.h>
#include <ec/compal/ene932/ec.h>
#include "ec.h"

#include <southbridge/intel/bd82x6x/pch.h>
#include <soc/nvs.h>
#include "thermal.h"
#include "onboard.h"

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	/* Disable USB ports in S3 by default */
	gnvs->s3u0 = 0;
	gnvs->s3u1 = 0;

	/* Disable USB ports in S5 by default */
	gnvs->s5u0 = 0;
	gnvs->s5u1 = 0;

	if (CONFIG(CHROMEOS) && !parrot_ec_running_ro())
		gnvs_set_ecfw_rw();

	/* EC handles all active thermal and fan control on Parrot. */
	gnvs->tcrt = CRITICAL_TEMPERATURE;
	gnvs->tpsv = PASSIVE_TEMPERATURE;

	// the lid is open by default.
	gnvs->lids = 1;

	// set trackpad IRQ
	if (parrot_rev() < 0x2) {		/* DVT vs PVT */
		gnvs->tpiq = BOARD_TRACKPAD_IRQ_DVT;
	} else {
		gnvs->tpiq = BOARD_TRACKPAD_IRQ_PVT;
	}
}
