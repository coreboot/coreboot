/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
