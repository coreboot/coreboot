/*
 * ACPI - create the Fixed ACPI Description Tables (FADT)
 *
 * Copyright (C) 2004 Nick Barker <nick.barker9@btinternet.com>
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
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

#include <string.h>
#include <arch/acpi.h>
#include "northbridge/via/vx800/vx800.h"

void acpi_create_fadt(acpi_fadt_t *fadt, acpi_facs_t *facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);

	memset((void *)fadt, 0, sizeof(acpi_fadt_t));
	memcpy(header->signature, "FACP", 4);
	header->length = 244;
	header->revision = 3;
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = 0;

	fadt->firmware_ctrl = (u32)facs;
	fadt->dsdt = (u32)dsdt;
	fadt->preferred_pm_profile = 0;
	fadt->sci_int = 0x9;

	fadt->smi_cmd = VX800_ACPI_IO_BASE + 0x2F;
	fadt->acpi_enable = 0xA1;
	fadt->acpi_disable = 0xA0;

	/*
	 * Value 42F,A1,A0, if we don't want SMI, then set them to zero.
	 * fadt->smi_cmd = 0x0;
	 * fadt->acpi_enable = 0x0;
	 * fadt->acpi_disable = 0x0;
	 */

	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = 0x0;

	fadt->pm1a_evt_blk = VX800_ACPI_IO_BASE;
	fadt->pm1b_evt_blk = 0x0;
	fadt->pm1a_cnt_blk = VX800_ACPI_IO_BASE + 0x4;
	fadt->pm1b_cnt_blk = 0x0;
	fadt->pm2_cnt_blk = 0x22;	/* To support cpu-c3. */
	/* fadt->pm2_cnt_blk = 0x0; */
	fadt->pm_tmr_blk = VX800_ACPI_IO_BASE + 0x8;
	fadt->gpe0_blk = VX800_ACPI_IO_BASE + 0x20;
	fadt->gpe1_blk = VX800_ACPI_IO_BASE + 0x50;

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm2_cnt_len = 1;	/* To support cpu-c3. */
	/* fadt->pm2_cnt_len = 0; */

	fadt->pm_tmr_len = 4;
	fadt->gpe0_blk_len = 4;
	fadt->gpe1_blk_len = 4;
	fadt->gpe1_base = 0x10;
	fadt->cst_cnt = 0;

	fadt->p_lvl2_lat = 0x50;	/* This is the coreboot source. */
	fadt->p_lvl3_lat = 0x320;
	/* fadt->p_lvl2_lat = 0x80; */
	/* fadt->p_lvl3_lat = 0x800; */
	/* fadt->p_lvl2_lat = 0x1; */
	/* fadt->p_lvl3_lat = 0x23; */

	/* fadt->p_lvl2_lat = 0x200; */	/* Disable. */
	/* fadt->p_lvl3_lat = 0x2000; */

	fadt->flush_size = 0;
	fadt->flush_stride = 0;
	fadt->duty_offset = 0;
	/* fadt->duty_width = 1; */
	fadt->duty_width = 4;
	fadt->day_alrm = 0x7d;
	fadt->mon_alrm = 0x7e;
	fadt->century = 0x32;
	fadt->iapc_boot_arch = 0x0;
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
	fadt->x_pm1a_evt_blk.bit_width = 4;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.resv = 0;
	fadt->x_pm1a_evt_blk.addrl = VX800_ACPI_IO_BASE;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	fadt->x_pm1b_evt_blk.space_id = 1;
	fadt->x_pm1b_evt_blk.bit_width = 4;
	fadt->x_pm1b_evt_blk.bit_offset = 0;
	fadt->x_pm1b_evt_blk.resv = 0;
	fadt->x_pm1b_evt_blk.addrl = 0x0;
	fadt->x_pm1b_evt_blk.addrh = 0x0;

	fadt->x_pm1a_cnt_blk.space_id = 1;
	fadt->x_pm1a_cnt_blk.bit_width = 2;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.resv = 0;
	fadt->x_pm1a_cnt_blk.addrl = VX800_ACPI_IO_BASE + 0x4;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	fadt->x_pm1b_cnt_blk.space_id = 1;
	fadt->x_pm1b_cnt_blk.bit_width = 2;
	fadt->x_pm1b_cnt_blk.bit_offset = 0;
	fadt->x_pm1b_cnt_blk.resv = 0;
	fadt->x_pm1b_cnt_blk.addrl = 0x0;
	fadt->x_pm1b_cnt_blk.addrh = 0x0;

	/* fadt->x_pm2_cnt_blk.space_id = 1; */
	fadt->x_pm2_cnt_blk.space_id = 0;
	fadt->x_pm2_cnt_blk.bit_width = 0;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.resv = 0;
	fadt->x_pm2_cnt_blk.addrl = 0x0;
	fadt->x_pm2_cnt_blk.addrh = 0x0;

	fadt->x_pm_tmr_blk.space_id = 1;
	fadt->x_pm_tmr_blk.bit_width = 4;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.resv = 0;
	fadt->x_pm_tmr_blk.addrl = VX800_ACPI_IO_BASE + 0x8;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	fadt->x_gpe0_blk.space_id = 1;
	fadt->x_gpe0_blk.bit_width = 0;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.resv = 0;
	fadt->x_gpe0_blk.addrl = VX800_ACPI_IO_BASE + 0x20;
	fadt->x_gpe0_blk.addrh = 0x0;

	fadt->x_gpe1_blk.space_id = 1;
	fadt->x_gpe1_blk.bit_width = 0;
	fadt->x_gpe1_blk.bit_offset = 0;
	fadt->x_gpe1_blk.resv = 0;
	fadt->x_gpe1_blk.addrl = 0x0;
	fadt->x_gpe1_blk.addrh = 0x0;

	header->checksum = acpi_checksum((void *)fadt, sizeof(acpi_fadt_t));
}
