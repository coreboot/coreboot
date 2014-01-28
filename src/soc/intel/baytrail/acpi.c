/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <arch/io.h>
#include <arch/smp/mpspec.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <console/console.h>
#include <types.h>
#include <string.h>
#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/tsc.h>
#include <cpu/intel/turbo.h>

#include <baytrail/acpi.h>
#include <baytrail/iomap.h>
#include <baytrail/irq.h>
#include <baytrail/msr.h>
#include <baytrail/pattrs.h>
#include <baytrail/pmc.h>

#define MWAIT_RES(state, sub_state)                         \
	{                                                   \
		.addrl = (((state) << 4) | (sub_state)),    \
		.space_id = ACPI_ADDRESS_SPACE_FIXED,       \
		.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,    \
		.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,    \
		.access_size = ACPI_FFIXEDHW_FLAG_HW_COORD, \
	}

/* C-state map without S0ix */
static acpi_cstate_t cstate_map[] = {
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
		.resource = MWAIT_RES(5, 8),
	},
	{
		/* C6FS with full L2 shrink */
		.ctype = 3, /* ACPI C3 */
		.latency = 1500, /* 1.5ms worst case */
		.power = 10,
		.resource = MWAIT_RES(5, 2),
	}
};

static int acpi_sci_irq(void)
{
	const unsigned long actl = ILB_BASE_ADDRESS + ACTL;
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

void acpi_create_intel_hpet(acpi_hpet_t * hpet)
{
	acpi_header_t *header = &(hpet->header);
	acpi_addr_t *addr = &(hpet->addr);

	memset((void *) hpet, 0, sizeof(acpi_hpet_t));

	/* fill out header fields */
	memcpy(header->signature, "HPET", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->length = sizeof(acpi_hpet_t);
	header->revision = 1;

	/* fill out HPET address */
	addr->space_id = 0;	/* Memory */
	addr->bit_width = 64;
	addr->bit_offset = 0;
	addr->addrl = (unsigned long long)HPET_BASE_ADDRESS & 0xffffffff;
	addr->addrh = (unsigned long long)HPET_BASE_ADDRESS >> 32;

	hpet->id = 0x8086a201;	/* Intel */
	hpet->number = 0x00;
	hpet->min_tick = 0x0080;

	header->checksum =
	    acpi_checksum((void *) hpet, sizeof(acpi_hpet_t));
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current,
					     MCFG_BASE_ADDRESS, 0, 0, 255);
	return current;
}

void acpi_fill_in_fadt(acpi_fadt_t *fadt)
{
	const uint16_t pmbase = ACPI_BASE_ADDRESS;

	fadt->sci_int = acpi_sci_irq();
	fadt->smi_cmd = APM_CNT;
	fadt->acpi_enable = APM_CNT_ACPI_ENABLE;
	fadt->acpi_disable = APM_CNT_ACPI_DISABLE;
	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = 0;

	fadt->pm1a_evt_blk = pmbase + PM1_STS;
	fadt->pm1b_evt_blk = 0x0;
	fadt->pm1a_cnt_blk = pmbase + PM1_CNT;
	fadt->pm1b_cnt_blk = 0x0;
	fadt->pm2_cnt_blk = pmbase + PM2A_CNT_BLK;
	fadt->pm_tmr_blk = pmbase + PM1_TMR;
	fadt->gpe0_blk = pmbase + GPE0_STS;
	fadt->gpe1_blk = 0;

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm2_cnt_len = 1;
	fadt->pm_tmr_len = 4;
	fadt->gpe0_blk_len = 2 * (GPE0_EN - GPE0_STS);
	fadt->gpe1_blk_len = 0;
	fadt->gpe1_base = 0;
	fadt->cst_cnt = 0;
	fadt->p_lvl2_lat = 1;
	fadt->p_lvl3_lat = 87;
	fadt->flush_size = 1024;
	fadt->flush_stride = 16;
	fadt->duty_offset = 1;
	fadt->duty_width = 0;
	fadt->day_alrm = 0xd;
	fadt->mon_alrm = 0x00;
	fadt->century = 0x00;
	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042;

	fadt->flags = ACPI_FADT_WBINVD | ACPI_FADT_C1_SUPPORTED |
			ACPI_FADT_C2_MP_SUPPORTED | ACPI_FADT_SLEEP_BUTTON |
			ACPI_FADT_RESET_REGISTER | ACPI_FADT_SEALED_CASE |
			ACPI_FADT_S4_RTC_WAKE | ACPI_FADT_PLATFORM_CLOCK;

	fadt->reset_reg.space_id = 1;
	fadt->reset_reg.bit_width = 8;
	fadt->reset_reg.bit_offset = 0;
	fadt->reset_reg.resv = 0;
	fadt->reset_reg.addrl = 0xcf9;
	fadt->reset_reg.addrh = 0;
	fadt->reset_value = 6;

	fadt->x_pm1a_evt_blk.space_id = 1;
	fadt->x_pm1a_evt_blk.bit_width = fadt->pm1_evt_len * 8;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.resv = 0;
	fadt->x_pm1a_evt_blk.addrl = pmbase + PM1_STS;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	fadt->x_pm1b_evt_blk.space_id = 1;
	fadt->x_pm1b_evt_blk.bit_width = 0;
	fadt->x_pm1b_evt_blk.bit_offset = 0;
	fadt->x_pm1b_evt_blk.resv = 0;
	fadt->x_pm1b_evt_blk.addrl = 0x0;
	fadt->x_pm1b_evt_blk.addrh = 0x0;

	fadt->x_pm1a_cnt_blk.space_id = 1;
	fadt->x_pm1a_cnt_blk.bit_width = fadt->pm1_cnt_len * 8;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.resv = 0;
	fadt->x_pm1a_cnt_blk.addrl = pmbase + PM1_CNT;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	fadt->x_pm1b_cnt_blk.space_id = 1;
	fadt->x_pm1b_cnt_blk.bit_width = 0;
	fadt->x_pm1b_cnt_blk.bit_offset = 0;
	fadt->x_pm1b_cnt_blk.resv = 0;
	fadt->x_pm1b_cnt_blk.addrl = 0x0;
	fadt->x_pm1b_cnt_blk.addrh = 0x0;

	fadt->x_pm2_cnt_blk.space_id = 1;
	fadt->x_pm2_cnt_blk.bit_width = fadt->pm2_cnt_len * 8;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.resv = 0;
	fadt->x_pm2_cnt_blk.addrl = pmbase + PM2A_CNT_BLK;
	fadt->x_pm2_cnt_blk.addrh = 0x0;

	fadt->x_pm_tmr_blk.space_id = 1;
	fadt->x_pm_tmr_blk.bit_width = fadt->pm_tmr_len * 8;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.resv = 0;
	fadt->x_pm_tmr_blk.addrl = pmbase + PM1_TMR;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	fadt->x_gpe0_blk.space_id = 1;
	fadt->x_gpe0_blk.bit_width = fadt->gpe0_blk_len * 8;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.resv = 0;
	fadt->x_gpe0_blk.addrl = pmbase + GPE0_STS;
	fadt->x_gpe0_blk.addrh = 0x0;

	fadt->x_gpe1_blk.space_id = 1;
	fadt->x_gpe1_blk.bit_width = 0;
	fadt->x_gpe1_blk.bit_offset = 0;
	fadt->x_gpe1_blk.resv = 0;
	fadt->x_gpe1_blk.addrl = 0x0;
	fadt->x_gpe1_blk.addrh = 0x0;
}

static acpi_tstate_t baytrail_tss_table[] = {
	{ 100, 1000, 0, 0x00, 0 },
	{ 88, 875, 0, 0x1e, 0 },
	{ 75, 750, 0, 0x1c, 0 },
	{ 63, 625, 0, 0x1a, 0 },
	{ 50, 500, 0, 0x18, 0 },
	{ 38, 375, 0, 0x16, 0 },
	{ 25, 250, 0, 0x14, 0 },
	{ 13, 125, 0, 0x12, 0 },
};

static int generate_T_state_entries(int core, int cores_per_package)
{
	int len;

	/* Indicate SW_ALL coordination for T-states */
	len = acpigen_write_TSD_package(core, cores_per_package, SW_ALL);

	/* Indicate FFixedHW so OS will use MSR */
	len += acpigen_write_empty_PTC();

	/* Set NVS controlled T-state limit */
	len += acpigen_write_TPC("\\TLVL");

	/* Write TSS table for MSR access */
	len += acpigen_write_TSS_package(
		ARRAY_SIZE(baytrail_tss_table), baytrail_tss_table);

	return len;
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

static int generate_P_state_entries(int core, int cores_per_package)
{
	int len, len_pss;
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

	/* Hardware coordination of P-states */
	coord_type = HW_ALL;

	/* Max Non-Turbo Frequency */
	clock_max = (ratio_max * pattrs->bclk_khz) / 1000;

	/* Calculate CPU TDP in mW */
	msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 1 << (msr.lo & 0xf);
	msr = rdmsr(MSR_PKG_POWER_LIMIT);
	power_max = ((msr.lo & 0x7fff) / power_unit) * 1000;

	/* Write _PCT indicating use of FFixedHW */
	len = acpigen_write_empty_PCT();

	/* Write _PPC with no limit on supported P-state */
	len += acpigen_write_PPC(0);

	/* Write PSD indicating configured coordination type */
	len += acpigen_write_PSD_package(core, 1, coord_type);

	/* Add P-state entries in _PSS table */
	len += acpigen_write_name("_PSS");

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
		len_pss = acpigen_write_package(num_entries + 2);

		ratio_turbo = pattrs->iacore_ratios[IACORE_TURBO];
		vid_turbo = pattrs->iacore_vids[IACORE_TURBO];
		control_status = (ratio_turbo << 8) | vid_turbo;

		/* Add entry for Turbo ratio */
		len_pss += acpigen_write_PSS_package(
			clock_max + 1,		/*MHz*/
			power_max,		/*mW*/
			10,			/*lat1*/
			10,			/*lat2*/
			control_status,		/*control*/
			control_status);	/*status*/
	} else {
		/* _PSS package count without Turbo */
		len_pss = acpigen_write_package(num_entries + 1);
		ratio_turbo = ratio_max;
		vid_turbo = vid_max;
	}

	/* First regular entry is max non-turbo ratio */
	control_status = (ratio_max << 8) | vid_max;
	len_pss += acpigen_write_PSS_package(
		clock_max,		/*MHz*/
		power_max,		/*mW*/
		10,			/*lat1*/
		10,			/*lat2*/
		control_status,		/*control */
		control_status);	/*status*/

	/* Set up ratio and vid ranges for VID calculation */
	ratio_range_2 = (ratio_turbo - ratio_min) * 2;
	vid_range_2 = (vid_turbo - vid_min) * 2;

	/* Generate the remaining entries */
	for (ratio = ratio_min + ((num_entries - 1) * ratio_step);
	     ratio >= ratio_min; ratio -= ratio_step) {

		/* Calculate VID for this ratio */
		vid = ((ratio - ratio_min) * vid_range_2) /
			ratio_range_2 + vid_min;
		/* Round up if remainder */
		if (((ratio - ratio_min) * vid_range_2) % ratio_range_2)
			vid++;

		/* Calculate power at this ratio */
		power = calculate_power(power_max, ratio_max, ratio);
		clock = (ratio * pattrs->bclk_khz) / 1000;
		control_status = (ratio << 8) | (vid & 0xff);

		len_pss += acpigen_write_PSS_package(
			clock,			/*MHz*/
			power,			/*mW*/
			10,			/*lat1*/
			10,			/*lat2*/
			control_status,		/*control*/
			control_status);	/*status*/
	}

	/* Fix package length */
	len_pss--;
	acpigen_patch_len(len_pss);

	return len + len_pss;
}

void generate_cpu_entries(void)
{
	int len_pr, core;
	int pcontrol_blk = get_pmbase(), plen = 6;
	const struct pattrs *pattrs = pattrs_get();

	for (core=0; core<pattrs->num_cpus; core++) {
		if (core > 0) {
			pcontrol_blk = 0;
			plen = 0;
		}

		/* Generate processor \_PR.CPUx */
		len_pr = acpigen_write_processor(
			core, pcontrol_blk, plen);

		/* Generate  P-state tables */
		len_pr += generate_P_state_entries(
			core, pattrs->num_cpus);

		/* Generate C-state tables */
		len_pr += acpigen_write_CST_package(
			cstate_map, ARRAY_SIZE(cstate_map));

		/* Generate T-state tables */
		len_pr += generate_T_state_entries(
			core, pattrs->num_cpus);

		len_pr--;
		acpigen_patch_len(len_pr);
	}
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
	current += acpi_create_madt_irqoverride(irqovr, 0, sci_irq, sci_irq,
	                                        sci_flags);

	return current;
}
