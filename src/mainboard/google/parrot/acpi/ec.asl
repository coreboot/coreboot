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
#define EC_SCI_GPE 23      // GPIO7 << 16 to GPE bit for Runtime SCI

/* ACPI code for EC functions */
#include "../../../../ec/compal/ene932/acpi/ec.asl"
