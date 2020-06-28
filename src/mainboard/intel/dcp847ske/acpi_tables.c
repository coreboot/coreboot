/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <southbridge/intel/bd82x6x/nvs.h>
#include "thermal.h"

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	/* Enable USB ports in S3 by default */
	gnvs->s3u0 = 1;
	gnvs->s3u1 = 1;

	/* Enable USB ports in S5 by default */
	gnvs->s5u0 = 1;
	gnvs->s5u1 = 1;

	gnvs->tcrt = CRITICAL_TEMPERATURE;
	gnvs->tpsv = PASSIVE_TEMPERATURE;
}
