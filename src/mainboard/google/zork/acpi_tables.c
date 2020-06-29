/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <soc/nvs.h>
#include <variant/thermal.h>

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	gnvs->tmps = CTL_TDP_SENSOR_ID;
	gnvs->tcrt = CRITICAL_TEMPERATURE;
	gnvs->tpsv = PASSIVE_TEMPERATURE;
}
