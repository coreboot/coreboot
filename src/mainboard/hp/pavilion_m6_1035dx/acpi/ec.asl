/*
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

/*
 * EC bits specific to the mainboard
 */
#define EC_SCI 3
/* TODO: We do not yet know how the LID is connected to the platform */
#define EC_ACPI_LID_SWITCH_OBJECT	Zero

/* ACPI code for EC functions */
#include <ec/compal/ene932/acpi/ec.asl>
