/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2009 Thomas Jourdan <thomas.jourdan@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <string.h>
#include <device/pci.h>
#include <arch/acpi.h>
#include <cpu/x86/smm.h>

#define ACPI_PM1_STS        (pmbase + 0x00)
#define ACPI_PM1_EN         (pmbase + 0x02)
#define ACPI_PM1_CNT        (pmbase + 0x04)
#define ACPI_PM1_TMR        (pmbase + 0x08)
#define ACPI_PROC_CNT       (pmbase + 0x10)
#define ACPI_LV2            (pmbase + 0x14)
#define ACPI_GPE0_STS       (pmbase + 0x28)
#define ACPI_GPE0_EN        (pmbase + 0x2C)
#define ACPI_SMI_EN         (pmbase + 0x30)
#define ACPI_SMI_STS        (pmbase + 0x34)
#define ACPI_ALT_GP_SMI_EN  (pmbase + 0x38)
#define ACPI_ALT_GP_SMI_STS (pmbase + 0x3A)
#define ACPI_MON_SMI        (pmbase + 0x40)
#define ACPI_DEVACT_STS     (pmbase + 0x44)
#define ACPI_DEVTRAP_EN     (pmbase + 0x48)
#define ACPI_BUS_ADDR_TRACK (pmbase + 0x4C)
#define ACPI_BUS_CYC_TRACK  (pmbase + 0x4E)

#define ACPI_PM1a_EVT_BLK ACPI_PM1_STS
#define ACPI_PM1a_CNT_BLK ACPI_PM1_CNT
#define ACPI_PM_TMR_BLK   ACPI_PM1_TMR
#define ACPI_P_BLK        ACPI_PROC_CNT
#define ACPI_GPE0_BLK     ACPI_GPE0_STS

void acpi_create_fadt(acpi_fadt_t * fadt, acpi_facs_t * facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);
	u16 pmbase = pci_read_config16(dev_find_slot(0, PCI_DEVFN(0x1f,0)), 0x40) & 0xfffe;

	memset((void *) fadt, 0, sizeof(acpi_fadt_t));
	memcpy(header->signature, "FACP", 4);
	header->length = 244;
	header->revision = 3;
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = 0;

	fadt->firmware_ctrl = (unsigned long) facs;
	fadt->dsdt = (unsigned long) dsdt;
	fadt->preferred_pm_profile = 7; /* Performance Server */
	fadt->sci_int = 0x9;
#if CONFIG_HAVE_SMI_HANDLER
	fadt->smi_cmd = APM_CNT;
#else
	fadt->smi_cmd = 0x00;
#endif
	fadt->acpi_enable = APM_CNT_ACPI_ENABLE;
	fadt->acpi_disable = APM_CNT_ACPI_DISABLE;
	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = 0xe2;

	fadt->pm1a_evt_blk = pmbase;
	fadt->pm1b_evt_blk = 0x0;
	fadt->pm1a_cnt_blk = pmbase + 0x4;
	fadt->pm1b_cnt_blk = 0x0;
	fadt->pm2_cnt_blk = 0x0;
	fadt->pm_tmr_blk = pmbase + 0x8;
	fadt->gpe0_blk = pmbase + 0x28;
	fadt->gpe1_blk = 0x0;

	fadt->pm1_evt_len = 0x4;
	fadt->pm1_cnt_len = 0x2;
	fadt->pm2_cnt_len = 0x0;
	fadt->pm_tmr_len = 0x4;
	fadt->gpe0_blk_len = 0x8;
	fadt->gpe1_blk_len = 0x0;
	fadt->gpe1_base = 0x0;
	fadt->cst_cnt = 0xe3;
	fadt->p_lvl2_lat = 0x65;
	fadt->p_lvl3_lat = 0x3e9;
	fadt->flush_size = 0x400;
	fadt->flush_stride = 0x10;
	fadt->duty_offset = 0x1;
	fadt->duty_width = 0x3;
	fadt->day_alrm = 0xd;
	fadt->mon_alrm = 0x00;
	fadt->century = 0x00;
	fadt->iapc_boot_arch = 0x03;
	fadt->flags = 0xa5;

	fadt->reset_reg.space_id = 1;
	fadt->reset_reg.bit_width = 8;
	fadt->reset_reg.bit_offset = 0;
	fadt->reset_reg.resv = 0;
	fadt->reset_reg.addrl = 0xcf9;
	fadt->reset_reg.addrh = 0;
	fadt->reset_value = 6;
	fadt->res3 = 0;
	fadt->res4 = 0;
	fadt->res5 = 0;
	fadt->x_firmware_ctl_l = (u32)facs;
	fadt->x_firmware_ctl_h = 0;
	fadt->x_dsdt_l = (u32)dsdt;
	fadt->x_dsdt_h = 0;

	fadt->x_pm1a_evt_blk.space_id = 1;
	fadt->x_pm1a_evt_blk.bit_width = 32;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.resv = 0;
	fadt->x_pm1a_evt_blk.addrl = pmbase;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	fadt->x_pm1b_evt_blk.space_id = 1;
	fadt->x_pm1b_evt_blk.bit_width = 32;
	fadt->x_pm1b_evt_blk.bit_offset = 0;
	fadt->x_pm1b_evt_blk.resv = 0;
	fadt->x_pm1b_evt_blk.addrl = 0x0;
	fadt->x_pm1b_evt_blk.addrh = 0x0;

	fadt->x_pm1a_cnt_blk.space_id = 1;
	fadt->x_pm1a_cnt_blk.bit_width = 16;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.resv = 0;
	fadt->x_pm1a_cnt_blk.addrl = pmbase + 0x4;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	fadt->x_pm1b_cnt_blk.space_id = 1;
	fadt->x_pm1b_cnt_blk.bit_width = 0;
	fadt->x_pm1b_cnt_blk.bit_offset = 0;
	fadt->x_pm1b_cnt_blk.resv = 0;
	fadt->x_pm1b_cnt_blk.addrl = 0x0;
	fadt->x_pm1b_cnt_blk.addrh = 0x0;

	fadt->x_pm2_cnt_blk.space_id = 1;
	fadt->x_pm2_cnt_blk.bit_width = 0;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.resv = 0;
	fadt->x_pm2_cnt_blk.addrl = 0x0;
	fadt->x_pm2_cnt_blk.addrh = 0x0;

	fadt->x_pm_tmr_blk.space_id = 1;
	fadt->x_pm_tmr_blk.bit_width = 32;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.resv = 0;
	fadt->x_pm_tmr_blk.addrl = pmbase + 0x8;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	fadt->x_gpe0_blk.space_id = 1;
	fadt->x_gpe0_blk.bit_width = 64;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.resv = 0;
	fadt->x_gpe0_blk.addrl = pmbase + 0x28;
	fadt->x_gpe0_blk.addrh = 0x0;

	fadt->x_gpe1_blk.space_id = 1;
	fadt->x_gpe1_blk.bit_width = 32;
	fadt->x_gpe1_blk.bit_offset = 0;
	fadt->x_gpe1_blk.resv = 0;
	fadt->x_gpe1_blk.addrl = 0x0;
	fadt->x_gpe1_blk.addrh = 0x0;

	header->checksum =
	    acpi_checksum((void *) fadt, header->length);
}
