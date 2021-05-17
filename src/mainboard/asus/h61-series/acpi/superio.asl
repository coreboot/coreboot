/* SPDX-License-Identifier: GPL-2.0-or-later */

#if CONFIG(BOARD_ASUS_P8H61_M_LX)

#define SUPERIO_DEV		SIO0
#define SUPERIO_PNP_BASE	0x2e
#define NCT6776_SHOW_PP
#define NCT6776_SHOW_SP1
#define NCT6776_SHOW_KBC
#define NCT6776_SHOW_HWM

#undef NCT6776_SHOW_GPIO

#include <superio/nuvoton/nct6776/acpi/superio.asl>

#else /* !BOARD_ASUS_P8H61_M_LX */
#include <drivers/pc80/pc/ps2_controller.asl>
#endif
