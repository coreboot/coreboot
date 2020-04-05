/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* mainboard configuration */
#include <mainboard/google/rambi/ec.h>

#include <variant/onboard.h>

/* Enable LID switch and provide wake pin for EC */
#define EC_ENABLE_LID_SWITCH
#define EC_ENABLE_WAKE_PIN	BOARD_PCH_WAKE_GPIO

/* ACPI code for EC functions */
#include <ec/google/chromeec/acpi/ec.asl>
