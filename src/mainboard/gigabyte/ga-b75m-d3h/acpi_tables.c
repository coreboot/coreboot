/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/bd82x6x/nvs.h>
#include "thermal.h"

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	gnvs->tcrt = CRITICAL_TEMPERATURE;
	gnvs->tpsv = PASSIVE_TEMPERATURE;
}
