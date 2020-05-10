/* SPDX-License-Identifier: GPL-2.0-only */

/* mainboard configuration */
#include "ec.h"
#include "onboard.h"

#define SIO_EC_MEMMAP_ENABLE     /* EC Memory Map Resources */
#define SIO_EC_HOST_ENABLE       /* EC Host Interface Resources */
#define SIO_EC_ENABLE_PS2K       /* Enable PS/2 Keyboard */

/* Override default IRQ settings */
#define SIO_EC_PS2K_IRQ Interrupt(ResourceConsumer, Edge, ActiveLow){\
                                                     BOARD_I8042_IRQ}

/* ACPI code for EC SuperIO functions */
#include <ec/google/chromeec/acpi/superio.asl>
