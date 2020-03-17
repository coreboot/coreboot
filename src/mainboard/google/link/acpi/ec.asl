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

/* mainboard configuration */
#include "../ec.h"

/* Enable EC backed Keyboard Backlight in ACPI */
#define EC_ENABLE_KEYBOARD_BACKLIGHT

/* Enable LID switch and provide wake pin for EC */
#define EC_ENABLE_LID_SWITCH

/* EC_LID_OUT is GPIO15 */
#define EC_ENABLE_WAKE_PIN	0x1f

/* ACPI code for EC functions */
#include <ec/google/chromeec/acpi/ec.asl>
