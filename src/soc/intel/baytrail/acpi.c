/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <arch/ioapic.h>
#include <device/mmio.h>
#include <arch/smp/mpspec.h>
#include <console/console.h>
#include <types.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/turbo.h>

#include <soc/acpi.h>
#include <soc/iomap.h>
#include <soc/irq.h>
#include <soc/msr.h>
#include <soc/pattrs.h>
#include <soc/pm.h>

#define MWAIT_RES(state, sub_state)                         \
	{                                                   \
		.addrl = (((state) << 4) | (sub_state)),    \
		.space_id = ACPI_ADDRESS_SPACE_FIXED,       \
		.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,    \
		.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,    \
		.access_size = ACPI_FFIXEDHW_FLAG_HW_COORD, \
	}

/* C-state map without S0ix */
static const acpi_cstate_t cstate_map[] = {
	{
		/* C1 */
		.ctype = 1, /* ACPI C1 */
		.latency = 1,
		.power = 1000,
		.resource = MWAIT_RES(0, 0),
	},
	{
		/* C6NS with no L2 shrink */
		/* NOTE: this substate is above CPUID limit */
		.ctype = 2, /* ACPI C2 */
		.latency = 500,
		.power = 10,
		.resource = MWAIT_RES(5, 1),
	},
	{
		/* C6FS with full L2 shrink */
		.ctype = 3, /* ACPI C3 */
		.latency = 1500, /* 1.5ms worst case */
		.power = 1,
		.resource = MWAIT_RES(5, 2),
	}
};

int acpi_sci_irq(void)
{
	u32 *actl = (u32 *)(ILB_BASE_ADDRESS + ACTL);
	int scis;
	static int sci_irq;

	if (sci_irq)
		return sci_irq;

	/* Determine how SCI is routed. */
	scis = read32(actl) & SCIS_MASK;
	switch (scis) {
	case SCIS_IRQ9:
	case SCIS_IRQ10:
	case SCIS_IRQ11:
		sci_irq = scis - SCIS_IRQ9 + 9;
		break;
	case SCIS_IRQ20:
	case SCIS_IRQ21:
	case SCIS_IRQ22:
	case SCIS_IRQ23:
		sci_irq = scis - SCIS_IRQ20 + 20;
		break;
	default:
		printk(BIOS_DEBUG, "Invalid SCI route! Defaulting to IRQ9.\n");
		sci_irq = 9;
		break;
	}

	printk(BIOS_DEBUG, "SCI is IRQ%d\n", sci_irq);
	return sci_irq;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics_with_nmis(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic_from_hw((acpi_madt_ioapic_t *)current, IO_APIC_ADDR);

	current = acpi_madt_irq_overrides(current);

	return current;
}

static acpi_tstate_t soc_tss_table[] = {
	{ 100, 1000, 0, 0x00, 0 },
	{  88,  875, 0, 0x1e, 0 },
	{  75,  750, 0, 0x1c, 0 },
	{  63,  625, 0, 0x1a, 0 },
	{  50,  500, 0, 0x18, 0 },
	{  38,  375, 0, 0x16, 0 },
	{  25,  250, 0, 0x14, 0 },
	{  13,  125, 0, 0x12, 0 },
};

static void generate_t_state_entries(int core, int cores_per_package)
{
	/* Indicate SW_ALL coordination for T-states */
	acpigen_write_TSD_package(core, cores_per_package, SW_ALL);

	/* Indicate FFixedHW so OS will use MSR */
	acpigen_write_empty_PTC();

	/* Set NVS controlled T-state limit */
	acpigen_write_TPC("\\TLVL");

	/* Write TSS table for MSR access */
	acpigen_write_TSS_package(ARRAY_SIZE(soc_tss_table), soc_tss_table);
}

static int calculate_power(int tdp, int p1_ratio, int ratio)
{
	u32 m, power;

	/*
	 * M = ((1.1 - ((p1_ratio - ratio) * 0.00625)) / 1.1) ^ 2
	 */

	m = (110000 - ((p1_ratio - ratio) * 625)) / 11;
	m = (m * m) / 1000;

	/*
	 * Power = (ratio / p1_ratio) * m * TDP
	 */
	power = ((ratio * 100000 / p1_ratio) / 100);
	power *= (m / 100) * (tdp / 1000);
	power /= 1000;

	return (int)power;
}

static void generate_p_state_entries(int core, int cores_per_package)
{
	int ratio_min, ratio_max, ratio_turbo, ratio_step, ratio_range_2;
	int coord_type, power_max, power_unit, num_entries;
	int ratio, power, clock, clock_max;
	int vid, vid_turbo, vid_min, vid_max, vid_range_2;
	u32 control_status;
	const struct pattrs *pattrs = pattrs_get();
	msr_t msr;

	/* Inputs from CPU attributes */
	ratio_max = pattrs->iacore_ratios[IACORE_MAX];
	ratio_min = pattrs->iacore_ratios[IACORE_LFM];
	vid_max = pattrs->iacore_vids[IACORE_MAX];
	vid_min = pattrs->iacore_vids[IACORE_LFM];

	/* Set P-states coordination type based on MSR disable bit */
	coord_type = (pattrs->num_cpus > 2) ? SW_ALL : HW_ALL;

	/* Max Non-Turbo Frequency */
	clock_max = (ratio_max * pattrs->bclk_khz) / 1000;

	/* Calculate CPU TDP in mW */
	msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 1 << (msr.lo & 0xf);
	msr = rdmsr(MSR_PKG_POWER_LIMIT);
	power_max = ((msr.lo & 0x7fff) / power_unit) * 1000;

	/* Write _PCT indicating use of FFixedHW */
	acpigen_write_empty_PCT();

	/* Write _PPC with NVS specified limit on supported P-state */
	acpigen_write_PPC_NVS();

	/* Write PSD indicating configured coordination type */
	acpigen_write_PSD_package(core, 1, coord_type);

	/* Add P-state entries in _PSS table */
	acpigen_write_name("_PSS");

	/* Determine ratio points */
	ratio_step = 1;
	num_entries = (ratio_max - ratio_min) / ratio_step;
	while (num_entries > 15) { /* ACPI max is 15 ratios */
		ratio_step <<= 1;
		num_entries >>= 1;
	}

	/* P[T] is Turbo state if enabled */
	if (get_turbo_state() == TURBO_ENABLED) {
		/* _PSS package count including Turbo */
		acpigen_write_package(num_entries + 2);

		ratio_turbo = pattrs->iacore_ratios[IACORE_TURBO];
		vid_turbo = pattrs->iacore_vids[IACORE_TURBO];
		control_status = (ratio_turbo << 8) | vid_turbo;

		/* Add entry for Turbo ratio */
		acpigen_write_PSS_package(
			clock_max + 1,		/* MHz */
			power_max,		/* mW */
			10,			/* lat1 */
			10,			/* lat2 */
			control_status,		/* control */
			control_status);	/* status */
	} else {
		/* _PSS package count without Turbo */
		acpigen_write_package(num_entries + 1);
		ratio_turbo = ratio_max;
		vid_turbo = vid_max;
	}

	/* First regular entry is max non-turbo ratio */
	control_status = (ratio_max << 8) | vid_max;
	acpigen_write_PSS_package(
		clock_max,		/* MHz */
		power_max,		/* mW */
		10,			/* lat1 */
		10,			/* lat2 */
		control_status,		/* control */
		control_status);	/* status */

	/* Set up ratio and vid ranges for VID calculation */
	ratio_range_2 = (ratio_turbo - ratio_min) * 2;
	vid_range_2 = (vid_turbo - vid_min) * 2;

	/* Generate the remaining entries */
	for (ratio = ratio_min + ((num_entries - 1) * ratio_step);
	     ratio >= ratio_min; ratio -= ratio_step) {

		/* Calculate VID for this ratio */
		vid = ((ratio - ratio_min) * vid_range_2) / ratio_range_2 + vid_min;

		/* Round up if remainder */
		if (((ratio - ratio_min) * vid_range_2) % ratio_range_2)
			vid++;

		/* Calculate power at this ratio */
		power = calculate_power(power_max, ratio_max, ratio);
		clock = (ratio * pattrs->bclk_khz) / 1000;
		control_status = (ratio << 8) | (vid & 0xff);

		acpigen_write_PSS_package(
			clock,			/* MHz */
			power,			/* mW */
			10,			/* lat1 */
			10,			/* lat2 */
			control_status,		/* control */
			control_status);	/* status */
	}

	/* Fix package length */
	acpigen_pop_len();
}

void generate_cpu_entries(const struct device *device)
{
	int core;
	const struct pattrs *pattrs = pattrs_get();

	for (core = 0; core < pattrs->num_cpus; core++) {
		/* Generate processor \_SB.CPUx */
		acpigen_write_processor(core, 0, 0);

		/* Generate  P-state tables */
		generate_p_state_entries(core, pattrs->num_cpus);

		/* Generate C-state tables */
		acpigen_write_CST_package(cstate_map, ARRAY_SIZE(cstate_map));

		/* Generate T-state tables */
		generate_t_state_entries(core, pattrs->num_cpus);

		acpigen_pop_len();
	}

	/* PPKG is usually used for thermal management
	   of the first and only package. */
	acpigen_write_processor_package("PPKG", 0, pattrs->num_cpus);

	/* Add a method to notify processor nodes */
	acpigen_write_processor_cnot(pattrs->num_cpus);
}

unsigned long acpi_madt_irq_overrides(unsigned long current)
{
	int sci_irq = acpi_sci_irq();
	acpi_madt_irqoverride_t *irqovr;
	uint16_t sci_flags = MP_IRQ_TRIGGER_LEVEL;

	/* INT_SRC_OVR */
	irqovr = (void *)current;
	current += acpi_create_madt_irqoverride(irqovr, 0, 0, 2, 0);

	if (sci_irq >= 20)
		sci_flags |= MP_IRQ_POLARITY_LOW;
	else
		sci_flags |= MP_IRQ_POLARITY_HIGH;

	irqovr = (void *)current;
	current += acpi_create_madt_irqoverride(irqovr, 0, sci_irq, sci_irq, sci_flags);

	return current;
}
