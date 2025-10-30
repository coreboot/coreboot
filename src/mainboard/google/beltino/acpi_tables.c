/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <device/device.h>
#include <soc/nvs.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <variant/thermal.h>

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	/* TPM Present */
	gnvs->tpmp = 1;

	gnvs->tcrt = CRITICAL_TEMPERATURE;
	gnvs->tpsv = PASSIVE_TEMPERATURE;
	gnvs->tmax = MAX_TEMPERATURE;
}
