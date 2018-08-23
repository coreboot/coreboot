/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <string.h>
#include <device/device.h>
#include <device/pci.h>
#include <arch/acpi.h>
#include <southbridge/intel/fsp_rangeley/soc.h>
#include <arch/io.h>

#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
#include <cpu/x86/smm.h>
#endif

/**
 * Fill in the FADT with generic values that can be overridden later.
 */

typedef struct southbridge_intel_fsp_rangeley_config config_t;

void acpi_fill_in_fadt(acpi_fadt_t * fadt, acpi_facs_t * facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);
	struct device *lpcdev = dev_find_slot(SOC_LPC_DEVFN);
	u16 pmbase = pci_read_config16(lpcdev, ABASE) & 0xfff0;
	config_t *config = lpcdev->chip_info;

	memset((void *) fadt, 0, sizeof(acpi_fadt_t));

	/*
	 * Reference section 5.2.9 Fixed ACPI Description Table (FADT)
	 * in the ACPI 3.0b specification.
	 */

	/* FADT Header Structure */
	memcpy(header->signature, "FACP", 4);
	header->length = sizeof(acpi_fadt_t);
	header->revision = get_acpi_table_revision(FADT);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = 1;

	/* ACPI Pointers */
	fadt->firmware_ctrl = (unsigned long) facs;
	fadt->dsdt = (unsigned long) dsdt;

	fadt->model = 0;		/* reserved, should be 0 ACPI 3.0 */
	fadt->preferred_pm_profile = config->fadt_pm_profile; /* unknown is default */

	/* System Management */
	fadt->sci_int = 0x09;
	fadt->smi_cmd = 0x00;		/* disable SMM */
	fadt->acpi_enable = 0x00;	/* unused if SMI_CMD = 0 */
	fadt->acpi_disable = 0x00;	/* unused if SMI_CMD = 0 */

	/* Enable ACPI */
	outl(inl(pmbase + PM1_CNT) | SCI_EN, pmbase + PM1_CNT);

	/* Power Control */
	fadt->s4bios_req = 0x00;
	fadt->pstate_cnt = 0x00;

	/* Control Registers - Base Address */
	fadt->pm1a_evt_blk = pmbase + PM1_STS;
	fadt->pm1b_evt_blk = 0x00;	/* Not Used */
	fadt->pm1a_cnt_blk = pmbase + PM1_CNT;
	fadt->pm1b_cnt_blk = 0x00;	/* Not Used */
	fadt->pm2_cnt_blk = pmbase + PM2A_CNT_BLK;
	fadt->pm_tmr_blk = pmbase + PM1_TMR;
	fadt->gpe0_blk = pmbase + GPE0_STS;
	fadt->gpe1_blk = 0x00;		/* Not Used */

	/* Control Registers - Length */
	fadt->pm1_evt_len = 4;		/* 32 bits */
	fadt->pm1_cnt_len = 2;		/* 32 bit register, 16 bits used */
	fadt->pm2_cnt_len = 1;		/* 8 bits */
	fadt->pm_tmr_len = 4;		/* 32 bits */
	fadt->gpe0_blk_len = 8;		/* 64 bits */
	fadt->gpe1_blk_len = 0;
	fadt->gpe1_base = 0;
	fadt->cst_cnt = 0;
	fadt->p_lvl2_lat = ACPI_FADT_C2_NOT_SUPPORTED;
	fadt->p_lvl3_lat = ACPI_FADT_C3_NOT_SUPPORTED;
	fadt->flush_size = 0;	/* set to 0 if WBINVD is 1 in flags */
	fadt->flush_stride = 0;	/* set to 0 if WBINVD is 1 in flags */
	fadt->duty_offset = 1;
	fadt->duty_width = 0;

	/* RTC Registers */
	fadt->day_alrm = 0x0D;
	fadt->mon_alrm = 0x00;
	fadt->century = 0x00;
	fadt->iapc_boot_arch = config->fadt_boot_arch; /* legacy free default */

	fadt->flags = ACPI_FADT_WBINVD | ACPI_FADT_C1_SUPPORTED |
			ACPI_FADT_C2_MP_SUPPORTED | ACPI_FADT_SLEEP_BUTTON |
			ACPI_FADT_RESET_REGISTER | ACPI_FADT_SLEEP_TYPE |
			ACPI_FADT_S4_RTC_WAKE | ACPI_FADT_PLATFORM_CLOCK;

	/* Reset Register */
	fadt->reset_reg.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->reset_reg.bit_width = 8;
	fadt->reset_reg.bit_offset = 0;
	fadt->reset_reg.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->reset_reg.addrl = 0xCF9;
	fadt->reset_reg.addrh = 0x00;
	fadt->reset_value = 6;

	/* Reserved Bits */
	fadt->res3 = 0x00;		/* reserved, MUST be 0 ACPI 3.0 */
	fadt->res4 = 0x00;		/* reserved, MUST be 0 ACPI 3.0 */
	fadt->res5 = 0x00;		/* reserved, MUST be 0 ACPI 3.0 */

	/* Extended ACPI Pointers */
	fadt->x_firmware_ctl_l = (unsigned long)facs;
	fadt->x_firmware_ctl_h = 0x00;
	fadt->x_dsdt_l = (unsigned long)dsdt;
	fadt->x_dsdt_h = 0x00;

	/* PM1 Status & PM1 Enable */
	fadt->x_pm1a_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_evt_blk.bit_width = 32;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm1a_evt_blk.addrl = fadt->pm1a_evt_blk;
	fadt->x_pm1a_evt_blk.addrh = 0x00;

	fadt->x_pm1b_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1b_evt_blk.bit_width = 0;
	fadt->x_pm1b_evt_blk.bit_offset = 0;
	fadt->x_pm1b_evt_blk.access_size = 0;
	fadt->x_pm1b_evt_blk.addrl = fadt->pm1b_evt_blk;
	fadt->x_pm1b_evt_blk.addrh = 0x00;

	/* PM1 Control Registers */
	fadt->x_pm1a_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_cnt_blk.bit_width = 16;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_cnt_blk.addrl = fadt->pm1a_cnt_blk;
	fadt->x_pm1a_cnt_blk.addrh = 0x00;

	fadt->x_pm1b_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1b_cnt_blk.bit_width = 0;
	fadt->x_pm1b_cnt_blk.bit_offset = 0;
	fadt->x_pm1b_cnt_blk.access_size = 0;
	fadt->x_pm1b_cnt_blk.addrl = fadt->pm1b_cnt_blk;
	fadt->x_pm1b_cnt_blk.addrh = 0x00;

	/* PM2 Control Registers */
	fadt->x_pm2_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm2_cnt_blk.bit_width = 8;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_pm2_cnt_blk.addrl = fadt->pm2_cnt_blk;
	fadt->x_pm2_cnt_blk.addrh = 0x00;

	/* PM1 Timer Register */
	fadt->x_pm_tmr_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm_tmr_blk.bit_width = 32;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm_tmr_blk.addrl = fadt->pm_tmr_blk;
	fadt->x_pm_tmr_blk.addrh = 0x00;

	/*  General-Purpose Event Registers */
	fadt->x_gpe0_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe0_blk.bit_width = 64;	/* EventStatus + EventEnable */
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_gpe0_blk.addrl = fadt->gpe0_blk;
	fadt->x_gpe0_blk.addrh = 0x00;

	fadt->x_gpe1_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe1_blk.bit_width = 0;
	fadt->x_gpe1_blk.bit_offset = 0;
	fadt->x_gpe1_blk.access_size = 0;
	fadt->x_gpe1_blk.addrl = fadt->gpe1_blk;
	fadt->x_gpe1_blk.addrh = 0x00;

	header->checksum =
		acpi_checksum((void *) fadt, sizeof(acpi_fadt_t));
}
