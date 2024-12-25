/* SPDX-License-Identifier: GPL-2.0-only */

#undef SUPERIO_DEV
#undef SUPERIO_PNP_BASE
#define SUPERIO_DEV		SIO0
#define SUPERIO_PNP_BASE	0x2e

#define NCT6776_SHOW_SP1	1
#define NCT6776_SHOW_KBC	1

#include "superio/nuvoton/nct6776/acpi/superio.asl"
