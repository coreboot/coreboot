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

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/i82801gx/nvs.h>
#include <southbridge/intel/common/pmutil.h>

#define GPE_EC_SCI	12

int mainboard_smi_apmc(u8 data)
{
	switch (data) {
		case APM_CNT_ACPI_ENABLE:
			/* route H8SCI to SCI */
			gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SCI);
			break;
		case APM_CNT_ACPI_DISABLE:
			/* route H8SCI# to SMI */
			gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SMI);
			break;
		default:
			break;
	}
	return 0;
}
