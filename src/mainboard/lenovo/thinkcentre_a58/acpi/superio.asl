/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
