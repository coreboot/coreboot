/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* mainboard configuration */
#include "../ec.h"
#include <variant/ec.h>
#include <variant/gpio.h>

/* Enable EC backed PD MCU device in ACPI */
#define EC_ENABLE_PD_MCU_DEVICE

/* Enable LID switch and provide wake pin for EC */
#define EC_ENABLE_LID_SWITCH
#define EC_ENABLE_WAKE_PIN	GPE_EC_WAKE

/* ACPI code for EC functions */
#include <ec/google/chromeec/acpi/ec.asl>
