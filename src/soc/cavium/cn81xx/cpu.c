/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018-present  Facebook, Inc.
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

#include <types.h>
#include <arch/io.h>
#include <soc/cpu.h>
#include <bdk-coreboot.h>

/* Return the number of cores available in the chip */
size_t cpu_get_num_cores(void)
{
	uint64_t available = read64((void *)0x87e006001738ll);
	return bdk_dpop(available);
}
