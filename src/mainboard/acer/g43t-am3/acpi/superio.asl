/* SPDX-License-Identifier: GPL-2.0-only */

#undef SUPERIO_DEV
#undef SUPERIO_PNP_BASE
#undef IT8720F_SHOW_SP1
#undef IT8720F_SHOW_SP2
#undef IT8720F_SHOW_EC
#undef IT8720F_SHOW_KBCK
#undef IT8720F_SHOW_KBCM
#undef IT8720F_SHOW_GPIO
#undef IT8720F_SHOW_CIR
#define SUPERIO_DEV		SIO0
#define SUPERIO_PNP_BASE	0x2e
#define IT8720F_SHOW_EC		1
#define IT8720F_SHOW_KBCK	1
#define IT8720F_SHOW_KBCM	1
#define IT8720F_SHOW_GPIO	1
#include <superio/ite/it8720f/acpi/superio.asl>
