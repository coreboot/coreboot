/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <device/device.h>
#include <vendorcode/google/chromeos/gnvs.h>
#include <ec/google/chromeec/ec.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <southbridge/intel/lynxpoint/nvs.h>

#include "thermal.h"

void acpi_create_gnvs(struct global_nvs *gnvs)
{
	/* Enable USB ports in S3 */
	gnvs->s3u0 = 1;
	gnvs->s3u1 = 1;

	/* Disable USB ports in S5 */
	gnvs->s5u0 = 0;
	gnvs->s5u1 = 0;

	/* TPM Present */
	gnvs->tpmp = 1;

#if CONFIG(CHROMEOS)
	gnvs->chromeos.vbt2 = google_ec_running_ro() ?
		ACTIVE_ECFW_RO : ACTIVE_ECFW_RW;
#endif

	gnvs->tmps = TEMPERATURE_SENSOR_ID;
	gnvs->tcrt = CRITICAL_TEMPERATURE;
	gnvs->tpsv = PASSIVE_TEMPERATURE;
	gnvs->tmax = MAX_TEMPERATURE;
	gnvs->f0pw = EC_THROTTLE_POWER_LIMIT;
	gnvs->flvl = 1;
}
