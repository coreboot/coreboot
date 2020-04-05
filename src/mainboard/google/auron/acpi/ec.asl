/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* mainboard configuration */
#include "ec.h"

/* variant configuration */
#include <variant/acpi/ec.asl>

/* Enable LID switch and provide wake pin for EC */
#define EC_ENABLE_LID_SWITCH
/*
 * There is no GPIO for LID, the EC pulses WAKE# pin instead.
 * There is no GPE for WAKE#, so fake it with PCI_EXP_WAKE.
 */
#define EC_ENABLE_WAKE_PIN	0x69

/* ACPI code for EC functions */
#include <ec/google/chromeec/acpi/ec.asl>
