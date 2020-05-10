/* SPDX-License-Identifier: GPL-2.0-only */

/* mainboard configuration */
#include "../ec.h"

#define SIO_EC_MEMMAP_ENABLE     // EC Memory Map Resources
#define SIO_EC_HOST_ENABLE       // EC Host Interface Resources
#define SIO_EC_ENABLE_PS2K       // Enable PS/2 Keyboard
#define SIO_EC_ENABLE_COM1       // Enable Serial Port 1

/* ACPI code for EC SuperIO functions */
#include <ec/google/chromeec/acpi/superio.asl>
