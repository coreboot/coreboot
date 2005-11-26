/*
 * ACPI - create the Fixed ACPI Description Tables (FADT)
 * (C) Copyright 2005 Stefan Reinauer <stepan@openbios.org>
 */

#include <string.h>
#include <arch/acpi.h>

void acpi_create_fadt(acpi_fadt_t *fadt,acpi_facs_t *facs,void *dsdt){
	acpi_header_t *header=&(fadt->header);

	/* Prepare the header */
	memset((void *)fadt,0,sizeof(acpi_fadt_t));
	memcpy(header->signature,"FACP",4);
	header->length = 244;
	header->revision = 1;
	memcpy(header->oem_id,OEM_ID,6);
	memcpy(header->oem_table_id,"LXBACPI ",8);
	memcpy(header->asl_compiler_id,ASLC,4);
	header->asl_compiler_revision=0;

	fadt->firmware_ctrl=(unsigned long)facs;
	fadt->dsdt= dsdt;
	fadt->res1=0x0;
	// 3=Workstation,4=Enterprise Server, 7=Performance Server
	fadt->preferred_pm_profile=0x03;
	fadt->sci_int=9;
	// disable system management mode by setting to 0: 
	fadt->smi_cmd = 0x502f;
	fadt->acpi_enable = 0xe1;
	fadt->acpi_disable = 0x1e;
	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = 0xe2;

	fadt->pm1a_evt_blk = 0x5000;
	fadt->pm1b_evt_blk = 0x0000;
	fadt->pm1a_cnt_blk = 0x5004;
	fadt->pm1b_cnt_blk = 0x0000;
	fadt->pm2_cnt_blk  = 0x0000;
	fadt->pm_tmr_blk   = 0x5008;
	fadt->gpe0_blk     = 0x5020;
	fadt->gpe1_blk     = 0x50b0;

	fadt->pm1_evt_len  =  4;
	fadt->pm1_cnt_len  =  2;
	fadt->pm2_cnt_len  =  0;
	fadt->pm_tmr_len   =  4;
	fadt->gpe0_blk_len =  4;
	fadt->gpe1_blk_len =  8;
	fadt->gpe1_base    = 16;
	
	fadt->cst_cnt    = 0xe3;
	fadt->p_lvl2_lat =  101;
	fadt->p_lvl3_lat = 1001;
	fadt->flush_size = 1024;
	fadt->flush_stride = 16;
	fadt->duty_offset = 1;
	fadt->duty_width = 3;
	fadt->day_alrm = 0; // 0x7d these have to be
	fadt->mon_alrm = 0; // 0x7e added to cmos.layout
	fadt->century =  0; // 0x7f to make rtc alrm work
	fadt->iapc_boot_arch = 0x3; // See table 5-11
	fadt->flags = 0xa5;
	
	fadt->res2 = 0;

	fadt->reset_reg.space_id = 1;
	fadt->reset_reg.bit_width = 8;
	fadt->reset_reg.bit_offset = 0;
	fadt->reset_reg.resv = 0;
	fadt->reset_reg.addrl = 0xcf9;
	fadt->reset_reg.addrh = 0x0;

	fadt->reset_value = 0x06;
	fadt->x_firmware_ctl_l = facs;
	fadt->x_firmware_ctl_h = 0;
	fadt->x_dsdt_l = dsdt;
	fadt->x_dsdt_h = 0;

	fadt->x_pm1a_evt_blk.space_id = 1;
	fadt->x_pm1a_evt_blk.bit_width = 32;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.resv = 0;
	fadt->x_pm1a_evt_blk.addrl = 0x5000;
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
	fadt->x_pm1a_cnt_blk.addrl = 0x5004;
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
	fadt->x_pm_tmr_blk.bit_width = 32;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.resv = 0;
	fadt->x_pm_tmr_blk.addrl = 0x5008;
	fadt->x_pm_tmr_blk.addrh = 0x0;


	fadt->x_gpe0_blk.space_id = 1;
	fadt->x_gpe0_blk.bit_width = 32;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.resv = 0;
	fadt->x_gpe0_blk.addrl = 0x5020;
	fadt->x_gpe0_blk.addrh = 0x0;


	fadt->x_gpe1_blk.space_id = 1;
	fadt->x_gpe1_blk.bit_width = 64;
	fadt->x_gpe1_blk.bit_offset = 16;
	fadt->x_gpe1_blk.resv = 0;
	fadt->x_gpe1_blk.addrl = 0x50b0;
	fadt->x_gpe1_blk.addrh = 0x0;

	header->checksum = acpi_checksum((void *)fadt, sizeof(acpi_fadt_t));

}
