/*
 * This file is part of the coreboot project.
 *
 * (C) Copyright 2004 Nick Barker <nick.barker9@btinternet.com>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * ACPI - create the Fixed ACPI Description Tables (FADT)
 */

#include <string.h>
#include <arch/acpi.h>

void acpi_create_fadt(acpi_fadt_t *fadt,acpi_facs_t *facs,void *dsdt){
	acpi_header_t *header=&(fadt->header);

	memset((void *)fadt,0,sizeof(acpi_fadt_t));
	memcpy(header->signature,"FACP",4);
	header->length = 244;
	header->revision = 3;
	memcpy(header->oem_id,OEM_ID,6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id,ASLC,4);
	header->asl_compiler_revision=0;

	fadt->firmware_ctrl=(u32)facs;
	fadt->dsdt=(u32)dsdt;
	fadt->preferred_pm_profile=0;
	fadt->sci_int=5;
	fadt->smi_cmd = 0;
	fadt->acpi_enable = 0;
	fadt->acpi_disable = 0;
	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = 0x0;

	fadt->pm1a_evt_blk = 0x400;
	fadt->pm1b_evt_blk = 0x0;
	fadt->pm1a_cnt_blk = 0x404;
	fadt->pm1b_cnt_blk = 0x0;
	fadt->pm2_cnt_blk = 0x0;
	fadt->pm_tmr_blk = 0x408;
	fadt->gpe0_blk = 0x420;
	fadt->gpe1_blk = 0x0;

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm2_cnt_len = 0;
	fadt->pm_tmr_len = 4;
	fadt->gpe0_blk_len = 4;
	fadt->gpe1_blk_len = 0;
	fadt->gpe1_base = 0;
	fadt->cst_cnt = 0;
	fadt->p_lvl2_lat = 90;
	fadt->p_lvl3_lat = 900;
	fadt->flush_size = 0;
	fadt->flush_stride = 0;
	fadt->duty_offset = 0;
	fadt->duty_width = 1;
	fadt->day_alrm = 125;
	fadt->mon_alrm = 126;
	fadt->century = 50;
	fadt->iapc_boot_arch = 0x1;
	fadt->flags = 0x4a5;

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
	fadt->x_pm1a_evt_blk.addrl = 0x400;
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
	fadt->x_pm1a_cnt_blk.addrl = 0x404;
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
	fadt->x_pm2_cnt_blk.addrl = 0x0;
	fadt->x_pm2_cnt_blk.addrh = 0x0;


	fadt->x_pm_tmr_blk.space_id = 1;
	fadt->x_pm_tmr_blk.bit_width = 4;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.resv = 0;
	fadt->x_pm_tmr_blk.addrl = 0x408;
	fadt->x_pm_tmr_blk.addrh = 0x0;


	fadt->x_gpe0_blk.space_id = 1;
	fadt->x_gpe0_blk.bit_width = 0;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.resv = 0;
	fadt->x_gpe0_blk.addrl = 0x420;
	fadt->x_gpe0_blk.addrh = 0x0;


	fadt->x_gpe1_blk.space_id = 1;
	fadt->x_gpe1_blk.bit_width = 0;
	fadt->x_gpe1_blk.bit_offset = 0;
	fadt->x_gpe1_blk.resv = 0;
	fadt->x_gpe1_blk.addrl = 0x0;
	fadt->x_gpe1_blk.addrh = 0x0;

	header->checksum = acpi_checksum((void *)fadt, sizeof(acpi_fadt_t));

}
