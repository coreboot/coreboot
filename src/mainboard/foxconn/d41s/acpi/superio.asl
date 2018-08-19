/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 secunet Security Networks AG
 * Copyright (C) 2017 Samuel Holland <samuel@sholland.org>
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
#undef IT8721F_SHOW_SP1
#undef IT8721F_SHOW_SP2
#undef IT8721F_SHOW_EC
#undef IT8721F_SHOW_KBCK
#undef IT8721F_SHOW_KBCM
#define SUPERIO_DEV		SIO0
#define SUPERIO_PNP_BASE	0x2e
#define IT8721F_SHOW_SP1
#define IT8721F_SHOW_SP2
#define IT8721F_SHOW_EC
#define IT8721F_SHOW_KBCK
#define IT8721F_SHOW_KBCM
#include <superio/ite/it8721f/acpi/superio.asl>
