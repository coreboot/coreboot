#include <device/device.h>
#include <arch/x86/include/arch/acpigen.h>
#include <southbridge/intel/i82801gx/i82801gx.h>

static acpi_cstate_t cst_entries[] = {
	{
		/* acpi C1 / cpu C1 */
		1, 0x01, 1000,
		{ ACPI_ADDRESS_SPACE_FIXED, 1, 2, { 1 }, 0, 0 }
	},
	{
		/* acpi C2 / cpu C2 */
		2, 0x01,  500,
		{ ACPI_ADDRESS_SPACE_IO, 8, 0, { 0 }, DEFAULT_PMBASE + LV2, 0 }
	},
	{
		/* acpi C3 / cpu C2 */
		2, 0x11,  250,
		{ ACPI_ADDRESS_SPACE_IO, 8, 0, { 0 }, DEFAULT_PMBASE + LV3, 0 }
	},
};

int get_cst_entries(acpi_cstate_t **entries)
{
	*entries = cst_entries;
	return ARRAY_SIZE(cst_entries);
}
