/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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

/* Enable LID switch and provide wake pin for EC */
#define EC_ENABLE_LID_SWITCH
/*
 * There is no GPIO for LID, the EC pulses WAKE# pin instead.
 * There is no GPE for WAKE#, so fake it with PCI_EXP_WAKE.
 */
#define EC_ENABLE_WAKE_PIN	0x69

/* Slippy variants have a throttle handler in thermal.asl */
#define EC_ENABLE_THROTTLING_HANDLER

/* ACPI code for EC functions */
#include <ec/google/chromeec/acpi/ec.asl>
