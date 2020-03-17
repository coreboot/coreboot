/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
