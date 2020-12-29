/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <soc/nvs.h>
#include "thermal.h"

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	/* Disable USB ports in S3 by default */
	gnvs->s3u0 = 0;
	gnvs->s3u1 = 0;

	/* Disable USB ports in S5 by default */
	gnvs->s5u0 = 0;
	gnvs->s5u1 = 0;

	// TODO: MLR
	// The firmware read/write status is a "virtual" switch and
	// will be handled elsewhere.  Until then hard-code to
	// read/write instead of read-only for developer mode.
	if (CONFIG(CHROMEOS))
		gnvs_set_ecfw_rw();

	// the lid is open by default.
	gnvs->lids = 1;

	/* EC handles all thermal and fan control on Butterfly. */
	gnvs->tcrt = CRITICAL_TEMPERATURE;
	gnvs->tpsv = PASSIVE_TEMPERATURE;
}
