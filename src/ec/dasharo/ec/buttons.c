/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/acpi/ec.h>

#include "acpi.h"

/**
 * Return the state of lid switch.
 *
 * @return 1 if the lid is open.
 */
int dasharo_ec_get_lid_state(void)
{
	return ec_read(DASHARO_EC_REG_LSTE) & DASHARO_EC_REG_LSTE_LID_STATE;
}
