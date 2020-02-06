/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Vladimir Serbinenko
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

#include <southbridge/intel/lynxpoint/nvs.h>

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	/* The lid is open by default. */
	gnvs->lids = 1;

	/* Temperature at which OS will shutdown. */
	gnvs->tcrt = 100;
	/* Temperature at which OS will throttle CPU. */
	gnvs->tpsv = 90;
}
