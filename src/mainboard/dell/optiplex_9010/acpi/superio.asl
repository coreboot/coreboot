/* SPDX-License-Identifier: GPL-2.0-only */

#undef SUPERIO_DEV
#undef SUPERIO_PNP_BASE
#define SUPERIO_DEV		SIO1
#define SUPERIO_PNP_BASE	0x2e

#define SCH5545_RUNTIME_BASE	0xa00
#define SCH5545_EMI_BASE	0xa40
#define SCH5545_SHOW_UARTA
#define SCH5545_SHOW_KBC

#include <superio/smsc/sch5545/acpi/superio.asl>
