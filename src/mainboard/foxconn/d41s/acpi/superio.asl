/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
