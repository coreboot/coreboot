/*
 * This file is part of the coreboot project.
 *
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
#define SUPERIO_PNP_BASE	0x4e

#if !CONFIG(DISABLE_UART_ON_TESTPADS)
#define NCT6776_SHOW_SP1	1
#endif
#define NCT6776_SHOW_HWM	1
#define NCT6776_SHOW_GPIO	1

#include <superio/nuvoton/nct6776/acpi/superio.asl>
