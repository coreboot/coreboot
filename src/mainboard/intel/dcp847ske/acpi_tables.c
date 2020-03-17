/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <southbridge/intel/bd82x6x/nvs.h>
#include "thermal.h"

void acpi_create_gnvs(global_nvs_t *gnvs)
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
