/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008-2010 by coresystems GmbH
 *  written by Stefan Reinauer <stepan@coresystems.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include "inteltool.h"

static const io_register_t pch_pm_registers[] = {
	{ 0x00, 2, "PM1_STS" }, // PM1 Status; ACPI pointer: PM1a_EVT_BLK
	{ 0x02, 2, "PM1_EN" },  // PM1 Enables; ACPI pointer: PM1a_EVT_BLK+2
	{ 0x04, 4, "PM1_CNT" }, // PM1 Control; ACPI pointer: PM1a_CNT_BLK
	{ 0x08, 4, "PM1_TMR" }, // PM1 Timer; ACPI pointer: PMTMR_BLK
	{ 0x0c, 4, "RESERVED" },
	{ 0x10, 4, "RESERVED" },
	{ 0x14, 4, "RESERVED" },
	{ 0x18, 4, "RESERVED" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 8, "GPE0_STS" }, // General Purpose Event 0 Status; ACPI pointer: GPE0_BLK
	{ 0x28, 8, "GPE0_EN" },  // General Purpose Event 0 Enables; ACPI pointer: GPE0_BLK+8
	{ 0x30, 4, "SMI_EN" },
	{ 0x34, 4, "SMI_STS" },
	{ 0x38, 2, "ALT_GP_SMI_EN" },
	{ 0x3a, 2, "ALT_GP_SMI_STS" },
	{ 0x3c, 1, "UPRWC" },   // USB Per-Port registers write control
	{ 0x3d, 1, "RESERVED" },
	{ 0x3e, 2, "RESERVED" },
	{ 0x40, 2, "RESERVED" },
	{ 0x42, 1, "GPE_CNTL" },
	{ 0x43, 1, "RESERVED" },
	{ 0x44, 2, "DEVACT_STS" }, // Device Activity Status
	{ 0x46, 2, "RESERVED" },
	{ 0x48, 4, "RESERVED" },
	{ 0x4c, 4, "RESERVED" },
	{ 0x50, 1, "PM2_CNT" },
	{ 0x51, 1, "RESERVED" },
	{ 0x52, 2, "RESERVED" },
	{ 0x54, 4, "RESERVED" },
	{ 0x58, 4, "RESERVED" },
	{ 0x5c, 4, "RESERVED" },
	/* The TCO registers start here. */
	{ 0x60, 2, "TCO_RLD" },
	{ 0x62, 1, "TCO_DAT_IN" },
	{ 0x63, 1, "TCO_DAT_OUT" },
	{ 0x64, 2, "TCO1_STS" },
	{ 0x66, 2, "TCO2_STS" },
	{ 0x68, 2, "TCO1_CNT" },
	{ 0x6a, 2, "TCO2_CNT" },
	{ 0x6c, 2, "TCO_MESSAGE" },
	{ 0x6e, 1, "TCO_WDCNT" },
	{ 0x6f, 1, "RESERVED" },
	{ 0x70, 1, "SW_IRQ_GEN" },
	{ 0x71, 1, "RESERVED" },
	{ 0x72, 2, "TCO_TMR" },
	{ 0x74, 4, "RESERVED" },
	{ 0x78, 4, "RESERVED" },
	{ 0x7c, 4, "RESERVED" },
};

static const io_register_t ich10_pm_registers[] = {
	{ 0x00, 2, "PM1_STS" }, // PM1 Status; ACPI pointer: PM1a_EVT_BLK
	{ 0x02, 2, "PM1_EN" },  // PM1 Enables; ACPI pointer: PM1a_EVT_BLK+2
	{ 0x04, 4, "PM1_CNT" }, // PM1 Control; ACPI pointer: PM1a_CNT_BLK
	{ 0x08, 4, "PM1_TMR" }, // PM1 Timer; ACPI pointer: PMTMR_BLK
	{ 0x0c, 4, "RESERVED" },
	{ 0x10, 4, "PROC_CNT" }, // Processor Control; ACPI pointer: P_BLK
#if DANGEROUS_REGISTERS
	/* These registers return 0 on read, but reading them may cause
	 * the system to enter Cx states, which might hang the system.
	 */
	{ 0x14, 1, "LV2 (Mobile)" },
	{ 0x15, 1, "LV3 (Mobile)" },
	{ 0x16, 1, "LV4 (Mobile)" },
#endif
	{ 0x17, 2, "RESERVED" },
	{ 0x19, 1, "RESERVED" },
	{ 0x1a, 2, "RESERVED" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 8, "GPE0_STS" }, // General Purpose Event 0 Status; ACPI pointer: GPE0_BLK
	{ 0x28, 8, "GPE0_EN" },  // General Purpose Event 0 Enables; ACPI pointer: GPE0_BLK+8
	{ 0x30, 4, "SMI_EN" },
	{ 0x34, 4, "SMI_STS" },
	{ 0x38, 2, "ALT_GP_SMI_EN" },
	{ 0x3a, 2, "ALT_GP_SMI_STS" },
	{ 0x3c, 1, "UPRWC" },   // USB Per-Port registers write control;
	{ 0x3d, 2, "RESERVED" },
	{ 0x3f, 1, "RESERVED" },
	{ 0x40, 2, "RESERVED" },
	{ 0x42, 1, "GPE_CNTL" },
	{ 0x43, 1, "RESERVED" },
	{ 0x44, 2, "DEVACT_STS" }, // Device Activity Status
	{ 0x46, 2, "RESERVED" },
	{ 0x48, 4, "RESERVED" },
	{ 0x4c, 4, "RESERVED" },
	{ 0x50, 1, "PM2_CNT (Mobile)" }, // PM2 Control (Mobile only); ACPI pointer: PM2a_CNT_BLK
	{ 0x51, 1, "RESERVED" },
	{ 0x52, 2, "RESERVED" },
	{ 0x54, 4, "C3_RES (Mobile)" },
	{ 0x58, 4, "RESERVED" },
	{ 0x5c, 4, "RESERVED" },
	/* Here start the TCO registers */
	{ 0x60, 2, "TCO_RLD" },
	{ 0x62, 1, "TCO_DAT_IN" },
	{ 0x63, 1, "TCO_DAT_OUT" },
	{ 0x64, 2, "TCO1_STS" },
	{ 0x66, 2, "TCO2_STS" },
	{ 0x68, 2, "TCO1_CNT" },
	{ 0x6a, 2, "TCO2_CNT" },
	{ 0x6c, 2, "TCO_MESSAGE" },
	{ 0x6e, 1, "TCO_WDCNT" },
	{ 0x6f, 1, "RESERVED" },
	{ 0x70, 1, "SW_IRQ_GEN" },
	{ 0x71, 1, "RESERVED" },
	{ 0x72, 2, "TCO_TMR" },
	{ 0x74, 4, "RESERVED" },
	{ 0x78, 4, "RESERVED" },
	{ 0x7c, 4, "RESERVED" },
};

static const io_register_t ich9_pm_registers[] = {
	{ 0x00, 2, "PM1_STS" }, // PM1 Status; ACPI pointer: PM1a_EVT_BLK
	{ 0x02, 2, "PM1_EN" },  // PM1 Enables; ACPI pointer: PM1a_EVT_BLK+2
	{ 0x04, 4, "PM1_CNT" }, // PM1 Control; ACPI pointer: PM1a_CNT_BLK
	{ 0x08, 4, "PM1_TMR" }, // PM1 Timer; ACPI pointer: PMTMR_BLK
	{ 0x0c, 4, "RESERVED" },
	{ 0x10, 4, "PROC_CNT" }, // Processor Control; ACPI pointer: P_BLK
#if DANGEROUS_REGISTERS
	/* These registers return 0 on read, but reading them may cause
	 * the system to enter Cx states, which might hang the system.
	 */
	{ 0x14, 1, "LV2 (Mobile)" },
	{ 0x15, 1, "LV3 (Mobile)" },
	{ 0x16, 1, "LV4 (Mobile)" },
	{ 0x17, 1, "LV5 (Mobile)" },
	{ 0x18, 1, "LV6 (Mobile)" },
#endif
	{ 0x19, 1, "RESERVED" },
	{ 0x1a, 2, "RESERVED" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 8, "GPE0_STS" }, // General Purpose Event 0 Status; ACPI pointer: GPE0_BLK
	{ 0x28, 8, "GPE0_EN" },  // General Purpose Event 0 Enables; ACPI pointer: GPE0_BLK+8
	{ 0x30, 4, "SMI_EN" },
	{ 0x34, 4, "SMI_STS" },
	{ 0x38, 2, "ALT_GP_SMI_EN" },
	{ 0x3a, 2, "ALT_GP_SMI_STS" },
	{ 0x3c, 1, "UPRWC" },   // USB Per-Port registers write control;
	{ 0x3d, 2, "RESERVED" },
	{ 0x3f, 1, "RESERVED" },
	{ 0x40, 2, "RESERVED" },
	{ 0x42, 1, "GPE_CNTL" },
	{ 0x43, 1, "RESERVED" },
	{ 0x44, 2, "DEVACT_STS" }, // Device Activity Status
	{ 0x46, 2, "RESERVED" },
	{ 0x48, 4, "RESERVED" },
	{ 0x4c, 4, "RESERVED" },
	{ 0x50, 1, "PM2_CNT (Mobile)" }, // PM2 Control (Mobile only); ACPI pointer: PM2a_CNT_BLK
	{ 0x51, 1, "RESERVED" },
	{ 0x52, 2, "RESERVED" },
	{ 0x54, 4, "C3_RES (Mobile)" },
	{ 0x58, 4, "C5_RES (Mobile)" },
	{ 0x5c, 4, "RESERVED" },
	/* Here start the TCO registers */
	{ 0x60, 2, "TCO_RLD" },
	{ 0x62, 1, "TCO_DAT_IN" },
	{ 0x63, 1, "TCO_DAT_OUT" },
	{ 0x64, 2, "TCO1_STS" },
	{ 0x66, 2, "TCO2_STS" },
	{ 0x68, 2, "TCO1_CNT" },
	{ 0x6a, 2, "TCO2_CNT" },
	{ 0x6c, 2, "TCO_MESSAGE" },
	{ 0x6e, 1, "TCO_WDCNT" },
	{ 0x6f, 1, "RESERVED" },
	{ 0x70, 1, "SW_IRQ_GEN" },
	{ 0x71, 1, "RESERVED" },
	{ 0x72, 2, "TCO_TMR" },
	{ 0x74, 4, "RESERVED" },
	{ 0x78, 4, "RESERVED" },
	{ 0x7c, 4, "RESERVED" },
};

static const io_register_t ich8_pm_registers[] = {
	{ 0x00, 2, "PM1_STS" },
	{ 0x02, 2, "PM1_EN" },
	{ 0x04, 4, "PM1_CNT" },
	{ 0x08, 4, "PM1_TMR" },
	{ 0x0c, 4, "RESERVED" },
	{ 0x10, 4, "PROC_CNT" },
#if DANGEROUS_REGISTERS
	/* These registers return 0 on read, but reading them may cause
	 * the system to enter Cx states, which might hang the system.
	 */
	{ 0x14, 1, "LV2 (Mobile)" },
	{ 0x15, 1, "LV3 (Mobile)" },
	{ 0x16, 1, "LV4 (Mobile)" },
	{ 0x17, 1, "LV5 (Mobile)" },
	{ 0x18, 1, "LV6 (Mobile)" },
#endif
	{ 0x19, 1, "RESERVED" },
	{ 0x1a, 2, "RESERVED" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 1, "PM2_CNT (Mobile)" },
	{ 0x21, 1, "RESERVED" },
	{ 0x22, 2, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "GPE0_STS" },
	{ 0x2C, 4, "GPE0_EN" },
	{ 0x30, 4, "SMI_EN" },
	{ 0x34, 4, "SMI_STS" },
	{ 0x38, 2, "ALT_GP_SMI_EN" },
	{ 0x3a, 2, "ALT_GP_SMI_STS" },
	{ 0x3c, 4, "RESERVED" },
	{ 0x40, 2, "RESERVED" },
	{ 0x42, 1, "GPE_CNTL" },
	{ 0x43, 1, "RESERVED" },
	{ 0x44, 2, "DEVACT_STS" },
	{ 0x46, 2, "RESERVED" },
	{ 0x48, 4, "RESERVED" },
	{ 0x4c, 4, "RESERVED" },
	{ 0x50, 1, "SS_CNT (Mobile)" },
	{ 0x51, 1, "RESERVED" },
	{ 0x52, 2, "RESERVED" },
	{ 0x54, 4, "C3_RES (Mobile)" },
	{ 0x58, 4, "C5_RES (Mobile)" },
	{ 0x5c, 4, "RESERVED" },
	/* Here start the TCO registers */
	{ 0x60, 2, "TCO_RLD" },
	{ 0x62, 1, "TCO_DAT_IN" },
	{ 0x63, 1, "TCO_DAT_OUT" },
	{ 0x64, 2, "TCO1_STS" },
	{ 0x66, 2, "TCO2_STS" },
	{ 0x68, 2, "TCO1_CNT" },
	{ 0x6a, 2, "TCO2_CNT" },
	{ 0x6c, 2, "TCO_MESSAGE" },
	{ 0x6e, 1, "TCO_WDCNT" },
	{ 0x6f, 1, "RESERVED" },
	{ 0x70, 1, "SW_IRQ_GEN" },
	{ 0x71, 1, "RESERVED" },
	{ 0x72, 2, "TCO_TMR" },
	{ 0x74, 4, "RESERVED" },
	{ 0x78, 4, "RESERVED" },
	{ 0x7c, 4, "RESERVED" },
};

static const io_register_t ich7_pm_registers[] = {
	{ 0x00, 2, "PM1_STS" },
	{ 0x02, 2, "PM1_EN" },
	{ 0x04, 4, "PM1_CNT" },
	{ 0x08, 4, "PM1_TMR" },
	{ 0x0c, 4, "RESERVED" },
	{ 0x10, 4, "PROC_CNT" },
#if DANGEROUS_REGISTERS
	/* These registers return 0 on read, but reading them may cause
	 * the system to enter C2/C3/C4 state, which might hang the system.
	 */
	{ 0x14, 1, "LV2 (Mobile/Ultra Mobile)" },
	{ 0x15, 1, "LV3 (Mobile/Ultra Mobile)" },
	{ 0x16, 1, "LV4 (Mobile/Ultra Mobile)" },
#endif
	{ 0x17, 1, "RESERVED" },
	{ 0x18, 4, "RESERVED" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 1, "PM2_CNT (Mobile/Ultra Mobile)" },
	{ 0x21, 1, "RESERVED" },
	{ 0x22, 2, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "GPE0_STS" },
	{ 0x2C, 4, "GPE0_EN" },
	{ 0x30, 4, "SMI_EN" },
	{ 0x34, 4, "SMI_STS" },
	{ 0x38, 2, "ALT_GP_SMI_EN" },
	{ 0x3a, 2, "ALT_GP_SMI_STS" },
	{ 0x3c, 4, "RESERVED" },
	{ 0x40, 2, "RESERVED" },
	{ 0x42, 1, "GPE_CNTL" },
	{ 0x43, 1, "RESERVED" },
	{ 0x44, 2, "DEVACT_STS" },
	{ 0x46, 2, "RESERVED" },
	{ 0x48, 4, "RESERVED" },
	{ 0x4c, 4, "RESERVED" },
	{ 0x50, 1, "SS_CNT (Mobile/Ultra Mobile)" },
	{ 0x51, 1, "RESERVED" },
	{ 0x52, 2, "RESERVED" },
	{ 0x54, 4, "C3_RES (Mobile/Ultra Mobile)" },
	{ 0x58, 4, "RESERVED" },
	{ 0x5c, 4, "RESERVED" },
	/* Here start the TCO registers */
	{ 0x60, 2, "TCO_RLD" },
	{ 0x62, 1, "TCO_DAT_IN" },
	{ 0x63, 1, "TCO_DAT_OUT" },
	{ 0x64, 2, "TCO1_STS" },
	{ 0x66, 2, "TCO2_STS" },
	{ 0x68, 2, "TCO1_CNT" },
	{ 0x6a, 2, "TCO2_CNT" },
	{ 0x6c, 2, "TCO_MESSAGE" },
	{ 0x6e, 1, "TCO_WDCNT" },
	{ 0x6f, 1, "RESERVED" },
	{ 0x70, 1, "SW_IRQ_GEN" },
	{ 0x71, 1, "RESERVED" },
	{ 0x72, 2, "TCO_TMR" },
	{ 0x74, 4, "RESERVED" },
	{ 0x78, 4, "RESERVED" },
	{ 0x7c, 4, "RESERVED" },
};

/*
 * INTEL I/O Controller Hub 6 Family
 * http://www.intel.com/assets/pdf/datasheet/301473.pdf
 */
static const io_register_t ich6_pm_registers[] = {
	/* 10.8.3 */
	{ 0x00, 2, "PM1_STS" },
	{ 0x02, 2, "PM1_EN" },
	{ 0x04, 4, "PM1_CNT" },
	{ 0x08, 4, "PM1_TMR" },
	{ 0x10, 4, "PROC_CNT" },
#if DANGEROUS_REGISTERS
	/* These registers return 0 on read, but reading them may cause
	 * the system to enter C2/C3/C4 state, which might hang the system.
	 */
	{ 0x14, 1, "LV2" },
	{ 0x15, 1, "LV3 (Mobile Only)" },
	{ 0x16, 1, "LV4 (Mobile Only)" },
#endif
	{ 0x20, 1, "PM2_CNT (Mobile Only)" },
	{ 0x28, 4, "GPE0_STS" },
	{ 0x2c, 4, "GPE0_EN" },
	{ 0x30, 4, "SMI_EN" },
	{ 0x34, 4, "SMI_STS" },
	{ 0x38, 2, "ALT_GP_SMI_EN" },
	{ 0x3a, 2, "ALT_GP_SMI_STS" },
	{ 0x44, 2, "DEVACT_STS" },
	{ 0x50, 1, "SS_CNT (Mobile Only)" },
	{ 0x54, 4, "C3_RES (Mobile Only)" },
};

static const io_register_t ich5_pm_registers[] = {
	{ 0x00, 2, "PM1_STS" },
	{ 0x02, 2, "PM1_EN" },
	{ 0x04, 4, "PM1_CNT" },
	{ 0x08, 4, "PM1_TMR" },
	{ 0x0c, 4, "RESERVED" },
	{ 0x10, 4, "PROC_CNT" },
	{ 0x14, 3, "RESERVED" },
	{ 0x17, 9, "RESERVED" },
	{ 0x20, 1, "RESERVED" },
	{ 0x28, 4, "GPE0_STS" },
	{ 0x2c, 4, "GPE0_EN" },
	{ 0x30, 4, "SMI_EN" },
	{ 0x34, 4, "SMI_STS" },
	{ 0x38, 2, "ALT_GP_SMI_EN" },
	{ 0x3a, 2, "ALT_GP_SMI_STS" },
	{ 0x3c, 4, "RESERVED" },
	{ 0x40, 1, "MON_SMI" },
	{ 0x42, 2, "RESERVED" },
	{ 0x44, 1, "DEVACT_STS" },
	{ 0x48, 1, "DEVTRAP_EN" },
	{ 0x50, 1, "RESERVED" },
	{ 0x51, 15, "RESERVED" },
	{ 0x60, 16, "RESERVED" },
};

static const io_register_t ich4_pm_registers[] = {
	{ 0x00, 2, "PM1_STS" },
	{ 0x02, 2, "PM1_EN" },
	{ 0x04, 4, "PM1_CNT" },
	{ 0x08, 4, "PM1_TMR" },
	{ 0x0c, 4, "RESERVED" },
	{ 0x10, 4, "PROC_CNT" },
#if DANGEROUS_REGISTERS
	/* These registers return 0 on read, but reading them may cause
	 * the system to enter C2/C3/C4 state, which might hang the system.
	 */
	{ 0x14, 1, "LV2 (Mobile)" },
	{ 0x15, 1, "LV3 (Mobile)" },
	{ 0x16, 1, "LV4 (Mobile)" },
#endif
	{ 0x17, 1, "RESERVED" },
	{ 0x18, 4, "RESERVED" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 1, "PM2_CNT (Mobile)" },
	{ 0x21, 1, "RESERVED" },
	{ 0x22, 2, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "GPE0_STS" },
	{ 0x2C, 4, "GPE0_EN" },
	{ 0x30, 4, "SMI_EN" },
	{ 0x34, 4, "SMI_STS" },
	{ 0x38, 2, "ALT_GP_SMI_EN" },
	{ 0x3a, 2, "ALT_GP_SMI_STS" },
	{ 0x3c, 4, "RESERVED" },
	{ 0x40, 2, "MON_SMI" },
	{ 0x42, 2, "RESERVED" },
	{ 0x44, 2, "DEVACT_STS" },
	{ 0x46, 2, "RESERVED" },
	{ 0x48, 4, "DEVTRAP_EN" },
	{ 0x4c, 2, "BUS_ADDR_TRACK" },
	{ 0x4e, 2, "BUS_CYC_TRACK" },
	{ 0x50, 1, "SS_CNT (Mobile/Ultra Mobile)" },
	{ 0x51, 1, "RESERVED" },
	{ 0x52, 2, "RESERVED" },
	{ 0x54, 4, "RESERVED" },
	{ 0x58, 4, "RESERVED" },
	{ 0x5c, 4, "RESERVED" },
	/* Here start the TCO registers */
	{ 0x60, 1, "TCO_RLD" },
	{ 0x61, 1, "TCO_TMR" },
	{ 0x62, 1, "TCO_DAT_IN" },
	{ 0x63, 1, "TCO_DAT_OUT" },
	{ 0x64, 2, "TCO1_STS" },
	{ 0x66, 2, "TCO2_STS" },
	{ 0x68, 2, "TCO1_CNT" },
	{ 0x6a, 2, "TCO2_CNT" },
	{ 0x6c, 2, "TCO_MESSAGE" },
	{ 0x6e, 1, "TCO_WDSTATUS" },
	{ 0x6f, 1, "RESERVED" },
	{ 0x70, 1, "SW_IRQ_GEN" },
	{ 0x71, 1, "RESERVED" },
	{ 0x72, 2, "RESERVED" },
	{ 0x74, 4, "RESERVED" },
	{ 0x78, 4, "RESERVED" },
	{ 0x7c, 4, "RESERVED" },
};

static const io_register_t ich2_pm_registers[] = {
	{ 0x00, 2, "PM1_STS" },
	{ 0x02, 2, "PM1_EN" },
	{ 0x04, 4, "PM1_CNT" },
	{ 0x08, 4, "PM1_TMR" },
	{ 0x0c, 4, "RESERVED" },
	{ 0x10, 4, "PROC_CNT" },
#if DANGEROUS_REGISTERS
	/* This register returns 0 on read, but reading it may cause
	 * the system to enter C2 state, which might hang the system.
	 */
	{ 0x14, 1, "LV2" },
	{ 0x15, 1, "RESERVED" },
	{ 0x16, 2, "RESERVED" },
#endif
	{ 0x18, 4, "RESERVED" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 4, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 2, "GPE0_STS" },
	{ 0x2a, 2, "GPE0_EN" },
	{ 0x2c, 2, "GPE1_STS" },
	{ 0x2e, 2, "GPE1_EN" },
	{ 0x30, 2, "SMI_EN" },
	{ 0x32, 2, "RESERVED" },
	{ 0x34, 2, "SMI_STS" },
	{ 0x36, 2, "RESERVED" },
	{ 0x38, 4, "RESERVED" },
	{ 0x3c, 4, "RESERVED" },
	{ 0x40, 2, "MON_SMI_STS" },
	{ 0x42, 2, "RESERVED" },
	{ 0x44, 2, "DEV_TRP_STS" },
	{ 0x46, 2, "RESERVED" },
	{ 0x48, 2, "TRP_EN" },
	{ 0x4A, 2, "RESERVED" },
	{ 0x4c, 2, "BUS_ADDR_TRACK" },
	{ 0x4e, 1, "BUS_CYC_TRACK" },
	{ 0x4f, 1, "RESERVED" },
	{ 0x50, 4, "RESERVED" },
	{ 0x54, 4, "RESERVED" },
	{ 0x58, 4, "RESERVED" },
	{ 0x5c, 4, "RESERVED" },
	/* Here start the TCO registers */
	{ 0x60, 1, "TCO_RLD" },
	{ 0x61, 1, "TCO_TMR" },
	{ 0x62, 1, "TCO_DAT_IN" },
	{ 0x63, 1, "TCO_DAT_OUT" },
	{ 0x64, 2, "TCO1_STS" },
	{ 0x66, 2, "TCO2_STS" },
	{ 0x68, 2, "TCO1_CNT" },
	{ 0x6a, 2, "TCO2_CNT" },
	{ 0x6c, 1, "TCO_MESSAGE1" },
	{ 0x6d, 1, "TCO_MESSAGE2" },
	{ 0x6e, 1, "TCO_WDSTATUS" },
	{ 0x6f, 1, "RESERVED" },
	{ 0x70, 1, "SW_IRQ_GEN" },
	{ 0x71, 1, "RESERVED" },
	{ 0x72, 2, "RESERVED" },
	{ 0x74, 4, "RESERVED" },
	{ 0x78, 4, "RESERVED" },
	{ 0x7c, 4, "RESERVED" },
};

static const io_register_t ich0_pm_registers[] = {
	{ 0x00, 2, "PM1_STS" },
	{ 0x02, 2, "PM1_EN" },
	{ 0x04, 4, "PM1_CNT" },
	{ 0x08, 4, "PM1_TMR" },
	{ 0x0c, 4, "RESERVED" },
	{ 0x10, 4, "PROC_CNT" },
#if DANGEROUS_REGISTERS
	/* This register returns 0 on read, but reading it may cause
	 * the system to enter C2 state, which might hang the system.
	 */
	{ 0x14, 1, "LV2" },
	{ 0x15, 1, "RESERVED" },
	{ 0x16, 2, "RESERVED" },
#endif
	{ 0x18, 4, "RESERVED" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 4, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "GPE0_STS" },
	{ 0x2C, 4, "GPE0_EN" },
	{ 0x30, 2, "SMI_EN" },
	{ 0x32, 2, "RESERVED" },
	{ 0x34, 2, "SMI_STS" },
	{ 0x36, 2, "RESERVED" },
	{ 0x38, 4, "RESERVED" },
	{ 0x3c, 4, "RESERVED" },
	{ 0x40, 2, "IOMON_STS_EN" },
	{ 0x42, 2, "RESERVED" },
	{ 0x44, 2, "DEVACT_STS" },
	{ 0x46, 2, "RESERVED" },
	{ 0x48, 4, "RESERVED" },
	{ 0x4c, 2, "BUS_ADDR_TRACK" },
	{ 0x4e, 1, "BUS_CYC_TRACK" },
	{ 0x4f, 1, "RESERVED" },
	{ 0x50, 4, "RESERVED" },
	{ 0x54, 4, "RESERVED" },
	{ 0x58, 4, "RESERVED" },
	{ 0x5c, 4, "RESERVED" },
	/* Here start the TCO registers */
	{ 0x60, 1, "TCO_RLD" },
	{ 0x61, 1, "TCO_TMR" },
	{ 0x62, 1, "TCO_DAT_IN" },
	{ 0x63, 1, "TCO_DAT_OUT" },
	{ 0x64, 2, "TCO1_STS" },
	{ 0x66, 2, "TCO2_STS" },
	{ 0x68, 2, "TCO1_CNT" },
	{ 0x6a, 2, "TCO2_CNT" },
	{ 0x6c, 1, "TCO_MESSAGE1" },
	{ 0x6d, 1, "TCO_MESSAGE2" },
	{ 0x6e, 1, "TCO_WDSTATUS" },
	{ 0x6f, 1, "RESERVED" },
	{ 0x70, 4, "RESERVED" },
	{ 0x74, 4, "RESERVED" },
	{ 0x78, 4, "RESERVED" },
	{ 0x7c, 4, "RESERVED" },
};

static const io_register_t i82371xx_pm_registers[] = {
	{ 0x00, 2, "PMSTS" },
	{ 0x02, 2, "PMEN" },
	{ 0x04, 2, "PMCNTRL" },
	{ 0x06, 2, "RESERVED" },
	{ 0x08, 1, "PMTMR" },
	{ 0x09, 1, "RESERVED" },
	{ 0x0A, 1, "RESERVED" },
	{ 0x0B, 1, "RESERVED" },
	{ 0x0C, 2, "GPSTS" },
	{ 0x0E, 2, "GPEN" },
	{ 0x10, 4, "PCNTRL" },
#if DANGEROUS_REGISTERS
	/*
	 * This register returns 0 on read, but reading it may cause
	 * the system to enter C2 state, which might hang the system.
	 */
	{ 0x14, 1, "PLVL2" },
	{ 0x15, 1, "PLVL3" },
	{ 0x16, 2, "RESERVED" },
#endif
	{ 0x18, 2, "GLBSTS" },
	{ 0x1A, 2, "RESERVED" },
	{ 0x1c, 4, "DEVSTS" },
	{ 0x20, 2, "GLBEN" },
	{ 0x22, 1, "RESERVED" },
	{ 0x23, 1, "RESERVED" },
	{ 0x24, 1, "RESERVED" },
	{ 0x25, 1, "RESERVED" },
	{ 0x26, 1, "RESERVED" },
	{ 0x27, 1, "RESERVED" },
	{ 0x28, 4, "GLBCTL" },
	{ 0x2C, 4, "DEVCTL" },
	/* The registers 0x30-0x33 and 0x34-0x37 allow byte-wise reads only. */
	{ 0x30, 1, "GPIREG 0" },
	{ 0x31, 1, "GPIREG 1" },
	{ 0x32, 1, "GPIREG 2" },
	{ 0x33, 1, "GPIREG 3" },
	{ 0x34, 1, "GPOREG 0" },
	{ 0x35, 1, "GPOREG 1" },
	{ 0x36, 1, "GPOREG 2" },
	{ 0x37, 1, "GPOREG 3" },
};

static const io_register_t i63xx_pm_registers[] = {
	{ 0x00, 2, "PM1_STS" },
	{ 0x02, 2, "PM1_EN" },
	{ 0x04, 4, "PM1_CNT" },
	{ 0x08, 4, "PM1_TMR" },
	{ 0x0c, 4, "RESERVED" },
	{ 0x10, 4, "PROC_CNT" },
#if DANGEROUS_REGISTERS
	/* This register returns 0 on read, but reading it may cause
	 * the system to enter C2 state, which might hang the system.
	 */
	{ 0x14, 1, "LV2" },
	{ 0x15, 1, "RESERVED" },
	{ 0x16, 2, "RESERVED" },
#endif
	{ 0x18, 4, "RESERVED" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 4, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "GPE0_STS" },
	{ 0x2C, 4, "GPE0_EN" },
	{ 0x30, 4, "SMI_EN" },
	{ 0x34, 4, "SMI_STS" },
	{ 0x38, 2, "ALT_GP_SMI_EN" },
	{ 0x3a, 2, "ALT_GP_SMI_STS" },
	{ 0x3c, 4, "RESERVED" },
	{ 0x40, 4, "RESERVED" },
	{ 0x44, 2, "DEVACT_STS" },
	{ 0x46, 2, "RESERVED" },
	{ 0x48, 4, "RESERVED" },
	{ 0x4c, 4, "RESERVED" },
	{ 0x50, 4, "RESERVED" },
	{ 0x54, 4, "C3_RES" },
	{ 0x58, 4, "RESERVED" },
	{ 0x5c, 4, "RESERVED" },
	{ 0x60, 1, "RESERVED" },
	{ 0x64, 4, "RESERVED" },
	{ 0x68, 4, "RESERVED" },
	{ 0x6c, 4, "RESERVED" },
	{ 0x70, 4, "RESERVED" },
	{ 0x74, 4, "RESERVED" },
	{ 0x78, 4, "RESERVED" },
	{ 0x7c, 4, "RESERVED" },
};

int print_pmbase(struct pci_dev *sb, struct pci_access *pacc)
{
	int i, size;
	uint16_t pmbase;
	const io_register_t *pm_registers;
	struct pci_dev *acpi;

	printf("\n============= PMBASE ============\n\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_Z68:
	case PCI_DEVICE_ID_INTEL_P67:
	case PCI_DEVICE_ID_INTEL_UM67:
	case PCI_DEVICE_ID_INTEL_HM65:
	case PCI_DEVICE_ID_INTEL_H67:
	case PCI_DEVICE_ID_INTEL_HM67:
	case PCI_DEVICE_ID_INTEL_Q65:
	case PCI_DEVICE_ID_INTEL_QS67:
	case PCI_DEVICE_ID_INTEL_Q67:
	case PCI_DEVICE_ID_INTEL_QM67:
	case PCI_DEVICE_ID_INTEL_B65:
	case PCI_DEVICE_ID_INTEL_C202:
	case PCI_DEVICE_ID_INTEL_C204:
	case PCI_DEVICE_ID_INTEL_C206:
	case PCI_DEVICE_ID_INTEL_H61:
	case PCI_DEVICE_ID_INTEL_Z77:
	case PCI_DEVICE_ID_INTEL_Z75:
	case PCI_DEVICE_ID_INTEL_Q77:
	case PCI_DEVICE_ID_INTEL_Q75:
	case PCI_DEVICE_ID_INTEL_B75:
	case PCI_DEVICE_ID_INTEL_H77:
	case PCI_DEVICE_ID_INTEL_C216:
	case PCI_DEVICE_ID_INTEL_QM77:
	case PCI_DEVICE_ID_INTEL_QS77:
	case PCI_DEVICE_ID_INTEL_HM77:
	case PCI_DEVICE_ID_INTEL_UM77:
	case PCI_DEVICE_ID_INTEL_HM76:
	case PCI_DEVICE_ID_INTEL_HM75:
	case PCI_DEVICE_ID_INTEL_HM70:
		pmbase = pci_read_word(sb, 0x40) & 0xff80;
		pm_registers = pch_pm_registers;
		size = ARRAY_SIZE(pch_pm_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH10R:
		pmbase = pci_read_word(sb, 0x40) & 0xff80;
		pm_registers = ich10_pm_registers;
		size = ARRAY_SIZE(ich10_pm_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH7:
	case PCI_DEVICE_ID_INTEL_ICH7M:
	case PCI_DEVICE_ID_INTEL_ICH7DH:
	case PCI_DEVICE_ID_INTEL_ICH7MDH:
	case PCI_DEVICE_ID_INTEL_NM10:
		pmbase = pci_read_word(sb, 0x40) & 0xfffc;
		pm_registers = ich7_pm_registers;
		size = ARRAY_SIZE(ich7_pm_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH9DH:
	case PCI_DEVICE_ID_INTEL_ICH9DO:
	case PCI_DEVICE_ID_INTEL_ICH9R:
	case PCI_DEVICE_ID_INTEL_ICH9:
	case PCI_DEVICE_ID_INTEL_ICH9M:
	case PCI_DEVICE_ID_INTEL_ICH9ME:
		pmbase = pci_read_word(sb, 0x40) & 0xfffc;
		pm_registers = ich9_pm_registers;
		size = ARRAY_SIZE(ich9_pm_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH8:
	case PCI_DEVICE_ID_INTEL_ICH8M:
		pmbase = pci_read_word(sb, 0x40) & 0xfffc;
		pm_registers = ich8_pm_registers;
		size = ARRAY_SIZE(ich8_pm_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH6:
		pmbase = pci_read_word(sb, 0x40) & 0xfffc;
		pm_registers = ich6_pm_registers;
		size = ARRAY_SIZE(ich6_pm_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH5:
		pmbase = pci_read_word(sb, 0x40) & 0xfffc;
		pm_registers = ich5_pm_registers;
		size = ARRAY_SIZE(ich5_pm_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH4:
		pmbase = pci_read_word(sb, 0x40) & 0xfffc;
		pm_registers = ich4_pm_registers;
		size = ARRAY_SIZE(ich4_pm_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH2:
		pmbase = pci_read_word(sb, 0x40) & 0xfffc;
		pm_registers = ich2_pm_registers;
		size = ARRAY_SIZE(ich2_pm_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH0:
		pmbase = pci_read_word(sb, 0x40) & 0xfffc;
		pm_registers = ich0_pm_registers;
		size = ARRAY_SIZE(ich0_pm_registers);
		break;
	case PCI_DEVICE_ID_INTEL_82371XX:
		acpi = pci_get_dev(pacc, sb->domain, sb->bus, sb->dev, 3);
		if (!acpi) {
			printf("Southbridge function 3 not found.\n");
			return 1;
		}
		pmbase = pci_read_word(acpi, 0x40) & 0xfffc;
		pm_registers = i82371xx_pm_registers;
		size = ARRAY_SIZE(i82371xx_pm_registers);
		break;

	case PCI_DEVICE_ID_INTEL_I63XX:
		pmbase = pci_read_word(sb, 0x40) & 0xfffc;
		pm_registers = i63xx_pm_registers;
		size = ARRAY_SIZE(i63xx_pm_registers);
		break;

	case PCI_DEVICE_ID_INTEL_MOBILE_5:
		pmbase = pci_read_word(sb, 0x40) & 0xfffc;
		pm_registers = i63xx_pm_registers;
		size = ARRAY_SIZE(i63xx_pm_registers);
		break;

	case 0x1234: // Dummy for non-existent functionality
		printf("This southbridge does not have PMBASE.\n");
		return 1;
	default:
		printf("Error: Dumping PMBASE on this southbridge is not (yet) supported.\n");
		return 1;
	}

	printf("PMBASE = 0x%04x (IO)\n\n", pmbase);

	for (i = 0; i < size; i++) {
		switch (pm_registers[i].size) {
		case 8:
			printf("pmbase+0x%04x: 0x%08x (%s)\n"
			       "               0x%08x\n",
				pm_registers[i].addr,
				inl(pmbase+pm_registers[i].addr),
				pm_registers[i].name,
				inl(pmbase+pm_registers[i].addr+4));
			break;
		case 4:
			printf("pmbase+0x%04x: 0x%08x (%s)\n",
				pm_registers[i].addr,
				inl(pmbase+pm_registers[i].addr),
				pm_registers[i].name);
			break;
		case 2:
			printf("pmbase+0x%04x: 0x%04x     (%s)\n",
				pm_registers[i].addr,
				inw(pmbase+pm_registers[i].addr),
				pm_registers[i].name);
			break;
		case 1:
			printf("pmbase+0x%04x: 0x%02x       (%s)\n",
				pm_registers[i].addr,
				inb(pmbase+pm_registers[i].addr),
				pm_registers[i].name);
			break;
		}
	}

	return 0;
}
