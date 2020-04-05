/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* Baseboard configuration */
#include <mainboard/google/rambi/ec.h>

#define SIO_EC_MEMMAP_ENABLE     // EC Memory Map Resources
#define SIO_EC_HOST_ENABLE       // EC Host Interface Resources

/* Variant configuration */
#include <variant/onboard.h>

/* Override default IRQ settings */
#define SIO_EC_PS2K_IRQ Interrupt(ResourceConsumer, Edge, ActiveLow) {BOARD_I8042_IRQ}

/* ACPI code for EC SuperIO functions */
#include <ec/google/chromeec/acpi/superio.asl>
