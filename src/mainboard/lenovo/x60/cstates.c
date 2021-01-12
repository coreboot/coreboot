/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <southbridge/intel/i82801gx/i82801gx.h>

#define MWAIT_RES(state, sub_state)				\
	{							\
		.space_id = ACPI_ADDRESS_SPACE_FIXED,		\
		.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,	\
		.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,	\
		.access_size = ACPI_ACCESS_SIZE_UNDEFINED,	\
		.addrl = (((state) << 4) | (sub_state)),	\
		.addrh = 0,					\
	}

static acpi_cstate_t cst_entries[] = {
	{
		.ctype = 1,
		.latency = 1,
		.power = 1000,
		.resource = MWAIT_RES(0, 0),
	},
	{
		.ctype = 2,
		.latency = 1,
		.power = 500,
		.resource = MWAIT_RES(1, 0),
	},
	{
		.ctype = 3,
		.latency = 17,
		.power = 250,
		.resource = MWAIT_RES(2, 0),
	},
};

int get_cst_entries(acpi_cstate_t **entries)
{
	*entries = cst_entries;
	return ARRAY_SIZE(cst_entries);
}
