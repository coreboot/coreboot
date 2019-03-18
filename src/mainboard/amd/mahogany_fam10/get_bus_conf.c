/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/pci.h>
#include <stdint.h>
#include <stdlib.h>
#include <cpu/amd/multicore.h>

#include <cpu/amd/amdfam10_sysconf.h>

/* Global variables for MB layouts and these will be shared by irqtable mptable
* and acpi_tables busnum is default.
*/
u32 apicid_sb700;

void get_bus_conf(void)
{
	u32 apicid_base;

	get_default_pci1234(1);

	sysconf.sbdn = (sysconf.hcdn[0] & 0xff);

	pirq_router_bus = (sysconf.pci1234[0] >> 16) & 0xff;

	/* I/O APICs:   APIC ID Version State   Address */
	if (CONFIG(LOGICAL_CPUS))
		apicid_base = get_apicid_base(1);
	else
		apicid_base = CONFIG_MAX_PHYSICAL_CPUS;
	apicid_sb700 = apicid_base + 0;
}
