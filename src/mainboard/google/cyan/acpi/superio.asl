/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

/* mainboard configuration */
#include <ec.h>
#include <variant/onboard.h>

#define SIO_EC_MEMMAP_ENABLE     /* EC Memory Map Resources */
#define SIO_EC_HOST_ENABLE       /* EC Host Interface Resources */
#define SIO_EC_ENABLE_PS2K       /* Enable PS/2 Keyboard */

/* Override default IRQ settings */
#define SIO_EC_PS2K_IRQ GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullDefault,,\
				 "\\_SB.GPNC") { BOARD_I8042_GPIO_INDEX }

/* ACPI code for EC SuperIO functions */
#include <ec/google/chromeec/acpi/superio.asl>
