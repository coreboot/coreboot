/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <string.h>
#include <southbridge/intel/i82801jx/nvs.h>

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	memset((void *)gnvs, 0, sizeof(*gnvs));

	gnvs->pwrs = 1;    /* Power state (AC = 1) */
	gnvs->osys = 2002; /* At least WINXP SP2 (HPET fix) */
	gnvs->apic = 1;    /* Enable APIC */
	gnvs->mpen = 1;    /* Enable Multi Processing */
	gnvs->cmap = 0x01; /* Enable COM 1 port */
}
