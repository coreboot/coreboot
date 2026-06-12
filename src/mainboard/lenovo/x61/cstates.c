/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <cpu/intel/speedstep.h>

static const acpi_cstate_t cst_entries[] = {
	{
		/* ACPI C1 / CPU C1 */
		1, 0x01, 1000,
		{ ACPI_ADDRESS_SPACE_FIXED, 1, 2, 1, 0, 0 }
	},
	{
		/* ACPI C2 / CPU C2 */
		2, 0x01,  500,
		{ ACPI_ADDRESS_SPACE_FIXED, 1, 2, 1, 0x10, 0 }
	},
	{
		/* ACPI C3 / CPU C3 */
		3, 0x11,  250,
		{ ACPI_ADDRESS_SPACE_FIXED, 1, 2, 3, 0x20, 0 }
	},
};

static const acpi_cstate_t bat_cst_entries[] = {
	{
		/* ACPI C1 / CPU C1 */
		1, 0x01, 1000,
		{ ACPI_ADDRESS_SPACE_FIXED, 1, 2, 1, 0, 0 }
	},
	{
		/* ACPI C2 / CPU C2 */
		2, 0x01,  500,
		{ ACPI_ADDRESS_SPACE_FIXED, 1, 2, 1, 0x10, 0 }
	},
	{
		/* ACPI C3 / CPU C4 */
		3, 0x39,  100,
		{ ACPI_ADDRESS_SPACE_FIXED, 1, 2, 3, 0x30, 0 }
	},
};

int get_cst_entries(const acpi_cstate_t **entries)
{
	*entries = cst_entries;
	return ARRAY_SIZE(cst_entries);
}

int get_bat_cst_entries(const acpi_cstate_t **entries)
{
	*entries = bat_cst_entries;
	return ARRAY_SIZE(bat_cst_entries);
}
