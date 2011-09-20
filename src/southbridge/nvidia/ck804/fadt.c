/*
 * ACPI - create the Fixed ACPI Description Tables (FADT)
 * (C) Copyright 2005 Stefan Reinauer <stepan@openbios.org>
 */

#include <string.h>
#include <console/console.h>
#include <arch/acpi.h>

extern unsigned pm_base;	/* pm_base should be set in sb acpi */

void acpi_create_fadt(acpi_fadt_t * fadt, acpi_facs_t * facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);

	printk(BIOS_DEBUG, "pm_base: 0x%04x\n", pm_base);

	/* Prepare the header */
	memset((void *)fadt, 0, sizeof(acpi_fadt_t));
	memcpy(header->signature, "FACP", 4);
#ifdef LONG_FADT
	header->length = 244;
	header->revision = 3;
#else
	header->length = 0x74;
	header->revision = 1;
#endif
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = 0;

	fadt->firmware_ctrl = (u32)facs;
	fadt->dsdt = (u32)dsdt;
	// 3=Workstation,4=Enterprise Server, 7=Performance Server
	fadt->preferred_pm_profile = 0;
	fadt->sci_int = 9;
	// disable system management mode by setting to 0:
	fadt->smi_cmd = 0;
	fadt->acpi_enable = 0;
	fadt->acpi_disable = 0;
	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = 0x0;

	fadt->pm1a_evt_blk = pm_base;
	fadt->pm1b_evt_blk = 0x0000;
	fadt->pm1a_cnt_blk = pm_base + 0x04;
	fadt->pm1b_cnt_blk = 0x0000;
	fadt->pm2_cnt_blk = pm_base + 0x1c;
	fadt->pm_tmr_blk = pm_base + 0x08;
	fadt->gpe0_blk = pm_base + 0x20;
	fadt->gpe1_blk = 0x0000;

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm2_cnt_len = 1;
	fadt->pm_tmr_len = 4;
	fadt->gpe0_blk_len = 8;
	fadt->gpe1_blk_len = 0;
	fadt->gpe1_base = 0;
	fadt->cst_cnt = 0;
	fadt->p_lvl2_lat = 0xffff;
	fadt->p_lvl3_lat = 0xffff;
	fadt->flush_size = 0;
	fadt->flush_stride = 0;
	fadt->duty_offset = 1;
	fadt->duty_width = 0;
	fadt->day_alrm = 0x7d;
	fadt->mon_alrm = 0x7e;
	fadt->century = 0x32;
	fadt->iapc_boot_arch = 0;
	fadt->flags = 0xa5;

#ifdef LONG_FADT
	fadt->res2 = 0;

	fadt->reset_reg.space_id = 1;
	fadt->reset_reg.bit_width = 8;
	fadt->reset_reg.bit_offset = 0;
	fadt->reset_reg.resv = 0;
	fadt->reset_reg.addrl = 0xcf9;
	fadt->reset_reg.addrh = 0x0;

	fadt->reset_value = 6;
	fadt->x_firmware_ctl_l = facs;
	fadt->x_firmware_ctl_h = 0;
	fadt->x_dsdt_l = dsdt;
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
	fadt->x_pm1a_cnt_blk.addrl = pm_base + 4;
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
	fadt->x_pm_tmr_blk.addrl = pm_base + 0x08;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	fadt->x_gpe0_blk.space_id = 1;
	fadt->x_gpe0_blk.bit_width = 32;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.resv = 0;
	fadt->x_gpe0_blk.addrl = pm_base + 0x20;
	fadt->x_gpe0_blk.addrh = 0x0;

	fadt->x_gpe1_blk.space_id = 1;
	fadt->x_gpe1_blk.bit_width = 64;
	fadt->x_gpe1_blk.bit_offset = 16;
	fadt->x_gpe1_blk.resv = 0;
	fadt->x_gpe1_blk.addrl = pm_base + 0xb0;
	fadt->x_gpe1_blk.addrh = 0x0;
#endif
	header->checksum = acpi_checksum((void *)fadt, header->length);
}
