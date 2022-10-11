/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * ACPI - create the Fixed ACPI Description Tables (FADT)
 */

#include <console/console.h>
#include <acpi/acpi.h>
#include <arch/io.h>
#include <device/device.h>
#include <version.h>

#include "SBPLATFORM.h"

#ifndef FADT_BOOT_ARCH
#if LEGACY_FREE
	#define FADT_BOOT_ARCH ACPI_FADT_LEGACY_FREE
#else
	#define FADT_BOOT_ARCH (ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042)
#endif
#endif

/*
 * Reference section 5.2.9 Fixed ACPI Description Table (FADT)
 * in the ACPI 3.0b specification.
 */
void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	u16 val = 0;

	printk(BIOS_DEBUG, "ACPI_BLK_BASE: 0x%04x\n", ACPI_BLK_BASE);

	fadt->sci_int = 9;		/* HUDSON 1 - IRQ 09 - ACPI SCI */

	val = PM1_EVT_BLK_ADDRESS;
	WritePMIO(SB_PMIOA_REG60, AccWidthUint16, &val);
	val = PM1_CNT_BLK_ADDRESS;
	WritePMIO(SB_PMIOA_REG62, AccWidthUint16, &val);
	val = PM1_TMR_BLK_ADDRESS;
	WritePMIO(SB_PMIOA_REG64, AccWidthUint16, &val);
	val = GPE0_BLK_ADDRESS;
	WritePMIO(SB_PMIOA_REG68, AccWidthUint16, &val);

	/* CpuControl is in \_PR.CP00, 6 bytes */
	val = CPU_CNT_BLK_ADDRESS;
	WritePMIO(SB_PMIOA_REG66, AccWidthUint16, &val);
	val = 0;
	WritePMIO(SB_PMIOA_REG6A, AccWidthUint16, &val);
	val = ACPI_PMA_CNT_BLK_ADDRESS;
	WritePMIO(SB_PMIOA_REG6E, AccWidthUint16, &val);

	/* AcpiDecodeEnable, When set, SB uses the contents of the
	 * PM registers at index 60-6B to decode ACPI I/O address.
	 * AcpiSmiEn & SmiCmdEn*/
	val = BIT0 | BIT1 | BIT2 | BIT4;
	WritePMIO(SB_PMIOA_REG74, AccWidthUint16, &val);

	/* RTC_En_En, TMR_En_En, GBL_EN_EN */
	outl(0x1, PM1_CNT_BLK_ADDRESS);		  /* set SCI_EN */
	fadt->pm1a_evt_blk = PM1_EVT_BLK_ADDRESS;
	fadt->pm1a_cnt_blk = PM1_CNT_BLK_ADDRESS;
	fadt->pm2_cnt_blk = ACPI_PMA_CNT_BLK_ADDRESS;
	fadt->pm_tmr_blk = PM1_TMR_BLK_ADDRESS;
	fadt->gpe0_blk = GPE0_BLK_ADDRESS;

	fadt->pm1_evt_len = 4;	/* 32 bits */
	fadt->pm1_cnt_len = 2;	/* 16 bits */
	fadt->pm2_cnt_len = 1;	/* 8 bits */
	fadt->pm_tmr_len = 4;	/* 32 bits */
	fadt->gpe0_blk_len = 8;	/* 64 bits */

	fadt->p_lvl2_lat = ACPI_FADT_C2_NOT_SUPPORTED;
	fadt->p_lvl3_lat = ACPI_FADT_C3_NOT_SUPPORTED;
	fadt->duty_offset = 1;	/* CLK_VAL bits 3:1 */
	fadt->duty_width = 3;	/* CLK_VAL bits 3:1 */
	fadt->day_alrm = 0;	/* 0x7d these have to be */
	fadt->mon_alrm = 0;	/* 0x7e added to cmos.layout */
	fadt->iapc_boot_arch = FADT_BOOT_ARCH;	/* See table 5-10 */
	fadt->res2 = 0;		/* reserved, MUST be 0 ACPI 3.0 */
	fadt->flags |= ACPI_FADT_WBINVD | /* See table 5-10 ACPI 3.0a spec */
				ACPI_FADT_C1_SUPPORTED |
				ACPI_FADT_SLEEP_BUTTON |
				ACPI_FADT_S4_RTC_WAKE |
				ACPI_FADT_32BIT_TIMER |
				ACPI_FADT_PCI_EXPRESS_WAKE |
				ACPI_FADT_S4_RTC_VALID |
				ACPI_FADT_REMOTE_POWER_ON;

	fadt->ARM_boot_arch = 0;	/* Must be zero if ACPI Revision <= 5.0 */

	fadt->x_pm1a_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_evt_blk.bit_width = fadt->pm1_evt_len * 8;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_evt_blk.addrl = PM1_EVT_BLK_ADDRESS;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	fadt->x_pm1a_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_cnt_blk.bit_width = fadt->pm1_cnt_len * 8;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.access_size = ACPI_ACCESS_SIZE_UNDEFINED;
	fadt->x_pm1a_cnt_blk.addrl = PM1_CNT_BLK_ADDRESS;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	fadt->x_pm2_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm2_cnt_blk.bit_width = 8;	/* Hudson 1 Pm2Control is 8 bits */
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_pm2_cnt_blk.addrl = ACPI_PMA_CNT_BLK_ADDRESS;
	fadt->x_pm2_cnt_blk.addrh = 0x0;

	fadt->x_pm_tmr_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm_tmr_blk.bit_width = fadt->pm_tmr_len * 8;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm_tmr_blk.addrl = PM1_TMR_BLK_ADDRESS;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	fadt->x_gpe0_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe0_blk.bit_width = fadt->gpe0_blk_len * 8; /* EventStatus + Event Enable */
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_gpe0_blk.addrl = GPE0_BLK_ADDRESS;
	fadt->x_gpe0_blk.addrh = 0x0;
}
