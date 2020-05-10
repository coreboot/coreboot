/* SPDX-License-Identifier: GPL-2.0-only */

/* mainboard configuration */
#include "../ec.h"

#define SIO_EC_ENABLE_PS2K       // Enable PS/2 Keyboard

/* ACPI code for EC SuperIO functions */
#include <ec/quanta/ene_kb3940q/acpi/superio.asl>
