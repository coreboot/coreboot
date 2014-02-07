/*
 * ACPI - create the Fixed ACPI Description Tables (FADT)
 * (C) Copyright 2005 Stefan Reinauer <stepan@openbios.org>
 * Copyright (C) 2011,2014 Oskar Enoksson <enok@lysator.liu.se>
 */

#include <string.h>
#include <console/console.h>
#include <arch/acpi.h>

extern unsigned pm_base; /* pm_base should be set in sb acpi */

void acpi_create_fadt(acpi_fadt_t *fadt,acpi_facs_t *facs,void *dsdt){

	acpi_header_t *header=&(fadt->header);

	printk(BIOS_DEBUG, "pm_base: 0x%04x\n", pm_base);

	/* Prepare the header */
	memset((void *)fadt,0,sizeof(acpi_fadt_t));
	memcpy(header->signature,"FACP",4);
	header->length = 244;
	header->revision = 3;
	memcpy(header->oem_id,OEM_ID,6);
	memcpy(header->oem_table_id,"COREBOOT",8);
	memcpy(header->asl_compiler_id,ASLC,4);
	header->asl_compiler_revision=0;

	fadt->firmware_ctrl=(u32)facs;
	fadt->dsdt= (u32)dsdt;
	// 3=Workstation,4=Enterprise Server, 7=Performance Server
	fadt->preferred_pm_profile=0x04;
	fadt->sci_int=9;

	// disable system management mode by setting to 0:
	fadt->smi_cmd = 0;//pm_base+0x2f;
	fadt->acpi_enable = 0xf0;
	fadt->acpi_disable = 0xf1;
	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = 0xe2;

	fadt->pm1a_evt_blk = pm_base;
	fadt->pm1b_evt_blk = 0x0000;
	fadt->pm1a_cnt_blk = pm_base+0x04;
	fadt->pm1b_cnt_blk = 0x0000;
	fadt->pm2_cnt_blk  = 0x0000;
	fadt->pm_tmr_blk   = pm_base+0x08;
	fadt->gpe0_blk     = pm_base+0x20;
	fadt->gpe1_blk     = pm_base+0xb0;

	fadt->pm1_evt_len  =  4;
	fadt->pm1_cnt_len  =  2;
	fadt->pm2_cnt_len  =  0;
	fadt->pm_tmr_len   =  4;
	fadt->gpe0_blk_len =  4;
	fadt->gpe1_blk_len =  8;
	fadt->gpe1_base    = 16;

	fadt->cst_cnt    = 0xe3;
	fadt->p_lvl2_lat =  101; // > 100 means system doesnt support C2 state
	fadt->p_lvl3_lat = 1001; // > 1000 means system doesnt support C3 state
	fadt->flush_size = 0;    // ignored if wbindv=1 in flags
	fadt->flush_stride = 0;  // ignored if wbindv=1 in flags
	fadt->duty_offset = 1;
	fadt->duty_width = 3;    // 0 means duty cycle not supported
	// _alrm value 0 means RTC alarm feature not supported
	fadt->day_alrm = 0; // 0x7d these have to be
	fadt->mon_alrm = 0; // 0x7e added to cmos.layout
	fadt->century =  0; // 0x7f to make rtc alrm work
	fadt->iapc_boot_arch =
	ACPI_FADT_LEGACY_DEVICES   |
	ACPI_FADT_8042             |
	  // ACPI_FADT_VGA_NOT_PRESENT  |
	  // ACPI_FADT_MSI_NOT_SUPPORTED|
	  // ACPI_FADT_NO_PCIE_ASPM_CONTROL|
	  0;

	fadt->res2 = 0;

	fadt->flags =
	  ACPI_FADT_WBINVD           |
	  // ACPI_FADT_WBINVD_FLUSH     |
	  ACPI_FADT_C1_SUPPORTED     |
	  // ACPI_FADT_C2_MP_SUPPORTED  |
	  // ACPI_FADT_POWER_BUTTON     |
	  ACPI_FADT_SLEEP_BUTTON     |
	  // ACPI_FADT_FIXED_RTC        |
	  // ACPI_FADT_S4_RTC_WAKE      |
	  // ACPI_FADT_32BIT_TIMER      |
	  // ACPI_FADT_DOCKING_SUPPORTED|
	  // ACPI_FADT_RESET_REGISTER   |
	  // ACPI_FADT_SEALED_CASE      |
	  // ACPI_FADT_HEADLESS         |
	  // ACPI_FADT_SLEEP_TYPE       |
	  // ACPI_FADT_PCI_EXPRESS_WAKE |
	  // ACPI_FADT_PLATFORM_CLOCK   |
	  // ACPI_FADT_S4_RTC_VALID     |
	  // ACPI_FADT_REMOTE_POWER_ON  |
	  // ACPI_FADT_APIC_CLUSTER     |
	  // ACPI_FADT_APIC_PHYSICAL    |
	  0;

	fadt->reset_reg.space_id = 1;
	fadt->reset_reg.bit_width = 8;
	fadt->reset_reg.bit_offset = 0;
	fadt->reset_reg.resv = 0;
	fadt->reset_reg.addrl = 0xcf9;
	fadt->reset_reg.addrh = 0x0;

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
	fadt->x_pm1a_evt_blk.addrl = pm_base;
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
	fadt->x_pm1a_cnt_blk.addrl = pm_base+4;
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
	fadt->x_pm_tmr_blk.addrl = pm_base+0x08;
	fadt->x_pm_tmr_blk.addrh = 0x0;


	fadt->x_gpe0_blk.space_id = 1;
	fadt->x_gpe0_blk.bit_width = 32;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.resv = 0;
	fadt->x_gpe0_blk.addrl = pm_base+0x20;
	fadt->x_gpe0_blk.addrh = 0x0;


	fadt->x_gpe1_blk.space_id = 1;
	fadt->x_gpe1_blk.bit_width = 64;
	fadt->x_gpe1_blk.bit_offset = 16;
	fadt->x_gpe1_blk.resv = 0;
	fadt->x_gpe1_blk.addrl = pm_base+0xb0;
	fadt->x_gpe1_blk.addrh = 0x0;

	header->checksum = acpi_checksum((void *)fadt, sizeof(acpi_fadt_t));

}
