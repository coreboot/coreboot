/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
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

void acpi_create_fadt(acpi_fadt_t * fadt, acpi_facs_t * facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);
	u16 pmbase = pci_read_config16(dev_find_slot(0, PCI_DEVFN(0x1f,0)), 0x40) & 0xfffe;

	memset((void *) fadt, 0, sizeof(acpi_fadt_t));
	memcpy(header->signature, "FACP", 4);
	header->length = 132;
	header->revision = 1;
	memcpy(header->oem_id, "CORE  ", 6);
	memcpy(header->oem_table_id, "COREBOOT", 8);
	memcpy(header->asl_compiler_id, "CORE", 4);
	header->asl_compiler_revision = 0;

	fadt->firmware_ctrl = (unsigned long) facs;
	fadt->dsdt = (unsigned long) dsdt;
	fadt->preferred_pm_profile = 0;
	fadt->sci_int = 0x9;
	fadt->smi_cmd = 0xb2;
	fadt->acpi_enable = 0xe1;
	fadt->acpi_disable = 0x1e;
	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = 0xe2;

	fadt->pm1a_evt_blk = pmbase;
	fadt->pm1b_evt_blk = 0x0;
	fadt->pm1a_cnt_blk = pmbase + 0x4;
	fadt->pm1b_cnt_blk = 0x0;
	fadt->pm2_cnt_blk = pmbase + 0x20;
	fadt->pm_tmr_blk = pmbase + 0x8;
	fadt->gpe0_blk = pmbase + 0x28;
	fadt->gpe1_blk = 0;

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm2_cnt_len = 1;
	fadt->pm_tmr_len = 4;
	fadt->gpe0_blk_len = 8;
	fadt->gpe1_blk_len = 0;
	fadt->gpe1_base = 0;
	fadt->cst_cnt = 0xe3;
	fadt->p_lvl2_lat = 1;
	fadt->p_lvl3_lat = 85;
	fadt->flush_size = 1024;
	fadt->flush_stride = 16;
	fadt->duty_offset = 1;
	fadt->duty_width = 0;
	fadt->day_alrm = 0xd;
	fadt->mon_alrm = 0x00;
	fadt->century = 0x00;
	fadt->iapc_boot_arch = 0x03;
	fadt->flags = 0x80a5;
	// wbinvd is operational
	// all cpus support c1
	// sleep button is generic
	// rtc wakeup/s4 not possible
	
	header->checksum =
	    acpi_checksum((void *) fadt, header->length);

}
