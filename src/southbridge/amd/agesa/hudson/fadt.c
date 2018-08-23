/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

/*
 * ACPI - create the Fixed ACPI Description Tables (FADT)
 */

#include <string.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <device/device.h>
#include "hudson.h"
#include "smi.h"

#if IS_ENABLED(CONFIG_HUDSON_LEGACY_FREE)
	#define FADT_BOOT_ARCH ACPI_FADT_LEGACY_FREE
#else
	#define FADT_BOOT_ARCH (ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042)
#endif

#ifndef FADT_PM_PROFILE
	#define FADT_PM_PROFILE PM_UNSPECIFIED
#endif

/*
 * Reference section 5.2.9 Fixed ACPI Description Table (FADT)
 * in the ACPI 3.0b specification.
 */
void acpi_create_fadt(acpi_fadt_t * fadt, acpi_facs_t * facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);

	printk(BIOS_DEBUG, "pm_base: 0x%04x\n", HUDSON_ACPI_IO_BASE);

	/* Prepare the header */
	memset((void *)fadt, 0, sizeof(acpi_fadt_t));
	memcpy(header->signature, "FACP", 4);
	header->length = sizeof(acpi_fadt_t);
	header->revision = get_acpi_table_revision(FADT);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = 0;

	if ((uintptr_t)facs > 0xffffffff)
		printk(BIOS_DEBUG, "ACPI: FACS lives above 4G\n");
	else
		fadt->firmware_ctrl = (uintptr_t)facs;

	if ((uintptr_t)dsdt > 0xffffffff)
		printk(BIOS_DEBUG, "ACPI: DSDT lives above 4G\n");
	else
		fadt->dsdt = (uintptr_t)dsdt;

	fadt->model = 0;		/* reserved, should be 0 ACPI 3.0 */
	fadt->preferred_pm_profile = FADT_PM_PROFILE;
	fadt->sci_int = 9;		/* HUDSON - IRQ 09 - ACPI SCI */

	if (IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)) {
		fadt->smi_cmd = ACPI_SMI_CTL_PORT;
		fadt->acpi_enable = ACPI_SMI_CMD_ENABLE;
		fadt->acpi_disable = ACPI_SMI_CMD_DISABLE;
		fadt->s4bios_req = 0;	/* Not supported */
		fadt->pstate_cnt = 0;	/* Not supported */
		fadt->cst_cnt = 0;	/* Not supported */
		outl(0x0, ACPI_PM1_CNT_BLK);	/* clear SCI_EN */
	} else {
		fadt->smi_cmd = 0;	/* disable system management mode */
		fadt->acpi_enable = 0;	/* unused if SMI_CMD = 0 */
		fadt->acpi_disable = 0;	/* unused if SMI_CMD = 0 */
		fadt->s4bios_req = 0;	/* unused if SMI_CMD = 0 */
		fadt->pstate_cnt = 0;	/* unused if SMI_CMD = 0 */
		fadt->cst_cnt = 0x00;	/* unused if SMI_CMD = 0 */
		outl(0x1, ACPI_PM1_CNT_BLK);	/* set SCI_EN */
	}

	fadt->pm1a_evt_blk = ACPI_PM_EVT_BLK;
	fadt->pm1b_evt_blk = 0x0000;
	fadt->pm1a_cnt_blk = ACPI_PM1_CNT_BLK;
	fadt->pm1b_cnt_blk = 0x0000;
	fadt->pm2_cnt_blk = 0x0000;
	fadt->pm_tmr_blk = ACPI_PM_TMR_BLK;
	fadt->gpe0_blk = ACPI_GPE0_BLK;
	fadt->gpe1_blk = 0x0000;		/* No gpe1 block in hudson */

	fadt->pm1_evt_len = 4;	/* 32 bits */
	fadt->pm1_cnt_len = 2;	/* 16 bits */
	fadt->pm2_cnt_len = 0;
	fadt->pm_tmr_len = 4;	/* 32 bits */
	fadt->gpe0_blk_len = 8;	/* 64 bits */
	fadt->gpe1_blk_len = 0;
	fadt->gpe1_base = 0;

	fadt->p_lvl2_lat = ACPI_FADT_C2_NOT_SUPPORTED;
	fadt->p_lvl3_lat = ACPI_FADT_C3_NOT_SUPPORTED;
	fadt->flush_size = 0;	/* set to 0 if WBINVD is 1 in flags */
	fadt->flush_stride = 0;	/* set to 0 if WBINVD is 1 in flags */
	fadt->duty_offset = 1;	/* CLK_VAL bits 3:1 */
	fadt->duty_width = 3;	/* CLK_VAL bits 3:1 */
	fadt->day_alrm = 0;	/* 0x7d these have to be */
	fadt->mon_alrm = 0;	/* 0x7e added to cmos.layout */
	fadt->century = 0;	/* 0x7f to make rtc alarm work */
	fadt->iapc_boot_arch = FADT_BOOT_ARCH;	/* See table 5-10 */
	fadt->res2 = 0;		/* reserved, MUST be 0 ACPI 3.0 */
	fadt->flags = ACPI_FADT_WBINVD | /* See table 5-10 ACPI 3.0a spec */
				ACPI_FADT_C1_SUPPORTED |
				ACPI_FADT_SLEEP_BUTTON |
				ACPI_FADT_S4_RTC_WAKE |
				ACPI_FADT_32BIT_TIMER |
				ACPI_FADT_RESET_REGISTER |
				ACPI_FADT_PCI_EXPRESS_WAKE |
				ACPI_FADT_PLATFORM_CLOCK |
				ACPI_FADT_S4_RTC_VALID |
				ACPI_FADT_REMOTE_POWER_ON;

	/* Format is from 5.2.3.1: Generic Address Structure */
	/* reset_reg: see section 4.7.3.6 ACPI 3.0a spec */
	/* 8 bit write of value 0x06 to 0xCF9 in IO space */
	fadt->reset_reg.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->reset_reg.bit_width = 8;
	fadt->reset_reg.bit_offset = 0;
	fadt->reset_reg.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->reset_reg.addrl = 0xcf9;
	fadt->reset_reg.addrh = 0x0;

	fadt->reset_value = 6;

	fadt->res3 = 0;		/* reserved, MUST be 0 ACPI 3.0 */
	fadt->res4 = 0;		/* reserved, MUST be 0 ACPI 3.0 */
	fadt->res5 = 0;		/* reserved, MUST be 0 ACPI 3.0 */


	fadt->x_firmware_ctl_l = ((uintptr_t)facs) & 0xffffffff;
	fadt->x_firmware_ctl_h = ((uint64_t)(uintptr_t)facs) >> 32;
	fadt->x_dsdt_l = ((uintptr_t)dsdt) & 0xffffffff;
	fadt->x_dsdt_h = ((uint64_t)(uintptr_t)dsdt) >> 32;

	fadt->x_pm1a_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_evt_blk.bit_width = 32;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_evt_blk.addrl = ACPI_PM_EVT_BLK;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	fadt->x_pm1b_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1b_evt_blk.bit_width = 0;
	fadt->x_pm1b_evt_blk.bit_offset = 0;
	fadt->x_pm1b_evt_blk.access_size = 0;
	fadt->x_pm1b_evt_blk.addrl = 0x0;
	fadt->x_pm1b_evt_blk.addrh = 0x0;


	fadt->x_pm1a_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_cnt_blk.bit_width = 16;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.access_size = 0;
	fadt->x_pm1a_cnt_blk.addrl = ACPI_PM1_CNT_BLK;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	fadt->x_pm1b_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1b_cnt_blk.bit_width = 0;
	fadt->x_pm1b_cnt_blk.bit_offset = 0;
	fadt->x_pm1b_cnt_blk.access_size = 0;
	fadt->x_pm1b_cnt_blk.addrl = 0x0;
	fadt->x_pm1b_cnt_blk.addrh = 0x0;

	/*
	 * Note: Under this current AMD C state implementation, this is no longer
	 *       used and should not be reported to OS.
	 */
	fadt->x_pm2_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm2_cnt_blk.bit_width = 0;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_pm2_cnt_blk.addrl = 0;
	fadt->x_pm2_cnt_blk.addrh = 0x0;


	fadt->x_pm_tmr_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm_tmr_blk.bit_width = 32;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm_tmr_blk.addrl = ACPI_PM_TMR_BLK;
	fadt->x_pm_tmr_blk.addrh = 0x0;


	fadt->x_gpe0_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe0_blk.bit_width = 64; /* EventStatus + Event Enable */
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_gpe0_blk.addrl = ACPI_GPE0_BLK;
	fadt->x_gpe0_blk.addrh = 0x0;


	fadt->x_gpe1_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe1_blk.bit_width = 0;
	fadt->x_gpe1_blk.bit_offset = 0;
	fadt->x_gpe1_blk.access_size = 0;
	fadt->x_gpe1_blk.addrl = 0;
	fadt->x_gpe1_blk.addrh = 0x0;

	header->checksum = acpi_checksum((void *)fadt, sizeof(acpi_fadt_t));
}
