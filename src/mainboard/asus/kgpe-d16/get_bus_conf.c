 /*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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

void get_bus_conf(void)
{
	get_default_pci1234(1);

	sysconf.sbdn = (sysconf.hcdn[0] & 0xff);

	pirq_router_bus = (sysconf.pci1234[0] >> 16) & 0xff;
}
