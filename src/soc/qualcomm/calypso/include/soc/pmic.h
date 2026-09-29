/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_CALYPSO_PMIC_H__
#define _SOC_QUALCOMM_CALYPSO_PMIC_H__

#include <types.h>

#define PMIC_SLAVE_ID				0x00
#define SDAM05_BASE_ADDR			0x7400
#define MEM_OFFSET_START			0x40

#define PON_EVENT_LOG_AREA_SIZE			(127 - 11 + 1)
#define PON_EVENT_TOTAL_LOG_AREA_SIZE		(PON_EVENT_LOG_AREA_SIZE * 2)

#define PM_PON_SDAM_COUNT_ADDR			(SDAM05_BASE_ADDR + MEM_OFFSET_START + 5)
#define PM_PON_ENQUEUE_ADDR			(SDAM05_BASE_ADDR + MEM_OFFSET_START + 6)
#define PM_PON_ENQUEUE_SDAM_NUM			(SDAM05_BASE_ADDR + MEM_OFFSET_START + 7)
#define PM_PON_LOGGING_AREA_START		(SDAM05_BASE_ADDR + MEM_OFFSET_START + 11)
#define PM_PON_PUSH_PTR_INDEX(x)		(x - (MEM_OFFSET_START + 11))

#define PM_PON_EVENT_PON_TRIGGER		0x1
#define PM_PON_EVENT_RESET_TYPE			0x7
#define PM_PON_EVENT_FUNDAMENTAL_RESET		0xC
#define BEGIN_PON				0x0D
#define PON_EVENT_PARSE_LIMIT			2

/*
 * PM_PON_EVENT_PON_TRIGGER data format: (SID << 12) | (PID << 4) | IRQ
 *   Bits [15:12] : SID (Slave ID = 0x1)
 *   Bits [11:4]  : PID (Peripheral ID = 0x8C..0x8F)
 *   Bits [3:0]   : IRQ (0x0 = CBLPWR)
 *
 * Depending on PMIC PBS/PSI firmware revision and charger/PON sub-peripheral
 * routing, AC cable power-on (CBLPWR) is logged with PID 0x8C (0x18C0) or
 * PID 0x8F (0x18F0). Mask out bits [5:4] (the lower 2 bits of PID) so both
 * 0x18C0 and 0x18F0 match PON_CBLPWR_RSN.
 */
#define PON_CBLPWR_RSN			0x18C0
#define PON_CBLPWR_RSN_MASK		0xFFCF
#define PON_RAW_XVDD_RB_MASK		0x8000

bool is_pon_on_ac(void);
bool is_reset_type_warm(void);

#endif  /* _SOC_QUALCOMM_CALYPSO_PMIC_H__ */
