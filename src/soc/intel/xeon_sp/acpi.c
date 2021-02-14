/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <intelblocks/acpi.h>
#include <soc/pci_devs.h>
#include <soc/util.h>

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

acpi_cstate_t *soc_get_cstate_map(size_t *entries)
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

static void print_madt_ioapic(int socket, int stack,
				     int ioapic_id, uint32_t ioapic_base, int gsi_base)
{
	printk(BIOS_DEBUG, "Adding MADT IOAPIC for socket: %d, stack: %d, ioapic_id: 0x%x, "
	       "ioapic_base: 0x%x, gsi_base: 0x%x\n",
	       socket, stack,  ioapic_id, ioapic_base, gsi_base);
	return;
}

const struct madt_ioapic_info *soc_get_ioapic_info(size_t *entries)
{
	int cur_index;
	const IIO_UDS *hob = get_iio_uds();

	/* With XEON-SP FSP, PCH IOAPIC is allocated with first 120 GSIs. */
#if (CONFIG(SOC_INTEL_COOPERLAKE_SP))
	const int gsi_bases[] = { 0, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0 };
#endif

#if (CONFIG(SOC_INTEL_SKYLAKE_SP))
	const int gsi_bases[] = { 0, 0x18, 0x20, 0x28, 0x30, 0x48, 0x50, 0x58, 0x60 };
#endif

	static struct madt_ioapic_info madt_tbl[ARRAY_SIZE(gsi_bases)];

	cur_index = 0;
	madt_tbl[cur_index].id  = PCH_IOAPIC_ID;
	madt_tbl[cur_index].addr = hob->PlatformData.IIO_resource[0].StackRes[0].IoApicBase;
	madt_tbl[cur_index].gsi_base = gsi_bases[cur_index];
	print_madt_ioapic(0, 0,  madt_tbl[cur_index].id,
			  madt_tbl[cur_index].addr, madt_tbl[cur_index].gsi_base);
	++cur_index;

	for (int socket = 0; socket < hob->PlatformData.numofIIO; ++socket) {
		for (int stack = 0; stack < MAX_IIO_STACK; ++stack) {
			const STACK_RES *ri =
				&hob->PlatformData.IIO_resource[socket].StackRes[stack];
			if (!is_iio_stack_res(ri))
				continue;
			assert(cur_index < ARRAY_SIZE(gsi_bases));
			madt_tbl[cur_index].id = soc_get_iio_ioapicid(socket, stack);
			madt_tbl[cur_index].gsi_base = gsi_bases[cur_index];
			madt_tbl[cur_index].addr = ri->IoApicBase;

			/*
			 * Stack 0 has non-PCH IOAPIC and PCH IOAPIC.
			 * The IIO IOAPIC is placed at 0x1000 from the reported base.
			 */
			if (stack == 0 && socket == 0)
				madt_tbl[cur_index].addr += 0x1000;

			print_madt_ioapic(socket, stack, madt_tbl[cur_index].id,
					  madt_tbl[cur_index].addr,
					  madt_tbl[cur_index].gsi_base);
			++cur_index;
		}
	}

	*entries = cur_index;
	return madt_tbl;
}
