/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 secunet Security Networks AG
 * Copyright (C) 2017 Tobias Diedrich <ranma+coreboot@tdiedrich.de>
 * Copyright (C) 2018 Iru Cai <mytbk920423@gmail.com>
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

#undef SUPERIO_DEV
#undef SUPERIO_PNP_BASE
#define SUPERIO_DEV		SIO0
#define SUPERIO_PNP_BASE	0x2e

#define NCT6776_SHOW_SP1	1
#define NCT6776_SHOW_KBC	1

#include "superio/nuvoton/nct6776/acpi/superio.asl"
