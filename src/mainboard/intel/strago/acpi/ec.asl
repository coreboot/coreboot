/* SPDX-License-Identifier: GPL-2.0-only */

/* mainboard configuration */
#include "ec.h"

/* Enable LID switch */
#define EC_ENABLE_LID_SWITCH

/* ACPI code for EC functions */
#include <ec/google/chromeec/acpi/ec.asl>
