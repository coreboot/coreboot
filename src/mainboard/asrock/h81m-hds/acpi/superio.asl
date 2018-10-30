/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Tristan Corrick <tristan@corrick.kiwi>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define SUPERIO_DEV		SIO0
#define SUPERIO_PNP_BASE	0x2e
#define NCT6776_SHOW_PP
#define NCT6776_SHOW_SP1
#define NCT6776_SHOW_KBC
#define NCT6776_SHOW_HWM

#undef NCT6776_SHOW_GPIO

#include <superio/nuvoton/nct6776/acpi/superio.asl>
