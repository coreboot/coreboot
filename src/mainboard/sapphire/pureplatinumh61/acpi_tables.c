/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_gnvs.h>
#include <southbridge/intel/bd82x6x/nvs.h>

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	gnvs->tcrt = 100;
	gnvs->tpsv = 90;
}
