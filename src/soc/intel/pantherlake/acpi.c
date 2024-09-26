/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <acpi/acpigen.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <fw_config.h>
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
#include <types.h>

/*
 * TODO: Update as per PTL spec.
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

/* TODO: Update as per PTL spec. */
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

/* TODO: Update as per PTL spec */
static int cstate_set_non_s0ix[] = {
	C_STATE_C1,
	C_STATE_C6_LONG_LAT,
	C_STATE_C7S_LONG_LAT
};

/* TODO: Update as per PTL spec */
static int cstate_set_s0ix[] = {
	C_STATE_C1,
	C_STATE_C6_LONG_LAT,
	C_STATE_C10
};

const acpi_cstate_t *soc_get_cstate_map(size_t *entries)
{
	static acpi_cstate_t map[MAX(ARRAY_SIZE(cstate_set_s0ix),
				ARRAY_SIZE(cstate_set_non_s0ix))];
	static bool c_state_initialized = false;
	size_t i;

	if (c_state_initialized)
		return map;

	const struct soc_intel_pantherlake_config *config = config_of_soc();
	if (config == NULL) {
		printk(BIOS_ERR, "Error: Configuration could not be retrieved.\n");
		return NULL;
	}

	int *set;
	if (config->s0ix_enable) {
		*entries = ARRAY_SIZE(cstate_set_s0ix);
		set = cstate_set_s0ix;
	} else {
		*entries = ARRAY_SIZE(cstate_set_non_s0ix);
		set = cstate_set_non_s0ix;
	}

	for (i = 0; i < *entries; i++) {
		map[i] = cstate_map[set[i]];
		map[i].ctype = i + 1;
	}
	c_state_initialized = true;

	return map;
}

void soc_power_states_generation(int core_id, int cores_per_package)
{
	const struct soc_intel_pantherlake_config *config = config_of_soc();
	if (config == NULL) {
		printk(BIOS_ERR, "Error: Configuration could not be retrieved.\n");
		return;
	}
	if (config->eist_enable)
		/* Generate P-state tables */
		generate_p_state_entries(core_id, cores_per_package);
}

void soc_fill_fadt(acpi_fadt_t *fadt)
{
	const uint16_t pmbase = ACPI_BASE_ADDRESS;

	const struct soc_intel_pantherlake_config *config = config_of_soc();
	if (config == NULL) {
		printk(BIOS_ERR, "Error: Configuration could not be retrieved.\n");
		return;
	}
	fadt->pm_tmr_blk = pmbase + PM1_TMR;
	fadt->pm_tmr_len = sizeof(uint32_t);

	fill_fadt_extended_pm_io(fadt);

	if (config->s0ix_enable)
		fadt->flags |= ACPI_FADT_LOW_PWR_IDLE_S0;
}

static struct min_sleep_state min_pci_sleep_states[] = {
	{ SA_DEVFN_ROOT,	ACPI_DEVICE_SLEEP_D3 },
	{ SA_DEVFN_IGD,		ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_IPU,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_TBT0,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_TBT1,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_TBT2,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_TBT3,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_TCSS_XHCI,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_TCSS_XDCI,	ACPI_DEVICE_SLEEP_D3 },
	{ SA_DEVFN_TCSS_DMA0,	ACPI_DEVICE_SLEEP_D3 },
	{ SA_DEVFN_TCSS_DMA1,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_THC0,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_THC1,	ACPI_DEVICE_SLEEP_D3 },
	{ PCH_DEVFN_XHCI,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_USBOTG,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_SRAM,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_CNVI_WIFI,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_I2C0,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_I2C1,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_I2C2,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_I2C3,	ACPI_DEVICE_SLEEP_D3 },
	{ PCH_DEVFN_CSE,	ACPI_DEVICE_SLEEP_D0 },
	{ PCI_DEVFN_I2C4,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_I2C5,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_UART2,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_PCIE1,	ACPI_DEVICE_SLEEP_D0 },
	{ PCI_DEVFN_PCIE2,	ACPI_DEVICE_SLEEP_D0 },
	{ PCI_DEVFN_PCIE3,	ACPI_DEVICE_SLEEP_D0 },
	{ PCI_DEVFN_PCIE4,	ACPI_DEVICE_SLEEP_D0 },
	{ PCI_DEVFN_PCIE5,	ACPI_DEVICE_SLEEP_D0 },
	{ PCI_DEVFN_PCIE6,	ACPI_DEVICE_SLEEP_D0 },
	{ PCI_DEVFN_PCIE7,	ACPI_DEVICE_SLEEP_D0 },
	{ PCI_DEVFN_PCIE8,	ACPI_DEVICE_SLEEP_D0 },
	{ PCI_DEVFN_PCIE9,	ACPI_DEVICE_SLEEP_D0 },
	{ PCI_DEVFN_PCIE10,	ACPI_DEVICE_SLEEP_D0 },
	{ PCI_DEVFN_PCIE11,	ACPI_DEVICE_SLEEP_D0 },
	{ PCI_DEVFN_PCIE12,	ACPI_DEVICE_SLEEP_D0 },
	{ PCI_DEVFN_UART0,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_UART1,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_GSPI0,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_GSPI1,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_ESPI,	ACPI_DEVICE_SLEEP_D0 },
	{ PCH_DEVFN_PMC,	ACPI_DEVICE_SLEEP_D0 },
	{ PCI_DEVFN_HDA,	ACPI_DEVICE_SLEEP_D0 },
	{ PCI_DEVFN_SPI,	ACPI_DEVICE_SLEEP_D3 },
	{ PCI_DEVFN_GBE,	ACPI_DEVICE_SLEEP_D3 },
};

struct min_sleep_state *soc_get_min_sleep_state_array(size_t *size)
{
	*size = ARRAY_SIZE(min_pci_sleep_states);
	return min_pci_sleep_states;
}

uint32_t soc_read_sci_irq_select(void)
{
	return read32p(soc_read_pmc_base() + IRQ_REG);
}

static unsigned long soc_fill_dmar(unsigned long current)
{
	uint32_t vtd_engine_enabled = MCHBAR32(GFXVTBAR);
	const uint64_t gfxvtbar = MCHBAR64(GFXVTBAR) & VTBAR_MASK;
	bool is_ipu_enabled = is_devfn_enabled(PCI_DEVFN_IPU);
	bool is_dptf_enabled = is_devfn_enabled(PCI_DEVFN_DPTF);
	bool is_npu_enabled = is_devfn_enabled(PCI_DEVFN_NPU);
	bool is_iaa_enabled = is_devfn_enabled(PCI_DEVFN_IAA);

	printk(BIOS_DEBUG, "%s - gfxvtbar:0x%llx  0x%x\n", __func__, gfxvtbar, MCHBAR32(GFXVTBAR));
	if (vtd_engine_enabled & GFXVT_ENABLED) {
		const unsigned long tmp = current;
		current += acpi_create_dmar_drhd(current, 0, 0, gfxvtbar, GFXVT_BASE_SIZE);
		current += acpi_create_dmar_ds_pci(current, 0, PCI_DEV_SLOT_IGD, 0);

		acpi_dmar_drhd_fixup(tmp, current);
	}

	if ((is_ipu_enabled || is_dptf_enabled || is_npu_enabled || is_iaa_enabled) &&
		(vtd_engine_enabled & NONGFXVT_ENABLED)) {
		const unsigned long tmp = current;
		current += acpi_create_dmar_drhd(current,
			0, 0, (uint64_t)VTVC0_BASE_ADDRESS, VTVC0_BASE_SIZE);
		if (is_ipu_enabled)
			current += acpi_create_dmar_ds_pci(current, 0, PCI_DEV_SLOT_IPU, 0);
		if (is_dptf_enabled)
			current += acpi_create_dmar_ds_pci(current, 0, PCI_DEV_SLOT_DPTF, 0);
		if (is_npu_enabled)
			current += acpi_create_dmar_ds_pci(current, 0, PCI_DEV_SLOT_NPU, 0);
		if (is_iaa_enabled)
			current += acpi_create_dmar_ds_pci(current, 0, PCI_DEV_SLOT_IAA, 0);

		acpi_dmar_drhd_fixup(tmp, current);
	}

	if (vtd_engine_enabled & IOCVT_ENABLED) {
		const unsigned long tmp = current;
		current += acpi_create_dmar_drhd(current,
			DRHD_INCLUDE_PCI_ALL, 0, (uint64_t)IOCVTD_BASE_ADDRESS, IOCVTD_BASE_SIZE);
		current += acpi_create_dmar_ds_ioapic_from_hw(current,
			IO_APIC_ADDR, V_P2SB_CFG_IBDF_BUS, V_P2SB_CFG_IBDF_DEV, V_P2SB_CFG_IBDF_FUNC);
		current += acpi_create_dmar_ds_msi_hpet(current, 0, V_P2SB_CFG_HBDF_BUS,
			V_P2SB_CFG_HBDF_DEV, V_P2SB_CFG_HBDF_FUNC);

		acpi_dmar_drhd_fixup(tmp, current);
	}

	/* Add RMRR entry */
	if (vtd_engine_enabled & GFXVT_ENABLED) {
		const unsigned long tmp = current;
		current += acpi_create_dmar_rmrr(current, 0,
				sa_get_gsm_base(), sa_get_tolud_base() - 1);
		current += acpi_create_dmar_ds_pci(current, 0, PCI_DEV_SLOT_IGD, 0);

		acpi_dmar_rmrr_fixup(tmp, current);
	}

	if (is_ipu_enabled || is_dptf_enabled || is_npu_enabled || is_iaa_enabled) {
		const unsigned long tmp = current;
		current += acpi_create_dmar_satc(current, ATC_REQUIRED, 0);
			current += acpi_create_dmar_ds_pci(current, 0, PCI_DEV_SLOT_IGD, 0);
		if (is_dptf_enabled)
			current += acpi_create_dmar_ds_pci(current, 0, PCI_DEV_SLOT_DPTF, 0);
		if (is_ipu_enabled)
			current += acpi_create_dmar_ds_pci(current, 0, PCI_DEV_SLOT_IPU, 0);
		if (is_npu_enabled)
			current += acpi_create_dmar_ds_pci(current, 0, PCI_DEV_SLOT_NPU, 0);
		if (is_iaa_enabled)
			current += acpi_create_dmar_ds_pci(current, 0, PCI_DEV_SLOT_IAA, 0);

		acpi_dmar_satc_fixup(tmp, current);
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
	    !(MCHBAR32(GFXVTBAR) & (GFXVT_ENABLED | NONGFXVT_ENABLED | IOCVT_ENABLED)))
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
	const struct soc_intel_pantherlake_config *config = config_of_soc();
	if (config == NULL) {
		printk(BIOS_ERR, "Configuration could not be retrieved.\n");
		return;
	}
	/* Enable DPTF based on mainboard configuration */
	gnvs->dpte = config->dptf_enable;

	/* Set USB2/USB3 wake enable bitmaps. */
	gnvs->u2we = config->usb2_wake_enable_bitmap;
	gnvs->u3we = config->usb3_wake_enable_bitmap;
}

int soc_madt_sci_irq_polarity(int sci)
{
	return MP_IRQ_POLARITY_HIGH;
}
