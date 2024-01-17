/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _INTELPCH_TCO_H_
#define _INTELPCH_TCO_H_

/* TCO registers and fields live behind TCOBASE I/O bar in SMBus device. */
#define TCO_RLD				0x00
#define TCO_DAT_IN			0x02
#define TCO_DAT_OUT			0x03
#define TCO1_STS			0x04
#define  TCO1_STS_TCO_SLVSEL		(1 << 13)
#define  TCO1_STS_CPUSERR		(1 << 12)
#define  TCO1_STS_CPUSMI		(1 << 10)
#define  TCO1_STS_CPUSCI		(1 << 9)
#define  TCO1_STS_BIOSWR		(1 << 8)
#define  TCO1_STS_NEWCENTURY		(1 << 7)
#define  TCO1_STS_TIMEOUT		(1 << 3)
#define  TCO1_STS_TCO_INT		(1 << 2)
#define  TCO1_STS_OS_TCO_SMI		(1 << 1)
#define  TCO1_STS_NMI2SMI		(1 << 0)
#define TCO2_STS			0x06
#define  TCO2_STS_SMLINK_SLAVE_SMI	(1 << 2)
#define  TCO2_STS_SECOND_TO		(1 << 1)
#define  TCO2_INTRD_DET			(1 << 0)
#define TCO1_CNT			0x08
#define  TCO1_LOCK			(1 << 12)
#define  TCO1_TMR_HLT			(1 << 11)
#define  TCO1_NMI2SMI_EN		(1 << 9)
#define  TCO1_NMI_NOW			(1 << 8)
#define TCO2_CNT			0x0A
#define  TCO2_OS_POLICY_MASK		(3 << 4)
#define  TCO2_OS_POLICY_SHUTDOWN	(1 << 4)
#define  TCO2_OS_POLICY_DONOT_LOAD	(1 << 5)
#define  TCO2_SMB_ALERT_DISABLE		(1 << 3)
#define  TCO2_INTRD_SEL_MASK		(3 << 1)
#define  TCO2_INTRD_SEL_SMI		(1 << 2)
#define  TCO2_INTRD_SEL_INT		(1 << 1)
#define TCO_MESSAGE1			0x0C
#define TCO_MESSAGE2			0x0D
#define TCO_WDSTATUS			0x0E
#define TCO_LEGACY_ELIM			0x10
#define  TCO_IRQ12_CAUSE		(1 << 1)
#define  TCO_IRQ1_CAUSE			(1 << 0)
#define TCO_TMR				0x12
#define  TCO_TMR_MASK			0x3FF

#endif
