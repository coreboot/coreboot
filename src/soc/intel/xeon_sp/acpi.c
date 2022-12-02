/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <intelblocks/acpi.h>
#include <soc/pci_devs.h>
#include <soc/util.h>
#include <stdint.h>

#include "chip.h"

/*
 * List of supported C-states in this processor.
 */
enum {
	C_STATE_C1,	/* 0 */
	C_STATE_C3,	/* 1 */
	C_STATE_C6,	/* 2 */
	C_STATE_C7,	/* 3 */
	NUM_C_STATES
};

static const acpi_cstate_t cstate_map[NUM_C_STATES] = {
	[C_STATE_C1] = {
		/* C1 */
		.latency = 1,
		.power = 0x3e8,
		.resource = MWAIT_RES(0, 0),
	},
	[C_STATE_C3] = {
		/* C3 */
		.latency = 15,
		.power = 0x1f4,
		.resource = MWAIT_RES(1, 0),
	},
	[C_STATE_C6] = {
		/* C6 */
		.latency = 41,
		.power = 0x15e,
		.resource = MWAIT_RES(2, 0),
	},
	[C_STATE_C7] = {
		/* C7 */
		.latency = 41,
		.power = 0x0c8,
		.resource = MWAIT_RES(3, 0),
	}
};

/* Max states supported */
static int cstate_set_all[] = {
	C_STATE_C1,
	C_STATE_C3,
	C_STATE_C6,
	C_STATE_C7
};

static int cstate_set_c1_c6[] = {
	C_STATE_C1,
	C_STATE_C6,
};

const acpi_cstate_t *soc_get_cstate_map(size_t *entries)
{
	static acpi_cstate_t map[ARRAY_SIZE(cstate_set_all)];
	int *cstate_set;
	int i;

	const config_t *config = config_of_soc();

	const enum acpi_cstate_mode states = config->cstate_states;

	switch (states) {
	case CSTATES_C1C6:
		*entries = ARRAY_SIZE(cstate_set_c1_c6);
		cstate_set = cstate_set_c1_c6;
		break;
	case CSTATES_ALL:
	default:
		*entries = ARRAY_SIZE(cstate_set_all);
		cstate_set = cstate_set_all;
		break;
	}

	for (i = 0; i < *entries; i++) {
		map[i] = cstate_map[cstate_set[i]];
		map[i].ctype = i + 1;
	}
	return map;
}

static uintptr_t xeonsp_ioapic_bases[CONFIG(XEON_SP_HAVE_IIO_IOAPIC) * 8 + 1];

size_t soc_get_ioapic_info(const uintptr_t *ioapic_bases[])
{
	int index = 0;
	const IIO_UDS *hob = get_iio_uds();

	*ioapic_bases = xeonsp_ioapic_bases;

	for (int socket = 0; socket < hob->PlatformData.numofIIO; ++socket) {
		for (int stack = 0; stack < MAX_IIO_STACK; ++stack) {
			const STACK_RES *ri =
				&hob->PlatformData.IIO_resource[socket].StackRes[stack];
			if (!is_iio_stack_res(ri))
				continue;
			uint32_t ioapic_base = ri->IoApicBase;
			assert(index < ARRAY_SIZE(xeonsp_ioapic_bases));
			xeonsp_ioapic_bases[index++] = ioapic_base;
			if (!CONFIG(XEON_SP_HAVE_IIO_IOAPIC))
				return index;
			/*
			 * Stack 0 has non-PCH IOAPIC and PCH IOAPIC.
			 * The IIO IOAPIC is placed at 0x1000 from the reported base.
			 */
			if (socket == 0 && stack == 0) {
				ioapic_base += 0x1000;
				assert(index < ARRAY_SIZE(xeonsp_ioapic_bases));
				xeonsp_ioapic_bases[index++] = ioapic_base;
			}
		}
	}

	return index;
}
