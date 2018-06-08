/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#ifndef _BROADWELL_LPC_H_
#define _BROADWELL_LPC_H_

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
#define GPIOBASE		0x48
#define BIOS_CNTL		0xdc
#define GPIO_BASE		0x48 /* LPC GPIO Base Address Register */
#define GPIO_CNTL		0x4C /* LPC GPIO Control Register */
#define  GPIO_EN		(1 << 4)
#define GPIO_ROUT		0xb8

#define PIRQA_ROUT		0x60
#define PIRQB_ROUT		0x61
#define PIRQC_ROUT		0x62
#define PIRQD_ROUT		0x63
#define PIRQE_ROUT		0x68
#define PIRQF_ROUT		0x69
#define PIRQG_ROUT		0x6A
#define PIRQH_ROUT		0x6B

#define LPC_IO_DEC		0x80 /* IO Decode Ranges Register */
#define LPC_EN			0x82 /* LPC IF Enables Register */
#define  CNF2_LPC_EN		(1 << 13) /* 0x4e/0x4f */
#define  CNF1_LPC_EN		(1 << 12) /* 0x2e/0x2f */
#define  MC_LPC_EN		(1 << 11) /* 0x62/0x66 */
#define  KBC_LPC_EN		(1 << 10) /* 0x60/0x64 */
#define  GAMEH_LPC_EN		(1 << 9)  /* 0x208/0x20f */
#define  GAMEL_LPC_EN		(1 << 8)  /* 0x200/0x207 */
#define  FDD_LPC_EN		(1 << 3)  /* LPC_IO_DEC[12] */
#define  LPT_LPC_EN		(1 << 2)  /* LPC_IO_DEC[9:8] */
#define  COMB_LPC_EN		(1 << 1)  /* LPC_IO_DEC[6:4] */
#define  COMA_LPC_EN		(1 << 0)  /* LPC_IO_DEC[2:0] */
#define LPC_GEN1_DEC		0x84 /* LPC IF Generic Decode Range 1 */
#define LPC_GEN2_DEC		0x88 /* LPC IF Generic Decode Range 2 */
#define LPC_GEN3_DEC		0x8c /* LPC IF Generic Decode Range 3 */
#define LPC_GEN4_DEC		0x90 /* LPC IF Generic Decode Range 4 */
#define LGMR			0x98 /* LPC Generic Memory Range */
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
