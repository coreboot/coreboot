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
#include "../gpio.h"

/* Enable EC backed ALS device in ACPI */
#define EC_ENABLE_ALS_DEVICE

/* Enable EC backed PD MCU device in ACPI */
#define EC_ENABLE_PD_MCU_DEVICE

/* Enable LID switch and provide wake pin for EC */
#define EC_ENABLE_LID_SWITCH
#define EC_ENABLE_WAKE_PIN	GPE_EC_WAKE

/* ACPI code for EC functions */
#include <ec/google/chromeec/acpi/ec.asl>
