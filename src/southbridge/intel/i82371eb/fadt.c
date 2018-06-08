/*
 * This file is part of the coreboot project.
 *
 * Based on src/southbridge/via/vt8237r/vt8237_fadt.c
 *
 * Copyright (C) 2004 Nick Barker <nick.barker9@btinternet.com>
 * Copyright (C) 2007, 2009 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <string.h>
#include <arch/acpi.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82371eb.h"

/**
 * Create the Fixed ACPI Description Tables (FADT) for any board with this SB.
 * Reference: ACPIspec40a, 5.2.9, page 118
 */
void acpi_create_fadt(acpi_fadt_t *fadt, acpi_facs_t *facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);
	struct device *dev;

	/* Power management controller */
	dev = dev_find_device(PCI_VENDOR_ID_INTEL,
				PCI_DEVICE_ID_INTEL_82371AB_SMB_ACPI, 0);

	memset((void *) fadt, 0, sizeof(acpi_fadt_t));
	memcpy(header->signature, "FACP", 4);
	header->length = 244;
	header->revision = 1;
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = 42;

	fadt->firmware_ctrl = (u32)facs;
	fadt->dsdt = (u32)dsdt;
	fadt->preferred_pm_profile = 0; /* unspecified */
	fadt->sci_int = 9;
	fadt->smi_cmd = 0; /* smi command port */
	fadt->acpi_enable = 0; /* acpi enable smi command */
	fadt->acpi_disable = 0; /* acpi disable smi command */
	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = 0x0;

	fadt->pm1a_evt_blk = DEFAULT_PMBASE;
	fadt->pm1b_evt_blk = 0x0;
	fadt->pm1a_cnt_blk = DEFAULT_PMBASE + PMCNTRL;
	fadt->pm1b_cnt_blk = 0x0;

	fadt->pm2_cnt_blk = 0;
	fadt->pm_tmr_blk = DEFAULT_PMBASE + PMTMR;
	fadt->gpe0_blk = DEFAULT_PMBASE + GPSTS;
	fadt->gpe1_blk = 0x0;
	fadt->gpe1_base = 0;
	fadt->gpe1_blk_len = 0;

	/* *_len define register width in bytes */
	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm2_cnt_len = 0; /* not supported */
	fadt->pm_tmr_len = 4;
	fadt->gpe0_blk_len = 4;

	fadt->cst_cnt = 0; /* smi command to indicate c state changed notification */
	fadt->p_lvl2_lat = 101; /* >100 means c2 not supported */
	fadt->p_lvl3_lat = 1001; /* >1000 means c3 not supported */
	fadt->flush_size = 0; /* only needed if CPU wbinvd is broken */
	fadt->flush_stride = 0;
	fadt->duty_offset = 1; /* bit 1:3 in PCNTRL reg (pmbase+0x10) */
	fadt->duty_width = 3; /* this width is in bits */
	fadt->day_alrm = 0x0d; /* rtc cmos RAM offset */
	fadt->mon_alrm = 0x0; /* not supported */
	fadt->century = 0x0; /* not supported */
	/*
	 * bit  meaning
	 * 0    1: We have user-visible legacy devices
	 * 1    1: 8042
	 * 2    0: VGA is ok to probe
	 * 3    1: MSI are not supported
	 */
	fadt->iapc_boot_arch = 0xb;
	/*
	 * bit  meaning
	 * 0    WBINVD
	 *      Processors in new ACPI-compatible systems are required to
	 *      support this function and indicate this to OSPM by setting
	 *      this field.
	 * 1    WBINVD_FLUSH
	 *      If set, indicates that the hardware flushes all caches on the
	 *      WBINVD instruction and maintains memory coherency, but does
	 *      not guarantee the caches are invalidated.
	 * 2    PROC_C1
	 *      C1 power state (x86 hlt instruction) is supported on all cpus
	 * 3    P_LVL2_UP
	 *      0: C2 only on uniprocessor, 1: C2 on uni- and multiprocessor
	 * 4    PWR_BUTTON
	 *      0: pwr button is fixed feature
	 *      1: pwr button has control method device if present
	 * 5    SLP_BUTTON
	 *      0: sleep button is fixed feature
	 *      1: sleep button has control method device if present
	 * 6    FIX_RTC
	 *      0: RTC wake status supported in fixed register spce
	 * 7    RTC_S4
	 *      1: RTC can wake from S4
	 * 8    TMR_VAL_EXT
	 *      1: pmtimer is 32bit, 0: pmtimer is 24bit
	 * 9    DCK_CAP
	 *      1: system supports docking station
	 * 10   RESET_REG_SUPPORT
	 *      1: fadt describes reset register for system reset
	 * 11   SEALED_CASE
	 *      1: No expansion possible, sealed case
	 * 12   HEADLESS
	 *      1: Video output, keyboard and mouse are not connected
	 * 13   CPU_SW_SLP
	 *      1: Special processor instruction needs to be executed
	 *      after writing SLP_TYP
	 * 14   PCI_EXP_WAK
	 *      1: PM1 regs support PCIEXP_WAKE_(STS|EN), must be set
	 *      on platforms with pci express support
	 * 15   USE_PLATFORM_CLOCK
	 *      1: OS should prefer platform clock over processor internal
	 *      clock.
	 * 16   S4_RTC_STS_VALID
	 * 17   REMOTE_POWER_ON_CAPABLE
	 *      1: platform correctly supports OSPM leaving GPE wake events
	 *      armed prior to an S5 transition.
	 * 18   FORCE_APIC_CLUSTER_MODEL
	 * 19   FORCE_APIC_PHYSICAL_DESTINATION_MODE
	 */
	fadt->flags = 0xa5;

	fadt->reset_reg.space_id = 0;
	fadt->reset_reg.bit_width = 0;
	fadt->reset_reg.bit_offset = 0;
	fadt->reset_reg.resv = 0;
	fadt->reset_reg.addrl = 0x0;
	fadt->reset_reg.addrh = 0x0;
	fadt->reset_value = 0;

	fadt->x_firmware_ctl_l = (u32)facs;
	fadt->x_firmware_ctl_h = 0;
	fadt->x_dsdt_l = (u32)dsdt;
	fadt->x_dsdt_h = 0;

	fadt->x_pm1a_evt_blk.space_id = 1;
	fadt->x_pm1a_evt_blk.bit_width = fadt->pm1_evt_len * 8;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.resv = 0;
	fadt->x_pm1a_evt_blk.addrl = fadt->pm1a_evt_blk;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	fadt->x_pm1b_evt_blk.space_id = 1;
	fadt->x_pm1b_evt_blk.bit_width = fadt->pm1_evt_len * 8;
	fadt->x_pm1b_evt_blk.bit_offset = 0;
	fadt->x_pm1b_evt_blk.resv = 0;
	fadt->x_pm1b_evt_blk.addrl = fadt->pm1b_evt_blk;
	fadt->x_pm1b_evt_blk.addrh = 0x0;

	fadt->x_pm1a_cnt_blk.space_id = 1;
	fadt->x_pm1a_cnt_blk.bit_width = fadt->pm1_cnt_len * 8;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.resv = 0;
	fadt->x_pm1a_cnt_blk.addrl = fadt->pm1a_cnt_blk;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	fadt->x_pm1b_cnt_blk.space_id = 1;
	fadt->x_pm1b_cnt_blk.bit_width = fadt->pm1_cnt_len * 8;
	fadt->x_pm1b_cnt_blk.bit_offset = 0;
	fadt->x_pm1b_cnt_blk.resv = 0;
	fadt->x_pm1b_cnt_blk.addrl = fadt->pm1b_cnt_blk;
	fadt->x_pm1b_cnt_blk.addrh = 0x0;

	fadt->x_pm2_cnt_blk.space_id = 1;
	fadt->x_pm2_cnt_blk.bit_width = fadt->pm2_cnt_len * 8;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.resv = 0;
	fadt->x_pm2_cnt_blk.addrl = fadt->pm2_cnt_blk;
	fadt->x_pm2_cnt_blk.addrh = 0x0;

	fadt->x_pm_tmr_blk.space_id = 1;
	fadt->x_pm_tmr_blk.bit_width = fadt->pm_tmr_len * 8;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.resv = 0;
	fadt->x_pm_tmr_blk.addrl = fadt->pm_tmr_blk;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	fadt->x_gpe0_blk.space_id = 1;
	fadt->x_gpe0_blk.bit_width = fadt->gpe0_blk_len * 8;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.resv = 0;
	fadt->x_gpe0_blk.addrl = fadt->gpe0_blk;
	fadt->x_gpe0_blk.addrh = 0x0;

	fadt->x_gpe1_blk.space_id = 1;
	fadt->x_gpe1_blk.bit_width = fadt->gpe1_blk_len * 8;
	fadt->x_gpe1_blk.bit_offset = 0;
	fadt->x_gpe1_blk.resv = 0;
	fadt->x_gpe1_blk.addrl = fadt->gpe1_blk;
	fadt->x_gpe1_blk.addrh = 0x0;

	header->checksum = acpi_checksum((void *) fadt, sizeof(acpi_fadt_t));
}
