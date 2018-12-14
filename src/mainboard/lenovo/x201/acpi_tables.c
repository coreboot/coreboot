/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2013 Vladimir Serbinenko <phcoder@gmail.com>
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

#include <southbridge/intel/ibexpeak/nvs.h>
#include "thermal.h"

static void acpi_update_thermal_table(global_nvs_t *gnvs)
{
	gnvs->tcrt = CRITICAL_TEMPERATURE;
	gnvs->tpsv = PASSIVE_TEMPERATURE;
}

void acpi_create_gnvs(global_nvs_t * gnvs)
{
	acpi_update_thermal_table(gnvs);
}
