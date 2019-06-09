/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Vlado Cibic <vladocb@protonmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <southbridge/intel/bd82x6x/nvs.h>
#include <option.h>

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	/* Turn off power for USB ports in S3 by default */
	gnvs->s3u0 = 0;
	gnvs->s3u1 = 0;

	/* Turn off power for USB ports in S5 by default */
	gnvs->s5u0 = 0;
	gnvs->s5u1 = 0;

	/* critical temp that will shutdown the pc == 95C degrees */
	gnvs->tcrt = 95;

	/* temp to start throttling the cpu == 85C */
	gnvs->tpsv = 85;
}
