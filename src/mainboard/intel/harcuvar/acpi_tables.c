/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <types.h>
#include <arch/acpi.h>
#include <arch/smp/mpspec.h>
#include <device/device.h>
#include <cpu/x86/msr.h>

#include <intelblocks/acpi.h>
#include <soc/acpi.h>
#include <soc/nvs.h>

extern const unsigned char AmlCode[];

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	acpi_init_gnvs(gnvs);

	/* Disable USB ports in S5 */
	gnvs->s5u0 = 0;
	gnvs->s5u1 = 0;

	/* TPM Present */
	gnvs->tpmp = 0;
}
