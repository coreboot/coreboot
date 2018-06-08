/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google, Inc.
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

#include <cbmem.h>
#include <device/device.h>
#include <symbols.h>

static void mainboard_enable(device_t dev)
{
	uintptr_t ram_base;
	size_t ram_size;

	/* FIXME: These values shouldn't necessarily be hardcoded */
	ram_base = 0x80000000;
	ram_size = 128 * MiB;
	ram_resource(dev, 0, ram_base / KiB, ram_size / KiB);

	cbmem_initialize_empty();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
