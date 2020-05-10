/* SPDX-License-Identifier: GPL-2.0-only */

/* mainboard configuration */
#include "../ec.h"
#define EC_SCI_GPE 23      // GPIO7 << 16 to GPE bit for Runtime SCI

/* ACPI code for EC functions */
#include "../../../../ec/compal/ene932/acpi/ec.asl"
