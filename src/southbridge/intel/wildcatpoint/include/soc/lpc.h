/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_LPC_H_
#define _BROADWELL_LPC_H_

#include <southbridge/intel/common/lpc_def.h> /* IWYU pragma: export */

/* PCI Configuration Space (D31:F0): LPC */
#define SERIRQ_CNTL		0x64
#define PMBASE			0x40
#define ACPI_CNTL		0x44
#define  ACPI_EN		(1 << 7)
#define  SCI_IRQ_SEL		(7 << 0)
#define  SCIS_IRQ9		0
#define  SCIS_IRQ10		1
#define  SCIS_IRQ11		2
#define  SCIS_IRQ20		4
#define  SCIS_IRQ21		5
#define  SCIS_IRQ22		6
#define  SCIS_IRQ23		7
#define GPIO_CNTL		0x4C /* LPC GPIO Control Register */
#define  GPIO_EN		(1 << 4)
#define GPIO_ROUT		0xb8

#define RCBA			0xf0 /* Root Complex Register Block */

/* Power Management */

#define GEN_PMCON_1		0xa0
#define  SMI_LOCK		(1 << 4)
#define GEN_PMCON_2		0xa2
#define  SYSTEM_RESET_STS	(1 << 4)
#define  THERMTRIP_STS		(1 << 3)
#define  SYSPWR_FLR		(1 << 1)
#define  PWROK_FLR		(1 << 0)
#define GEN_PMCON_3		0xa4
#define  SUS_PWR_FLR		(1 << 14)
#define  GEN_RST_STS		(1 << 9)
#define  RTC_BATTERY_DEAD	(1 << 2)
#define  PWR_FLR		(1 << 1)
#define  SLEEP_AFTER_POWER_FAIL	(1 << 0)
#define GEN_PMCON_LOCK		0xa6
#define  SLP_STR_POL_LOCK	(1 << 2)
#define  ACPI_BASE_LOCK		(1 << 1)
#define PMIR			0xac
#define  PMIR_CF9LOCK		(1 << 31)
#define  PMIR_CF9GR		(1 << 20)

#define LPC_IBDF		0x6C		/* I/O APIC bus/dev/fn */
#define LPC_HnBDF(n)		(0x70 + n * 2)	/* HPET n bus/dev/fn */

#endif
