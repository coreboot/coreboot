/*
 * This file is part of the coreboot project.
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
#undef SCH5147_SHOW_UARTA
#undef SCH5147_SHOW_UARTB
#undef SCH5147_SHOW_KBC
#undef SCH5147_SHOW_HWMON
#define SUPERIO_DEV		SIO0
#define SUPERIO_PNP_BASE	0x2e
#define SCH5147_SHOW_UARTA
#define SCH5147_SHOW_UARTB
#define SCH5147_SHOW_KBC
#include <superio/smsc/sch5147/acpi/superio.asl>
