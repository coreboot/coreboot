/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <acpi/acpi_pm.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <intelblocks/acpi_wake_source.h>
#include <intelblocks/acpi.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/pmclib.h>
#include <soc/cpu.h>
#include <soc/msr.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/systemagent.h>
#include <string.h>
#include <types.h>

#include "chip.h"

/*
 * List of supported C-states in this processor.
 */
enum {
	C_STATE_C0,		/* 0 */
	C_STATE_C1,		/* 1 */
	C_STATE_C1E,		/* 2 */
	C_STATE_C3,		/* 3 */
	C_STATE_C6_SHORT_LAT,	/* 4 */
	C_STATE_C6_LONG_LAT,	/* 5 */
	C_STATE_C7_SHORT_LAT,	/* 6 */
	C_STATE_C7_LONG_LAT,	/* 7 */
	C_STATE_C7S_SHORT_LAT,	/* 8 */
	C_STATE_C7S_LONG_LAT,	/* 9 */
	C_STATE_C8,		/* 10 */
	C_STATE_C9,		/* 11 */
	C_STATE_C10,		/* 12 */
	NUM_C_STATES
};
#define MWAIT_RES(state, sub_state)				\
	{							\
		.addrl = (((state) << 4) | (sub_state)),	\
		.space_id = ACPI_ADDRESS_SPACE_FIXED,		\
		.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,	\
		.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,	\
		.access_size = ACPI_FFIXEDHW_FLAG_HW_COORD,	\
	}

static acpi_cstate_t cstate_map[NUM_C_STATES] = {
	[C_STATE_C0] = { },
	[C_STATE_C1] = {
		.latency = 0,
		.power = C1_POWER,
		.resource = MWAIT_RES(0, 0),
	},
	[C_STATE_C1E] = {
		.latency = 0,
		.power = C1_POWER,
		.resource = MWAIT_RES(0, 1),
	},
	[C_STATE_C3] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(0),
		.power = C3_POWER,
		.resource = MWAIT_RES(1, 0),
	},
	[C_STATE_C6_SHORT_LAT] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(1),
		.power = C6_POWER,
		.resource = MWAIT_RES(2, 0),
	},
	[C_STATE_C6_LONG_LAT] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(2),
		.power = C6_POWER,
		.resource = MWAIT_RES(2, 1),
	},
	[C_STATE_C7_SHORT_LAT] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(1),
		.power = C7_POWER,
		.resource = MWAIT_RES(3, 0),
	},
	[C_STATE_C7_LONG_LAT] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(2),
		.power = C7_POWER,
		.resource = MWAIT_RES(3, 1),
	},
	[C_STATE_C7S_SHORT_LAT] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(1),
		.power = C7_POWER,
		.resource = MWAIT_RES(3, 2),
	},
	[C_STATE_C7S_LONG_LAT] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(2),
		.power = C7_POWER,
		.resource = MWAIT_RES(3, 3),
	},
	[C_STATE_C8] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(3),
		.power = C8_POWER,
		.resource = MWAIT_RES(4, 0),
	},
	[C_STATE_C9] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(4),
		.power = C9_POWER,
		.resource = MWAIT_RES(5, 0),
	},
	[C_STATE_C10] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(5),
		.power = C10_POWER,
		.resource = MWAIT_RES(6, 0),
	},
};

static int cstate_set_s0ix[] = {
	C_STATE_C1E,
	C_STATE_C7S_LONG_LAT,
	C_STATE_C10
};

static int cstate_set_non_s0ix[] = {
	C_STATE_C1E,
	C_STATE_C3,
	C_STATE_C7S_LONG_LAT,
};

const acpi_cstate_t *soc_get_cstate_map(size_t *entries)
{
	static acpi_cstate_t map[MAX(ARRAY_SIZE(cstate_set_s0ix),
				ARRAY_SIZE(cstate_set_non_s0ix))];
	int *set;
	int i;

	config_t *config = config_of_soc();

	int is_s0ix_enable = config->s0ix_enable;

	if (is_s0ix_enable) {
		*entries = ARRAY_SIZE(cstate_set_s0ix);
		set = cstate_set_s0ix;
	} else {
		*entries = ARRAY_SIZE(cstate_set_non_s0ix);
		set = cstate_set_non_s0ix;
	}

	for (i = 0; i < *entries; i++) {
		memcpy(&map[i], &cstate_map[set[i]], sizeof(acpi_cstate_t));
		map[i].ctype = i + 1;
	}
	return map;
}

void soc_power_states_generation(int core_id, int cores_per_package)
{
	config_t *config = config_of_soc();

	/* Generate P-state tables */
	if (config->eist_enable)
		generate_p_state_entries(core_id, cores_per_package);
}

uint32_t soc_read_sci_irq_select(void)
{
	return read32p(soc_read_pmc_base() + IRQ_REG);
}

void soc_fill_gnvs(struct global_nvs *gnvs)
{
	const struct soc_intel_skylake_config *config = config_of_soc();

	/* Enable DPTF based on mainboard configuration */
	gnvs->dpte = config->dptf_enable;

	/* Set USB2/USB3 wake enable bitmaps. */
	gnvs->u2we = config->usb2_wake_enable_bitmap;
	gnvs->u3we = config->usb3_wake_enable_bitmap;
}

static unsigned long soc_fill_dmar(unsigned long current)
{
	const u32 gfx_vtbar = MCHBAR32(GFXVTBAR) & ~0xfff;
	const bool gfxvten = MCHBAR32(GFXVTBAR) & 1;

	/* iGFX has to be enabled, GFXVTBAR set and in 32-bit space. */
	const bool emit_igd =
			is_devfn_enabled(SA_DEVFN_IGD) &&
			gfx_vtbar && gfxvten &&
			!MCHBAR32(GFXVTBAR + 4);

	/* First, add DRHD entries */
	if (emit_igd) {
		const unsigned long tmp = current;

		current += acpi_create_dmar_drhd(current, 0, 0, gfx_vtbar);
		current += acpi_create_dmar_ds_pci(current, 0, 2, 0);

		acpi_dmar_drhd_fixup(tmp, current);
	}

	const u32 vtvc0bar = MCHBAR32(VTVC0BAR) & ~0xfff;
	const bool vtvc0en = MCHBAR32(VTVC0BAR) & 1;

	/* General VTBAR has to be set and in 32-bit space. */
	if (vtvc0bar && vtvc0en && !MCHBAR32(VTVC0BAR + 4)) {
		const unsigned long tmp = current;

		current += acpi_create_dmar_drhd(current, DRHD_INCLUDE_PCI_ALL, 0, vtvc0bar);

		current += acpi_create_dmar_ds_ioapic(current, 2, V_P2SB_IBDF_BUS,
						      V_P2SB_IBDF_DEV, V_P2SB_IBDF_FUN);

		current += acpi_create_dmar_ds_msi_hpet(current, 0, V_P2SB_HBDF_BUS,
							V_P2SB_HBDF_DEV, V_P2SB_HBDF_FUN);

		acpi_dmar_drhd_fixup(tmp, current);
	}

	/* Then, add RMRR entries after all DRHD entries */
	if (emit_igd) {
		const unsigned long tmp = current;

		current += acpi_create_dmar_rmrr(current, 0,
				sa_get_gsm_base(), sa_get_tolud_base() - 1);
		current += acpi_create_dmar_ds_pci(current, 0, 2, 0);
		acpi_dmar_rmrr_fixup(tmp, current);
	}

	return current;
}

unsigned long sa_write_acpi_tables(const struct device *const dev,
				   unsigned long current,
				   struct acpi_rsdp *const rsdp)
{
	acpi_dmar_t *const dmar = (acpi_dmar_t *)current;

	/* Create DMAR table only if we have VT-d capability. */
	if (!soc_vtd_enabled())
		return current;

	printk(BIOS_DEBUG, "ACPI:    * DMAR\n");
	acpi_create_dmar(dmar, DMAR_INTR_REMAP, soc_fill_dmar);
	current += dmar->header.length;
	current = acpi_align_current(current);
	acpi_add_table(rsdp, dmar);

	return current;
}

int soc_madt_sci_irq_polarity(int sci)
{
	if (sci >= 20)
		return MP_IRQ_POLARITY_LOW;
	else
		return MP_IRQ_POLARITY_HIGH;
}

void acpi_fill_soc_wake(uint32_t *pm1_en, uint32_t *gpe0_en,
			const struct chipset_power_state *ps)
{
	const struct soc_intel_skylake_config *config = config_of_soc();

	if (ps->prev_sleep_state == ACPI_S3 && deep_s3_enabled()) {
		if (config->deep_sx_config & DSX_EN_LAN_WAKE_PIN)
			gpe0_en[GPE_STD] |= LAN_WAK_EN;
		if (config->deep_sx_config & DSX_EN_WAKE_PIN)
			*pm1_en |= PCIEXPWAK_STS;
	}
}
