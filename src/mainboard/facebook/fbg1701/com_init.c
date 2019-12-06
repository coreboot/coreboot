/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 * Copyright (C) 2018-2019 Eltan B.V.
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

#include <bootblock_common.h>
#include <device/pnp_ops.h>

#define SERIAL_DEV PNP_DEV(0x6E, 1) /* ITE8528 UART1 */

void bootblock_mainboard_early_init(void)
{
	/* Enable the serial port inside the EC */
	pnp_set_logical_device(SERIAL_DEV);
	pnp_set_enable(SERIAL_DEV, 1);
}
