/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
 * Copyright (C) 2016-2018 Siemens AG
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
#include <soc/broadwell_de.h>
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
	uint8_t actl = 0;
	static uint8_t sci_irq = 0;
	struct device *dev = dev_find_slot(0, PCI_DEVFN(LPC_DEV, LPC_FUNC));

	/* If this function was already called, just return the stored value. */
	if (sci_irq)
		return sci_irq;
	/* Get contents of ACPI control register. */
	actl = pci_read_config8(dev, ACPI_CNTL_OFFSET) & SCIS_MASK;
	/* Determine how SCI is routed. */
	switch (actl) {
	case SCIS_IRQ9:
	case SCIS_IRQ10:
	case SCIS_IRQ11:
		sci_irq = actl + 9;
		break;
	case SCIS_IRQ20:
	case SCIS_IRQ21:
	case SCIS_IRQ22:
	case SCIS_IRQ23:
		sci_irq = actl - SCIS_IRQ20 + 20;
		break;
	default:
		printk(BIOS_DEBUG, "Invalid SCI route! Defaulting to IRQ9.\n");
		sci_irq = 9;
		break;
	}
	printk(BIOS_DEBUG, "SCI is IRQ%d\n", sci_irq);
	return sci_irq;
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
	header->revision = get_acpi_table_revision(HPET);

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
	header->revision = get_acpi_table_revision(FADT);
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

static unsigned long acpi_fill_dmar(unsigned long current)
{
	uint32_t vtbar, tmp = current;
	struct device *dev = dev_find_slot(0, VTD_DEV_FUNC);
	uint16_t bdf, hpet_bdf[8];
	uint8_t i, j;

	if (!dev)
		return current;

	vtbar = pci_read_config32(dev, VTBAR_OFFSET) & VTBAR_MASK;
	if (!vtbar)
		return current;

	current += acpi_create_dmar_drhd(current,
			DRHD_INCLUDE_PCI_ALL, 0, vtbar);
	/* The IIO I/O APIC is fixed on PCI 00:05.4 on Broadwell-DE */
	current += acpi_create_dmar_ds_ioapic(current,
			9, 0, 5, 4);
	/* Get the PCI BDF for the PCH I/O APIC */
	dev = dev_find_slot(0, LPC_DEV_FUNC);
	bdf = pci_read_config16(dev, 0x6c);
	current += acpi_create_dmar_ds_ioapic(current,
			8, (bdf >> 8), PCI_SLOT(bdf), PCI_FUNC(bdf));

	/*
	 * Check if there are different PCI paths for the 8 HPET timers
	 * and add every different PCI path as a separate HPET entry.
	 * Although the DMAR specification talks about HPET block for this
	 * entry, it is possible to assign a unique PCI BDF to every single
	 * timer within a HPET block which will result in different source
	 * IDs reported by a generated MSI.
	 * In default configuration every single timer will have the same
	 * PCI BDF which will result in a single HPET entry in DMAR table.
	 * I have checked several different systems and all of them had one
	 * single entry for HPET in DMAR.
	 */
	memset(hpet_bdf, 0, sizeof(hpet_bdf));
	/* Get all unique HPET paths. */
	for (i = 0; i < ARRAY_SIZE(hpet_bdf); i++) {
		bdf = pci_read_config16(dev, 0x70 + (i * 2));
		for (j = 0; j < i; j++) {
			if (hpet_bdf[j] == bdf)
				break;
		}
		if (j == i)
			hpet_bdf[i] = bdf;
	}
	/* Create one HPET entry in DMAR for every unique HPET PCI path. */
	for (i = 0; i < ARRAY_SIZE(hpet_bdf); i++) {
		if (hpet_bdf[i])
			current += acpi_create_dmar_ds_msi_hpet(current,
				0, (hpet_bdf[i] >> 8), PCI_SLOT(hpet_bdf[i]),
				PCI_FUNC(hpet_bdf[i]));
	}
	acpi_dmar_drhd_fixup(tmp, current);

	/* Create root port ATSR capability */
	tmp = current;
	current += acpi_create_dmar_atsr(current, 0, 0);
	/* Add one entry to ATSR for each PCI root port */
	dev = all_devices;
	do {
		dev = dev_find_class(PCI_CLASS_BRIDGE_PCI << 8, dev);
		if (dev && dev->bus->secondary == 0 &&
		    PCI_SLOT(dev->path.pci.devfn) <= 3)
			current += acpi_create_dmar_ds_pci_br(current,
					dev->bus->secondary,
					PCI_SLOT(dev->path.pci.devfn),
					PCI_FUNC(dev->path.pci.devfn));
	} while (dev);
	acpi_dmar_atsr_fixup(tmp, current);

	return current;
}

unsigned long vtd_write_acpi_tables(struct device *const dev,
					     unsigned long current,
					     struct acpi_rsdp *const rsdp)
{
	acpi_dmar_t *const dmar = (acpi_dmar_t *)current;

	/* Create DMAR table only if virtualization is enabled */
	if (!(pci_read_config32(dev, VTBAR_OFFSET) & VTBAR_ENABLED))
		return current;

	printk(BIOS_DEBUG, "ACPI:    * DMAR\n");
	acpi_create_dmar(dmar, DMAR_INTR_REMAP, acpi_fill_dmar);
	current += dmar->header.length;
	current = acpi_align_current(current);
	acpi_add_table(rsdp, dmar);
	current = acpi_align_current(current);

	return current;
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

void generate_cpu_entries(struct device *device)
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

	return current;
}
