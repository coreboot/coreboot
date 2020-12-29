/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <acpi/acpigen.h>
#include <device/mmio.h>
#include <arch/smp/mpspec.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/acpi.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/soc_chip.h>
#include <soc/systemagent.h>
#include <string.h>
#include <types.h>
#include <wrdd.h>

/*
 * List of supported C-states in this processor.
 */
enum {
	C_STATE_C0,		/* 0 */
	C_STATE_C1,		/* 1 */
	C_STATE_C1E,		/* 2 */
	C_STATE_C6_SHORT_LAT,	/* 3 */
	C_STATE_C6_LONG_LAT,	/* 4 */
	C_STATE_C7_SHORT_LAT,	/* 5 */
	C_STATE_C7_LONG_LAT,	/* 6 */
	C_STATE_C7S_SHORT_LAT,	/* 7 */
	C_STATE_C7S_LONG_LAT,	/* 8 */
	C_STATE_C8,		/* 9 */
	C_STATE_C9,		/* 10 */
	C_STATE_C10,		/* 11 */
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

static const acpi_cstate_t cstate_map[NUM_C_STATES] = {
	[C_STATE_C0] = {},
	[C_STATE_C1] = {
		.latency = C1_LATENCY,
		.power = C1_POWER,
		.resource = MWAIT_RES(0, 0),
	},
	[C_STATE_C1E] = {
		.latency = C1_LATENCY,
		.power = C1_POWER,
		.resource = MWAIT_RES(0, 1),
	},
	[C_STATE_C6_SHORT_LAT] = {
		.latency = C6_LATENCY,
		.power = C6_POWER,
		.resource = MWAIT_RES(2, 0),
	},
	[C_STATE_C6_LONG_LAT] = {
		.latency = C6_LATENCY,
		.power = C6_POWER,
		.resource = MWAIT_RES(2, 1),
	},
	[C_STATE_C7_SHORT_LAT] = {
		.latency = C7_LATENCY,
		.power = C7_POWER,
		.resource = MWAIT_RES(3, 0),
	},
	[C_STATE_C7_LONG_LAT] = {
		.latency = C7_LATENCY,
		.power = C7_POWER,
		.resource = MWAIT_RES(3, 1),
	},
	[C_STATE_C7S_SHORT_LAT] = {
		.latency = C7_LATENCY,
		.power = C7_POWER,
		.resource = MWAIT_RES(3, 2),
	},
	[C_STATE_C7S_LONG_LAT] = {
		.latency = C7_LATENCY,
		.power = C7_POWER,
		.resource = MWAIT_RES(3, 3),
	},
	[C_STATE_C8] = {
		.latency = C8_LATENCY,
		.power = C8_POWER,
		.resource = MWAIT_RES(4, 0),
	},
	[C_STATE_C9] = {
		.latency = C9_LATENCY,
		.power = C9_POWER,
		.resource = MWAIT_RES(5, 0),
	},
	[C_STATE_C10] = {
		.latency = C10_LATENCY,
		.power = C10_POWER,
		.resource = MWAIT_RES(6, 0),
	},
};

static int cstate_set_non_s0ix[] = {
	C_STATE_C1,
	C_STATE_C6_LONG_LAT,
	C_STATE_C7S_LONG_LAT
};

static int cstate_set_s0ix[] = {
	C_STATE_C1,
	C_STATE_C7S_LONG_LAT,
	C_STATE_C10
};

acpi_cstate_t *soc_get_cstate_map(size_t *entries)
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

	if (config->eist_enable)
		/* Generate P-state tables */
		generate_p_state_entries(core_id, cores_per_package);
}

void soc_fill_fadt(acpi_fadt_t *fadt)
{
	const uint16_t pmbase = ACPI_BASE_ADDRESS;

	config_t *config = config_of_soc();

	fadt->pm_tmr_blk = pmbase + PM1_TMR;
	fadt->pm_tmr_len = 4;
	fadt->x_pm_tmr_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm_tmr_blk.bit_width = fadt->pm_tmr_len * 8;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm_tmr_blk.addrl = pmbase + PM1_TMR;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	if (config->s0ix_enable)
		fadt->flags |= ACPI_FADT_LOW_PWR_IDLE_S0;
}

uint32_t soc_read_sci_irq_select(void)
{
	return read32((void *)soc_read_pmc_base() + IRQ_REG);
}

static unsigned long soc_fill_dmar(unsigned long current)
{
	const struct device *const igfx_dev = pcidev_path_on_root(SA_DEVFN_IGD);
	const uint64_t gfxvtbar = MCHBAR64(GFXVTBAR) & VTBAR_MASK;
	const bool gfxvten = MCHBAR32(GFXVTBAR) & VTBAR_ENABLED;

	if (is_dev_enabled(igfx_dev) && gfxvtbar && gfxvten) {
		const unsigned long tmp = current;

		current += acpi_create_dmar_drhd(current, 0, 0, gfxvtbar);
		current += acpi_create_dmar_ds_pci(current, 0, 2, 0);

		acpi_dmar_drhd_fixup(tmp, current);
	}

	const struct device *const ipu_dev = pcidev_path_on_root(SA_DEVFN_IPU);
	const uint64_t ipuvtbar = MCHBAR64(IPUVTBAR) & VTBAR_MASK;
	const bool ipuvten = MCHBAR32(IPUVTBAR) & VTBAR_ENABLED;

	if (is_dev_enabled(ipu_dev) && ipuvtbar && ipuvten) {
		const unsigned long tmp = current;

		current += acpi_create_dmar_drhd(current, 0, 0, ipuvtbar);
		current += acpi_create_dmar_ds_pci(current, 0, 5, 0);

		acpi_dmar_drhd_fixup(tmp, current);
	}

	const uint64_t vtvc0bar = MCHBAR64(VTVC0BAR) & VTBAR_MASK;
	const bool vtvc0en = MCHBAR32(VTVC0BAR) & VTBAR_ENABLED;

	if (vtvc0bar && vtvc0en) {
		const unsigned long tmp = current;

		current += acpi_create_dmar_drhd(current,
				DRHD_INCLUDE_PCI_ALL, 0, vtvc0bar);
		current += acpi_create_dmar_ds_ioapic(current,
				2, V_P2SB_CFG_IBDF_BUS, V_P2SB_CFG_IBDF_DEV,
				V_P2SB_CFG_IBDF_FUNC);
		current += acpi_create_dmar_ds_msi_hpet(current,
				0, V_P2SB_CFG_HBDF_BUS, V_P2SB_CFG_HBDF_DEV,
				V_P2SB_CFG_HBDF_FUNC);

		acpi_dmar_drhd_fixup(tmp, current);
	}

	/* TCSS Thunderbolt root ports */
	for (unsigned int i = 0; i < MAX_TBT_PCIE_PORT; i++) {
		const struct device *const tbt_dev = pcidev_path_on_root(SA_DEVFN_TBT(i));
		if (is_dev_enabled(tbt_dev)) {
			const uint64_t tbtbar = MCHBAR64(TBTxBAR(i)) & VTBAR_MASK;
			const bool tbten = MCHBAR32(TBTxBAR(i)) & VTBAR_ENABLED;
			if (tbtbar && tbten) {
				const unsigned long tmp = current;

				current += acpi_create_dmar_drhd(current, 0, 0, tbtbar);
				current += acpi_create_dmar_ds_pci_br(current, 0, 7, i);

				acpi_dmar_drhd_fixup(tmp, current);
			}
		}
	}

	/* Add RMRR entry */
	if (is_dev_enabled(igfx_dev)) {
		const unsigned long tmp = current;
		current += acpi_create_dmar_rmrr(current, 0,
				sa_get_gsm_base(), sa_get_tolud_base() - 1);
		current += acpi_create_dmar_ds_pci(current, 0, 2, 0);
		acpi_dmar_rmrr_fixup(tmp, current);
	}

	return current;
}

unsigned long sa_write_acpi_tables(const struct device *dev, unsigned long current,
				   struct acpi_rsdp *rsdp)
{
	acpi_dmar_t *const dmar = (acpi_dmar_t *)current;

	/*
	 * Create DMAR table only if we have VT-d capability and FSP does not override its
	 * feature.
	 */
	if ((pci_read_config32(dev, CAPID0_A) & VTD_DISABLE) ||
	    !(MCHBAR32(VTVC0BAR) & VTBAR_ENABLED))
		return current;

	printk(BIOS_DEBUG, "ACPI:    * DMAR\n");
	acpi_create_dmar(dmar, DMAR_INTR_REMAP | DMA_CTRL_PLATFORM_OPT_IN_FLAG, soc_fill_dmar);
	current += dmar->header.length;
	current = acpi_align_current(current);
	acpi_add_table(rsdp, dmar);

	return current;
}

void soc_fill_gnvs(struct global_nvs *gnvs)
{
	config_t *config = config_of_soc();

	/* Set unknown wake source */
	gnvs->pm1i = -1;

	/* Enable DPTF based on mainboard configuration */
	gnvs->dpte = config->dptf_enable;

	/* Fill in the Wifi Region id */
	gnvs->cid1 = wifi_regulatory_domain();

	/* Set USB2/USB3 wake enable bitmaps. */
	gnvs->u2we = config->usb2_wake_enable_bitmap;
	gnvs->u3we = config->usb3_wake_enable_bitmap;

	/* Fill in Above 4GB MMIO resource */
	sa_fill_gnvs(gnvs);
}

uint32_t acpi_fill_soc_wake(uint32_t generic_pm1_en,
			    const struct chipset_power_state *ps)
{
	/*
	 * WAK_STS bit is set when the system is in one of the sleep states
	 * (via the SLP_EN bit) and an enabled wake event occurs. Upon setting
	 * this bit, the PMC will transition the system to the ON state and
	 * can only be set by hardware and can only be cleared by writing a one
	 * to this bit position.
	 */

	generic_pm1_en |= WAK_STS | RTC_EN | PWRBTN_EN;
	return generic_pm1_en;
}

int soc_madt_sci_irq_polarity(int sci)
{
	return MP_IRQ_POLARITY_HIGH;
}

static int acpigen_soc_gpio_op(const char *op, unsigned int gpio_num)
{
	/* op (gpio_num) */
	acpigen_emit_namestring(op);
	acpigen_write_integer(gpio_num);
	return 0;
}

static int acpigen_soc_get_gpio_state(const char *op, unsigned int gpio_num)
{
	/* Store (op (gpio_num), Local0) */
	acpigen_write_store();
	acpigen_soc_gpio_op(op, gpio_num);
	acpigen_emit_byte(LOCAL0_OP);
	return 0;
}

int acpigen_soc_read_rx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_get_gpio_state("\\_SB.PCI0.GRXS", gpio_num);
}

int acpigen_soc_get_tx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_get_gpio_state("\\_SB.PCI0.GTXS", gpio_num);
}

int acpigen_soc_set_tx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_gpio_op("\\_SB.PCI0.STXS", gpio_num);
}

int acpigen_soc_clear_tx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_gpio_op("\\_SB.PCI0.CTXS", gpio_num);
}
