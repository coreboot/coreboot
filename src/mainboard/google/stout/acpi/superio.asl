/* SPDX-License-Identifier: GPL-2.0-only */

/* mainboard configuration */
#include "../ec.h"

#define SIO_EC_ENABLE_PS2K       // Enable PS/2 Keyboard
#define SIO_ENABLE_PS2M          // Enable PS/2 Mouse

/* ACPI code for EC SuperIO functions */
#include <ec/quanta/it8518/acpi/superio.asl>
