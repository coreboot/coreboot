/*
 * This file is part of the coreboot project.
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
#include "vt8237r.h"

/**
 * Create the Fixed ACPI Description Tables (FADT) for any board with this SB.
 */
void acpi_create_fadt(acpi_fadt_t *fadt, acpi_facs_t *facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);
	struct device *dev;
	int is_vt8237s = 0;

	/* Power management controller */
	dev = dev_find_device(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_VT8237S_LPC, 0);

	if (dev)
		is_vt8237s = 1;

	memset((void *) fadt, 0, sizeof(acpi_fadt_t));
	memcpy(header->signature, "FACP", 4);
	header->length = 244;
	header->revision = 4;
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = 42;

	fadt->firmware_ctrl = (u32)facs;
	fadt->dsdt = (u32)dsdt;
	fadt->preferred_pm_profile = 0;
	fadt->sci_int = 9;
	fadt->smi_cmd = 0;
	fadt->acpi_enable = 0;
	fadt->acpi_disable = 0;
	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = 0x0;

	fadt->pm1a_evt_blk = VT8237R_ACPI_IO_BASE;
	fadt->pm1b_evt_blk = 0x0;
	fadt->pm1a_cnt_blk = VT8237R_ACPI_IO_BASE + 0x4;
	fadt->pm1b_cnt_blk = 0x0;
	/* once we support C2/C3 this could be set to 0x22 and chipset needs to be adjusted too */
	fadt->pm2_cnt_blk = 0x0;
	fadt->pm_tmr_blk = VT8237R_ACPI_IO_BASE + 0x8;
	fadt->gpe0_blk = VT8237R_ACPI_IO_BASE + 0x20;
	if (is_vt8237s) {
		fadt->gpe1_blk = VT8237R_ACPI_IO_BASE + 0x60;
		fadt->gpe1_base = 0x10;
		fadt->gpe1_blk_len = 4;
	} else {
		fadt->gpe1_blk = 0x0;
		fadt->gpe1_base = 0;
		fadt->gpe1_blk_len = 0;
	}

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm2_cnt_len = 0;
	fadt->pm_tmr_len = 4;
	fadt->gpe0_blk_len = 4;

	fadt->cst_cnt = 0;
	fadt->p_lvl2_lat = 90;
	fadt->p_lvl3_lat = 900;
	fadt->flush_size = 0;
	fadt->flush_stride = 0;
	fadt->duty_offset = 0;
	fadt->duty_width = 1;	//??
	fadt->day_alrm = 0x7d;
	fadt->mon_alrm = 0x7e;
	fadt->century = 0x32;
	/* We have legacy devices, 8042, VGA is ok to probe, MSI are not supported */
	fadt->iapc_boot_arch = 0xb;
	/* check me */
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
