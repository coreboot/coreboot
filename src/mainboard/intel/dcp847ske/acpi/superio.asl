/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
