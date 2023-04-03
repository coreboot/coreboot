/* SPDX-License-Identifier: GPL-2.0-only */

#undef SUPERIO_DEV
#undef SUPERIO_PNP_BASE
#undef IT8784E_SHOW_UARTA
#undef IT8784E_SHOW_UARTB
#undef IT8784E_SHOW_EC
#undef IT8784E_SHOW_KBC
#undef IT8784E_SHOW_PS2M
#define SUPERIO_DEV		SIO0
#define SUPERIO_PNP_BASE	0x2e
#define IT8784E_SHOW_SP1
#define IT8784E_SHOW_EC
#include <superio/ite/it8784e/acpi/superio.asl>
