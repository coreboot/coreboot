/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

/*
 * ACPI - create the Fixed ACPI Description Tables (FADT)
 */

#include <string.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <device/device.h>
#include <../southbridge/amd/sb600/sb600.h>

/*extern*/ u16 pm_base = 0x800;
/* pm_base should be set in sb acpi */
/* pm_base should be got from bar2 of rs690. Here I compact ACPI
 * registers into 32 bytes limit.
 * */

#define ACPI_PM_EVT_BLK		(pm_base + 0x00) /* 4 bytes */
#define ACPI_PM1_CNT_BLK	(pm_base + 0x04) /* 2 bytes */
#define ACPI_PMA_CNT_BLK	(pm_base + 0x0F) /* 1 byte */
#define ACPI_PM_TMR_BLK		(pm_base + 0x18) /* 4 bytes */
#define ACPI_GPE0_BLK		(pm_base + 0x10) /* 8 bytes */
#define ACPI_CPU_CONTORL	(pm_base + 0x08) /* 6 bytes */

void acpi_create_fadt(acpi_fadt_t * fadt, acpi_facs_t * facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);

	pm_base &= 0xFFFF;
	printk(BIOS_DEBUG, "pm_base: 0x%04x\n", pm_base);

	/* Prepare the header */
	memset((void *)fadt, 0, sizeof(acpi_fadt_t));
	memcpy(header->signature, "FACP", 4);
	header->length = 244;
	header->revision = 3;
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->oem_revision = 0x20101005;
	header->asl_compiler_revision = 3;

	fadt->firmware_ctrl = (u32) facs;
	fadt->dsdt = (u32) dsdt;
	/* 3=Workstation,4=Enterprise Server, 7=Performance Server */
	fadt->preferred_pm_profile = 0x03;
	fadt->sci_int = 9;
	/* disable system management mode by setting to 0: */
	fadt->smi_cmd = 0;
	fadt->acpi_enable = 0xf0;
	fadt->acpi_disable = 0xf1;
	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = 0xe2;

	pm_iowrite(0x20, ACPI_PM_EVT_BLK & 0xFF);
	pm_iowrite(0x21, ACPI_PM_EVT_BLK >> 8);
	pm_iowrite(0x22, ACPI_PM1_CNT_BLK & 0xFF);
	pm_iowrite(0x23, ACPI_PM1_CNT_BLK >> 8);
	pm_iowrite(0x24, ACPI_PM_TMR_BLK & 0xFF);
	pm_iowrite(0x25, ACPI_PM_TMR_BLK >> 8);
	pm_iowrite(0x28, ACPI_GPE0_BLK & 0xFF);
	pm_iowrite(0x29, ACPI_GPE0_BLK >> 8);

	/* CpuControl is in \_PR.CPU0, 6 bytes */
	pm_iowrite(0x26, ACPI_CPU_CONTORL & 0xFF);
	pm_iowrite(0x27, ACPI_CPU_CONTORL >> 8);

	pm_iowrite(0x2A, 0);	/* AcpiSmiCmdLo */
	pm_iowrite(0x2B, 0);	/* AcpiSmiCmdHi */

	pm_iowrite(0x2C, ACPI_PMA_CNT_BLK & 0xFF);
	pm_iowrite(0x2D, ACPI_PMA_CNT_BLK >> 8);

	pm_iowrite(0x0E, 1<<3 | 0<<2); /* AcpiDecodeEnable, When set, SB uses
					* the contents of the PM registers at
					* index 20-2B to decode ACPI I/O address.
					* AcpiSmiEn & SmiCmdEn*/
	pm_iowrite(0x10, 1<<1 | 1<<3| 1<<5); /* RTC_En_En, TMR_En_En, GBL_EN_EN */
	outl(0x1, ACPI_PM1_CNT_BLK);		  /* set SCI_EN */

	fadt->pm1a_evt_blk = ACPI_PM_EVT_BLK;
	fadt->pm1b_evt_blk = 0x0000;
	fadt->pm1a_cnt_blk = ACPI_PM1_CNT_BLK;
	fadt->pm1b_cnt_blk = 0x0000;
	fadt->pm2_cnt_blk = ACPI_PMA_CNT_BLK;
	fadt->pm_tmr_blk = ACPI_PM_TMR_BLK;
	fadt->gpe0_blk = ACPI_GPE0_BLK;
	fadt->gpe1_blk = 0x0000;	/* we dont have gpe1 block, do we? */

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm2_cnt_len = 1;
	fadt->pm_tmr_len = 4;
	fadt->gpe0_blk_len = 8;
	fadt->gpe1_blk_len = 0;
	fadt->gpe1_base = 0;

	fadt->cst_cnt = 0xe3;
	fadt->p_lvl2_lat = 101;
	fadt->p_lvl3_lat = 1001;
	fadt->flush_size = 0;
	fadt->flush_stride = 0;
	fadt->duty_offset = 1;
	fadt->duty_width = 3;
	fadt->day_alrm = 0;	/* 0x7d these have to be */
	fadt->mon_alrm = 0;	/* 0x7e added to cmos.layout */
	fadt->century = 0;	/* 0x7f to make rtc alrm work */
	fadt->iapc_boot_arch = 0x3;	/* See table 5-11 */
	fadt->flags = 0x0001c1a5;/* 0x25; */

	fadt->res2 = 0;

	fadt->reset_reg.space_id = 1;
	fadt->reset_reg.bit_width = 8;
	fadt->reset_reg.bit_offset = 0;
	fadt->reset_reg.resv = 0;
	fadt->reset_reg.addrl = 0xcf9;
	fadt->reset_reg.addrh = 0x0;

	fadt->reset_value = 6;
	fadt->x_firmware_ctl_l = (u32) facs;
	fadt->x_firmware_ctl_h = 0;
	fadt->x_dsdt_l = (u32) dsdt;
	fadt->x_dsdt_h = 0;

	fadt->x_pm1a_evt_blk.space_id = 1;
	fadt->x_pm1a_evt_blk.bit_width = 32;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.resv = 0;
	fadt->x_pm1a_evt_blk.addrl = ACPI_PM_EVT_BLK;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	fadt->x_pm1b_evt_blk.space_id = 1;
	fadt->x_pm1b_evt_blk.bit_width = 4;
	fadt->x_pm1b_evt_blk.bit_offset = 0;
	fadt->x_pm1b_evt_blk.resv = 0;
	fadt->x_pm1b_evt_blk.addrl = 0x0;
	fadt->x_pm1b_evt_blk.addrh = 0x0;

	fadt->x_pm1a_cnt_blk.space_id = 1;
	fadt->x_pm1a_cnt_blk.bit_width = 16;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.resv = 0;
	fadt->x_pm1a_cnt_blk.addrl = ACPI_PM1_CNT_BLK;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	fadt->x_pm1b_cnt_blk.space_id = 1;
	fadt->x_pm1b_cnt_blk.bit_width = 2;
	fadt->x_pm1b_cnt_blk.bit_offset = 0;
	fadt->x_pm1b_cnt_blk.resv = 0;
	fadt->x_pm1b_cnt_blk.addrl = 0x0;
	fadt->x_pm1b_cnt_blk.addrh = 0x0;

	fadt->x_pm2_cnt_blk.space_id = 1;
	fadt->x_pm2_cnt_blk.bit_width = 0;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.resv = 0;
	fadt->x_pm2_cnt_blk.addrl = ACPI_PMA_CNT_BLK;
	fadt->x_pm2_cnt_blk.addrh = 0x0;

	fadt->x_pm_tmr_blk.space_id = 1;
	fadt->x_pm_tmr_blk.bit_width = 32;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.resv = 0;
	fadt->x_pm_tmr_blk.addrl = ACPI_PM_TMR_BLK;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	fadt->x_gpe0_blk.space_id = 1;
	fadt->x_gpe0_blk.bit_width = 32;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.resv = 0;
	fadt->x_gpe0_blk.addrl = ACPI_GPE0_BLK;
	fadt->x_gpe0_blk.addrh = 0x0;

	fadt->x_gpe1_blk.space_id = 1;
	fadt->x_gpe1_blk.bit_width = 0;
	fadt->x_gpe1_blk.bit_offset = 0;
	fadt->x_gpe1_blk.resv = 0;
	fadt->x_gpe1_blk.addrl = 0;
	fadt->x_gpe1_blk.addrh = 0x0;

	header->checksum = acpi_checksum((void *)fadt, sizeof(acpi_fadt_t));
}
