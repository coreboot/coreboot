/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <acpi/acpi_pm.h>
#include <acpi/acpigen.h>
#include <arch/cpu.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/common/common.h>
#include <cpu/intel/turbo.h>
#include <intelblocks/acpi_wake_source.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/sgx.h>
#include <intelblocks/uart.h>
#include <intelblocks/systemagent.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/systemagent.h>
#include <string.h>
#include <types.h>
#include <device/pci_ops.h>

#include "chip.h"

#define  CPUID_6_EAX_ISST	(1 << 7)

/*
 * List of suported C-states in this processor.
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

static int get_cores_per_package(void)
{
	struct cpuinfo_x86 c;
	struct cpuid_result result;
	int cores = 1;

	get_fms(&c, cpuid_eax(1));
	if (c.x86 != 6)
		return 1;

	result = cpuid_ext(0xb, 1);
	cores = result.ebx & 0xff;

	return cores;
}

void soc_fill_gnvs(struct global_nvs *gnvs)
{
	const struct soc_intel_skylake_config *config = config_of_soc();

	/* Enable DPTF based on mainboard configuration */
	gnvs->dpte = config->dptf_enable;

	/* Set USB2/USB3 wake enable bitmaps. */
	gnvs->u2we = config->usb2_wake_enable_bitmap;
	gnvs->u3we = config->usb3_wake_enable_bitmap;

	if (CONFIG(SOC_INTEL_COMMON_BLOCK_SGX_ENABLE))
		sgx_fill_gnvs(gnvs);

	/* Fill in Above 4GB MMIO resource */
	sa_fill_gnvs(gnvs);
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current,
					     CONFIG_MMCONF_BASE_ADDRESS, 0, 0,
					     CONFIG_MMCONF_BUS_NUMBER - 1);
	return current;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				2, IO_APIC_ADDR, 0);

	return acpi_madt_irq_overrides(current);
}

static void write_c_state_entries(acpi_cstate_t *map, const int *set, size_t max_c_state)
{
	for (size_t i = 0; i < max_c_state; i++) {
		memcpy(&map[i], &cstate_map[set[i]], sizeof(acpi_cstate_t));
		map[i].ctype = i + 1;
	}

	/* Generate C-state tables */
	acpigen_write_CST_package(map, max_c_state);
}

static void generate_c_state_entries(int s0ix_enable)
{
	if (s0ix_enable) {
		acpi_cstate_t map[ARRAY_SIZE(cstate_set_s0ix)];
		write_c_state_entries(map, cstate_set_s0ix, ARRAY_SIZE(map));
	} else {
		acpi_cstate_t map[ARRAY_SIZE(cstate_set_non_s0ix)];
		write_c_state_entries(map, cstate_set_non_s0ix, ARRAY_SIZE(map));
	}
}

static int calculate_power(int tdp, int p1_ratio, int ratio)
{
	u32 m;
	u32 power;

	/*
	 * M = ((1.1 - ((p1_ratio - ratio) * 0.00625)) / 1.1) ^ 2
	 *
	 * Power = (ratio / p1_ratio) * m * tdp
	 */

	m = (110000 - ((p1_ratio - ratio) * 625)) / 11;
	m = (m * m) / 1000;

	power = ((ratio * 100000 / p1_ratio) / 100);
	power *= (m / 100) * (tdp / 1000);
	power /= 1000;

	return (int)power;
}

static void generate_p_state_entries(int core, int cores_per_package)
{
	int ratio_min, ratio_max, ratio_turbo, ratio_step;
	int coord_type, power_max, power_unit, num_entries;
	int ratio, power, clock, clock_max;
	msr_t msr;

	/* Determine P-state coordination type from MISC_PWR_MGMT[0] */
	msr = rdmsr(MSR_MISC_PWR_MGMT);
	if (msr.lo & MISC_PWR_MGMT_EIST_HW_DIS)
		coord_type = SW_ANY;
	else
		coord_type = HW_ALL;

	/* Get bus ratio limits and calculate clock speeds */
	msr = rdmsr(MSR_PLATFORM_INFO);
	ratio_min = (msr.hi >> (40-32)) & 0xff; /* Max Efficiency Ratio */

	/* Determine if this CPU has configurable TDP */
	if (cpu_config_tdp_levels()) {
		/* Set max ratio to nominal TDP ratio */
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		ratio_max = msr.lo & 0xff;
	} else {
		/* Max Non-Turbo Ratio */
		ratio_max = (msr.lo >> 8) & 0xff;
	}
	clock_max = ratio_max * CONFIG_CPU_BCLK_MHZ;

	/* Calculate CPU TDP in mW */
	msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 2 << ((msr.lo & 0xf) - 1);
	msr = rdmsr(MSR_PKG_POWER_SKU);
	power_max = ((msr.lo & 0x7fff) / power_unit) * 1000;

	/* Write _PCT indicating use of FFixedHW */
	acpigen_write_empty_PCT();

	/* Write _PPC with no limit on supported P-state */
	acpigen_write_PPC_NVS();

	/* Write PSD indicating configured coordination type */
	acpigen_write_PSD_package(core, 1, coord_type);

	/* Add P-state entries in _PSS table */
	acpigen_write_name("_PSS");

	/* Determine ratio points */
	ratio_step = PSS_RATIO_STEP;
	num_entries = ((ratio_max - ratio_min) / ratio_step) + 1;
	if (num_entries > PSS_MAX_ENTRIES) {
		ratio_step += 1;
		num_entries = ((ratio_max - ratio_min) / ratio_step) + 1;
	}

	/* P[T] is Turbo state if enabled */
	if (get_turbo_state() == TURBO_ENABLED) {
		/* _PSS package count including Turbo */
		acpigen_write_package(num_entries + 2);

		msr = rdmsr(MSR_TURBO_RATIO_LIMIT);
		ratio_turbo = msr.lo & 0xff;

		/* Add entry for Turbo ratio */
		acpigen_write_PSS_package(
			clock_max + 1,		/* MHz */
			power_max,		/* mW */
			PSS_LATENCY_TRANSITION,	/* lat1 */
			PSS_LATENCY_BUSMASTER,	/* lat2 */
			ratio_turbo << 8,	/* control */
			ratio_turbo << 8);	/* status */
	} else {
		/* _PSS package count without Turbo */
		acpigen_write_package(num_entries + 1);
	}

	/* First regular entry is max non-turbo ratio */
	acpigen_write_PSS_package(
		clock_max,		/* MHz */
		power_max,		/* mW */
		PSS_LATENCY_TRANSITION,	/* lat1 */
		PSS_LATENCY_BUSMASTER,	/* lat2 */
		ratio_max << 8,		/* control */
		ratio_max << 8);	/* status */

	/* Generate the remaining entries */
	for (ratio = ratio_min + ((num_entries - 1) * ratio_step);
	     ratio >= ratio_min; ratio -= ratio_step) {

		/* Calculate power at this ratio */
		power = calculate_power(power_max, ratio_max, ratio);
		clock = ratio * CONFIG_CPU_BCLK_MHZ;

		acpigen_write_PSS_package(
			clock,			/* MHz */
			power,			/* mW */
			PSS_LATENCY_TRANSITION,	/* lat1 */
			PSS_LATENCY_BUSMASTER,	/* lat2 */
			ratio << 8,		/* control */
			ratio << 8);		/* status */
	}

	/* Fix package length */
	acpigen_pop_len();
}

static void generate_cppc_entries(int core_id)
{
	/* Generate GCPC table in first logical core */
	if (core_id == 0) {
		struct cppc_config cppc_config;
		cpu_init_cppc_config(&cppc_config, CPPC_VERSION_2);
		acpigen_write_CPPC_package(&cppc_config);
	}

	/* Write _CST entry for each logical core */
	acpigen_write_CPPC_method();
}

void generate_cpu_entries(const struct device *device)
{
	int core_id, cpu_id, pcontrol_blk = ACPI_BASE_ADDRESS, plen = 6;
	int totalcores = dev_count_cpu();
	int cores_per_package = get_cores_per_package();
	int numcpus = totalcores/cores_per_package;
	config_t *config = config_of_soc();
	int is_s0ix_enable = config->s0ix_enable;
	const bool isst_supported = cpuid_eax(6) & CPUID_6_EAX_ISST;

	printk(BIOS_DEBUG, "Found %d CPU(s) with %d core(s) each.\n",
	       numcpus, cores_per_package);

	for (cpu_id = 0; cpu_id < numcpus; cpu_id++) {
		for (core_id = 0; core_id < cores_per_package; core_id++) {
			if (core_id > 0) {
				pcontrol_blk = 0;
				plen = 0;
			}

			/* Generate processor \_SB.CPUx */
			acpigen_write_processor(
				cpu_id*cores_per_package+core_id,
				pcontrol_blk, plen);
			/* Generate C-state tables */
			generate_c_state_entries(is_s0ix_enable);

			if (config->eist_enable) {
				/* Generate P-state tables */
				generate_p_state_entries(core_id,
						cores_per_package);
			}

			if (isst_supported)
				generate_cppc_entries(core_id);

			acpigen_pop_len();
		}
	}

	/* PPKG is usually used for thermal management
	   of the first and only package. */
	acpigen_write_processor_package("PPKG", 0, cores_per_package);

	/* Add a method to notify processor nodes */
	acpigen_write_processor_cnot(cores_per_package);
}

static unsigned long acpi_fill_dmar(unsigned long current)
{
	struct device *const igfx_dev = pcidev_path_on_root(SA_DEVFN_IGD);
	const u32 gfx_vtbar = MCHBAR32(GFXVTBAR) & ~0xfff;
	const bool gfxvten = MCHBAR32(GFXVTBAR) & 1;

	/* iGFX has to be enabled, GFXVTBAR set and in 32-bit space. */
	const bool emit_igd =
			igfx_dev && igfx_dev->enabled &&
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

unsigned long northbridge_write_acpi_tables(const struct device *const dev,
					    unsigned long current,
					    struct acpi_rsdp *const rsdp)
{
	acpi_dmar_t *const dmar = (acpi_dmar_t *)current;

	/* Create DMAR table only if we have VT-d capability. */
	if (!soc_is_vtd_capable())
		return current;

	printk(BIOS_DEBUG, "ACPI:    * DMAR\n");
	acpi_create_dmar(dmar, DMAR_INTR_REMAP, acpi_fill_dmar);
	current += dmar->header.length;
	current = acpi_align_current(current);
	acpi_add_table(rsdp, dmar);

	return current;
}

unsigned long acpi_madt_irq_overrides(unsigned long current)
{
	int sci = acpi_sci_irq();
	acpi_madt_irqoverride_t *irqovr;
	uint16_t flags = MP_IRQ_TRIGGER_LEVEL;

	/* INT_SRC_OVR */
	irqovr = (void *)current;
	current += acpi_create_madt_irqoverride(irqovr, 0, 0, 2, 0);

	if (sci >= 20)
		flags |= MP_IRQ_POLARITY_LOW;
	else
		flags |= MP_IRQ_POLARITY_HIGH;

	/* SCI */
	irqovr = (void *)current;
	current += acpi_create_madt_irqoverride(irqovr, 0, sci, sci, flags);

	/* NMI */
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)current, 0xff, 5, 1);

	return current;
}

unsigned long southbridge_write_acpi_tables(const struct device *device,
					     unsigned long current,
					     struct acpi_rsdp *rsdp)
{
	current = acpi_write_dbg2_pci_uart(rsdp, current,
					   uart_get_device(),
					   ACPI_ACCESS_SIZE_DWORD_ACCESS);
	current = acpi_write_hpet(device, current, rsdp);
	return acpi_align_current(current);
}

/* Save wake source information for calculating ACPI _SWS values */
int soc_fill_acpi_wake(const struct chipset_power_state *ps, uint32_t *pm1, uint32_t **gpe0)
{
	const struct soc_intel_skylake_config *config = config_of_soc();
	static uint32_t gpe0_sts[GPE0_REG_MAX];
	uint32_t pm1_en;
	uint32_t gpe0_std;
	int i;
	const int last_index = GPE0_REG_MAX - 1;

	pm1_en = ps->pm1_en;
	gpe0_std = ps->gpe0_en[3];

	/*
	 * Chipset state in the suspend well (but not RTC) is lost in Deep S3
	 * so enable Deep S3 wake events that are configured by the mainboard
	 */
	if (ps->prev_sleep_state == ACPI_S3 &&
	    (config->deep_s3_enable_ac || config->deep_s3_enable_dc)) {
		pm1_en |= PWRBTN_STS; /* Always enabled as wake source */
		if (config->deep_sx_config & DSX_EN_LAN_WAKE_PIN)
			gpe0_std |= LAN_WAK_EN;
		if (config->deep_sx_config & DSX_EN_WAKE_PIN)
			pm1_en |= PCIEXPWAK_STS;
	}

	*pm1 = ps->pm1_sts & pm1_en;

	/* Mask off GPE0 status bits that are not enabled */
	*gpe0 = &gpe0_sts[0];
	for (i = 0; i < last_index; i++)
		gpe0_sts[i] = ps->gpe0_sts[i] & ps->gpe0_en[i];
	gpe0_sts[last_index] = ps->gpe0_sts[last_index] & gpe0_std;

	return GPE0_REG_MAX;
}
