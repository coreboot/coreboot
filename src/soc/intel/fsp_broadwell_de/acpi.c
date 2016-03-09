/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <string.h>
#include <types.h>
#include <arch/acpigen.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/smp/mpspec.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/speedstep.h>
#include <cpu/intel/turbo.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/acpi.h>
#include <soc/iomap.h>
#include <soc/irq.h>
#include <soc/lpc.h>
#include <soc/msr.h>
#include <soc/pattrs.h>
#include <soc/pci_devs.h>
#include <chip.h>

uint16_t get_pmbase(void)
{
	return ACPI_BASE_ADDRESS;
}

#define MWAIT_RES(state, sub_state)                         \
	{                                                   \
		.addrl = (((state) << 4) | (sub_state)),    \
		.space_id = ACPI_ADDRESS_SPACE_FIXED,       \
		.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,    \
		.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,    \
		.access_size = ACPI_FFIXEDHW_FLAG_HW_COORD, \
	}

/* C-state map */
static acpi_cstate_t cstate_map[] = {
	{
		/* C1 */
		.ctype = 1, /* ACPI C1 */
		.latency = 1,
		.power = 1000,
		.resource = MWAIT_RES(0, 0),
	},
	{
		/* C3 */
		.ctype = 2, /* ACPI C2 */
		.latency = 15,
		.power = 500,
		.resource = MWAIT_RES(1, 0),
	},
	{
		/* C6 */
		.ctype = 3, /* ACPI C3 */
		.latency = 41,
		.power = 350,
		.resource = MWAIT_RES(2, 0),
	}
};

static int acpi_sci_irq(void)
{
	return 9;
}

void acpi_create_intel_hpet(acpi_hpet_t *hpet)
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
	addr->space_id   = 0;	/* Memory */
	addr->bit_width  = 64;
	addr->bit_offset = 0;
	addr->addrl      = (unsigned long long)HPET_BASE_ADDRESS & 0xffffffff;
	addr->addrh      = (unsigned long long)HPET_BASE_ADDRESS >> 32;

	hpet->id         = 0x8086a201;	/* Intel */
	hpet->number     = 0x00;
	hpet->min_tick   = 0x0080;

	header->checksum = acpi_checksum((void *) hpet, sizeof(acpi_hpet_t));
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current,
	                                     MCFG_BASE_ADDRESS, 0, 0, 255);
	return current;
}

/**
 * Fill in the fadt with generic values that can be overridden later.
 */

void acpi_fill_in_fadt(acpi_fadt_t *fadt, acpi_facs_t *facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);
	u16 pmbase = get_pmbase();

	memset((void *) fadt, 0, sizeof(acpi_fadt_t));

	/*
	 * Reference section 5.2.9 Fixed ACPI Description Table (FADT)
	 * in the ACPI 3.0b specification.
	 */

	/* FADT Header Structure */
	memcpy(header->signature, "FACP", 4);
	header->length = sizeof(acpi_fadt_t);
	header->revision = ACPI_FADT_REV_ACPI_3_0;
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = 1;

	/* ACPI Pointers */
	fadt->firmware_ctrl = (unsigned long) facs;
	fadt->dsdt = (unsigned long) dsdt;

	fadt->model                = 0; /* reserved, should be 0 ACPI 3.0 */
	fadt->preferred_pm_profile = 0;
	fadt->sci_int              = acpi_sci_irq();

	/* System Management */
	fadt->smi_cmd      = 0x00; /* disable SMM */
	fadt->acpi_enable  = 0x00; /* unused  if  SMI_CMD = 0 */
	fadt->acpi_disable = 0x00; /* unused  if  SMI_CMD = 0 */

	/* Enable ACPI */
	outl(inl(pmbase + 4) | 0x01, pmbase + 4);

	/* Power Control */
	fadt->s4bios_req = 0x00;
	fadt->pstate_cnt = 0x00;

	/* Control Registers - Base Address */
	fadt->pm1a_evt_blk = pmbase + PM1_STS;
	fadt->pm1b_evt_blk = 0x00; /* Not Used */
	fadt->pm1a_cnt_blk = pmbase + PM1_CNT;
	fadt->pm1b_cnt_blk = 0x00; /* Not Used */
	fadt->pm2_cnt_blk  = pmbase + PM2A_CNT_BLK;
	fadt->pm_tmr_blk   = pmbase + PM1_TMR;
	fadt->gpe0_blk     = pmbase + GPE0_STS;
	fadt->gpe1_blk     = 0x00; /* Not Used */

	/* Control Registers - Length */
	fadt->pm1_evt_len  = 4; /* 32 bits */
	fadt->pm1_cnt_len  = 2; /* 32 bit register, 16 bits used */
	fadt->pm2_cnt_len  = 1; /* 8 bits */
	fadt->pm_tmr_len   = 4; /* 32 bits */
	fadt->gpe0_blk_len = 8; /* 64 bits */
	fadt->gpe1_blk_len = 0;
	fadt->gpe1_base    = 0;
	fadt->cst_cnt      = 0;
	fadt->p_lvl2_lat   = ACPI_FADT_C2_NOT_SUPPORTED;
	fadt->p_lvl3_lat   = ACPI_FADT_C3_NOT_SUPPORTED;
	fadt->flush_size   = 0; /* set to 0 if WBINVD is 1 in flags */
	fadt->flush_stride = 0; /* set to 0 if WBINVD is 1 in flags */
	fadt->duty_offset  = 1;
	fadt->duty_width   = 0;

	/* RTC Registers */
	fadt->day_alrm       = 0x0D;
	fadt->mon_alrm       = 0x00;
	fadt->century        = 0x00;
	fadt->iapc_boot_arch = 0;

	fadt->flags = ACPI_FADT_WBINVD | ACPI_FADT_C1_SUPPORTED |
			ACPI_FADT_C2_MP_SUPPORTED | ACPI_FADT_SLEEP_BUTTON |
			ACPI_FADT_RESET_REGISTER | ACPI_FADT_SLEEP_TYPE |
			ACPI_FADT_S4_RTC_WAKE | ACPI_FADT_PLATFORM_CLOCK;

	/* Reset Register */
	fadt->reset_reg.space_id    = ACPI_ADDRESS_SPACE_IO;
	fadt->reset_reg.bit_width   = 8;
	fadt->reset_reg.bit_offset  = 0;
	fadt->reset_reg.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->reset_reg.addrl       = 0xCF9;
	fadt->reset_reg.addrh       = 0x00;
	fadt->reset_value           = 6;

	/* Reserved Bits */
	fadt->res3 = 0x00; /* reserved, MUST be 0 ACPI 3.0 */
	fadt->res4 = 0x00; /* reserved, MUST be 0 ACPI 3.0 */
	fadt->res5 = 0x00; /* reserved, MUST be 0 ACPI 3.0 */

	/* Extended ACPI Pointers */
	fadt->x_firmware_ctl_l = (unsigned long)facs;
	fadt->x_firmware_ctl_h = 0x00;
	fadt->x_dsdt_l         = (unsigned long)dsdt;
	fadt->x_dsdt_h         = 0x00;

	/* PM1 Status & PM1 Enable */
	fadt->x_pm1a_evt_blk.space_id    = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_evt_blk.bit_width   = fadt->pm1_evt_len * 8;
	fadt->x_pm1a_evt_blk.bit_offset  = 0;
	fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm1a_evt_blk.addrl       = fadt->pm1a_evt_blk;
	fadt->x_pm1a_evt_blk.addrh       = 0x00;

	fadt->x_pm1b_evt_blk.space_id    = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1b_evt_blk.bit_width   = 0;
	fadt->x_pm1b_evt_blk.bit_offset  = 0;
	fadt->x_pm1b_evt_blk.access_size = 0;
	fadt->x_pm1b_evt_blk.addrl       = fadt->pm1b_evt_blk;
	fadt->x_pm1b_evt_blk.addrh       = 0x00;

	/* PM1 Control Registers */
	fadt->x_pm1a_cnt_blk.space_id    = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_cnt_blk.bit_width   = 16;
	fadt->x_pm1a_cnt_blk.bit_offset  = 0;
	fadt->x_pm1a_cnt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_cnt_blk.addrl       = fadt->pm1a_cnt_blk;
	fadt->x_pm1a_cnt_blk.addrh       = 0x00;

	fadt->x_pm1b_cnt_blk.space_id    = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1b_cnt_blk.bit_width   = 0;
	fadt->x_pm1b_cnt_blk.bit_offset  = 0;
	fadt->x_pm1b_cnt_blk.access_size = 0;
	fadt->x_pm1b_cnt_blk.addrl       = fadt->pm1b_cnt_blk;
	fadt->x_pm1b_cnt_blk.addrh       = 0x00;

	/* PM2 Control Registers */
	fadt->x_pm2_cnt_blk.space_id     = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm2_cnt_blk.bit_width    = 8;
	fadt->x_pm2_cnt_blk.bit_offset   = 0;
	fadt->x_pm2_cnt_blk.access_size  = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_pm2_cnt_blk.addrl        = fadt->pm2_cnt_blk;
	fadt->x_pm2_cnt_blk.addrh        = 0x00;

	/* PM1 Timer Register */
	fadt->x_pm_tmr_blk.space_id      = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm_tmr_blk.bit_width     = 32;
	fadt->x_pm_tmr_blk.bit_offset    = 0;
	fadt->x_pm_tmr_blk.access_size   = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm_tmr_blk.addrl         = fadt->pm_tmr_blk;
	fadt->x_pm_tmr_blk.addrh         = 0x00;

	/*  General-Purpose Event Registers */
	fadt->x_gpe0_blk.space_id        = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe0_blk.bit_width       = 64;	/* EventStatus + EventEnable */
	fadt->x_gpe0_blk.bit_offset      = 0;
	fadt->x_gpe0_blk.access_size     = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_gpe0_blk.addrl           = fadt->gpe0_blk;
	fadt->x_gpe0_blk.addrh           = 0x00;

	fadt->x_gpe1_blk.space_id        = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe1_blk.bit_width       = 0;
	fadt->x_gpe1_blk.bit_offset      = 0;
	fadt->x_gpe1_blk.access_size     = 0;
	fadt->x_gpe1_blk.addrl           = fadt->gpe1_blk;
	fadt->x_gpe1_blk.addrh           = 0x00;

	header->checksum = acpi_checksum((void *) fadt, sizeof(acpi_fadt_t));
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

static void generate_P_state_entries(int core, int cores_per_package)
{
	int ratio_min, ratio_max, ratio_step;
	int coord_type, power_max, power_unit, num_entries;
	int ratio, power, clock, clock_max;
	int turbo;
	u32 control_status;
	msr_t msr;

	/* Hardware coordination of P-states */
	coord_type = HW_ALL;

	/* Check for Turbo Mode */
	turbo = get_turbo_state() == TURBO_ENABLED;

	/* CPU attributes */
	msr = rdmsr(MSR_PLATFORM_INFO);
	ratio_min = (msr.hi >>  8) & 0xff;	// LFM
	ratio_max = (msr.lo >>  8) & 0xff;	// HFM
	clock_max = (ratio_max * 100);

	/* Calculate CPU TDP in mW */
	msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 1 << (msr.lo & 0xf);
	msr = rdmsr(MSR_PKG_POWER_LIMIT);
	power_max = ((msr.lo & 0x7fff) / power_unit) * 1000;

	/* Write _PCT indicating use of FFixedHW */
	acpigen_write_empty_PCT();

	/* Write _PPC starting from first supported P-state */
	acpigen_write_PPC(0);

	/* Write PSD indicating configured coordination type */
	acpigen_write_PSD_package(core, 1, coord_type);

	/* Add P-state entries in _PSS table */
	acpigen_write_name("_PSS");

	/* Determine ratio points */
	/* Note: There should be at most 16 performance states. If Turbo Mode
	   is enabled, the Max Turbo Ratio will occupy one of these states. */
	ratio_step = 1;
	num_entries = (ratio_max - ratio_min) / ratio_step;
	while (num_entries > (15-turbo)) {
		ratio_step <<= 1;
		num_entries >>= 1;
	}

	if (turbo) {
		/* _PSS package count (with turbo)  */
		acpigen_write_package(num_entries + 2);

		/* Get Max Turbo Ratio */
		msr = rdmsr(MSR_TURBO_RATIO_LIMIT);
		ratio = msr.lo & 0xff;

		acpigen_write_PSS_package(
			ratio * 100,		/* MHz */
			power_max,		/* mW */
			10,			/* lat1 */
			10,			/* lat2 */
			ratio << 8,		/* control */
			ratio << 8);		/* status */
	} else {
		/* _PSS package count (without turbo) */
		acpigen_write_package(num_entries + 1);
	}

	/* Generate the _PSS entries */
	for (ratio = ratio_min + (num_entries * ratio_step);
		ratio >= ratio_min; ratio -= ratio_step) {

		/* Calculate power at this ratio */
		power = calculate_power(power_max, ratio_max, ratio);
		clock = ratio * 100;
		control_status = ratio << 8;

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

void generate_cpu_entries(device_t device)
{
	int core;
	int pcontrol_blk = get_pmbase(), plen = 6;
	const struct pattrs *pattrs = pattrs_get();

	for (core = 0; core < pattrs->num_cpus; core++) {
		if (core > 0) {
			pcontrol_blk = 0;
			plen = 0;
		}

		/* Generate processor \_PR.CP0x */
		acpigen_write_processor(core, pcontrol_blk, plen);

		/* Generate P-state tables */
		generate_P_state_entries(core, pattrs->num_cpus);

		/* Generate C-state tables */
		acpigen_write_CST_package(cstate_map, ARRAY_SIZE(cstate_map));

		acpigen_pop_len();
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

	irqovr = (void *)current;
	current += acpi_create_madt_irqoverride(irqovr, 0, 3, 3,
	                                       (MP_IRQ_TRIGGER_LEVEL
	                                       |MP_IRQ_POLARITY_LOW));

	irqovr = (void *)current;
	current += acpi_create_madt_irqoverride(irqovr, 0, 4, 4,
	                                       (MP_IRQ_TRIGGER_LEVEL
	                                       |MP_IRQ_POLARITY_LOW));

	return current;
}
