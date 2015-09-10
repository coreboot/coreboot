/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 secunet Security Networks AG
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
#undef IT8783EF_SHOW_UARTA
#undef IT8783EF_SHOW_UARTB
#undef IT8783EF_SHOW_UARTC
#undef IT8783EF_SHOW_UARTD
#undef IT8783EF_SHOW_KBC
#undef IT8783EF_SHOW_PS2M
#define SUPERIO_DEV		SIO0
#define SUPERIO_PNP_BASE	0x2e
#define IT8783EF_SHOW_UARTA	1
#define IT8783EF_SHOW_UARTB	1
#define IT8783EF_SHOW_UARTC	1
#define IT8783EF_SHOW_UARTD	1
#include <superio/ite/it8783ef/acpi/superio.asl>
