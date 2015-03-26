/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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
 * Foundation, Inc.
 */

#ifndef _BAYTRAIL_LPC_H_
#define _BAYTRAIL_LPC_H_

#define FADT_SOC_LPC_DEV 0, PCI_DEVFN(0x1f,0)

/* PCI config registers in LPC bridge. */
#define REVID		0x08
#define ABASE		0x40 /* IO BAR */
#define PBASE		0x44 /* MEM BAR */
#define GBASE		0x48 /* IO BAR */
#define IOBASE		0x4c /* MEM BAR */
#define IBASE		0x50 /* MEM BAR */
#define SBASE		0x54 /* MEM BAR */
#define MPBASE		0x58 /* MEM BAR */
#define PUBASE		0x5c
#define  SET_BAR_ENABLE	0x02
#define UART_CONT	0x80
#define RCBA		0xf0
#define  RCBA_ENABLE 0x01

#define ILB_ACTL	0
#define ILB_MC			0x4
#define ILB_PIRQA_ROUT	0x8
#define ILB_PIRQB_ROUT	0x9
#define ILB_PIRQC_ROUT	0xA
#define ILB_PIRQD_ROUT	0xB
#define ILB_PIRQE_ROUT	0xC
#define ILB_PIRQF_ROUT	0xD
#define ILB_PIRQG_ROUT	0xE
#define ILB_PIRQH_ROUT	0xF
#define ILB_SERIRQ_CNTL	0x10
#define  SCNT_CONTINUOUS_MODE	(1 << 7)
#define  SCNT_QUIET_MODE		0
#define ILB_IR00 0x20
#define ILB_IR01 0x22
#define ILB_IR02 0x24
#define ILB_IR03 0x26
#define ILB_IR04 0x28
#define ILB_IR05 0x2A
#define ILB_IR06 0x2C
#define ILB_IR07 0x2E
#define ILB_IR08 0x30
#define ILB_IR09 0x32
#define ILB_IR10 0x34
#define ILB_IR11 0x36
#define ILB_IR12 0x38
#define ILB_IR13 0x3A
#define ILB_IR14 0x3C
#define ILB_IR15 0x3E
#define ILB_IR16 0x40
#define ILB_IR17 0x42
#define ILB_IR18 0x44
#define ILB_IR19 0x46
#define ILB_IR20 0x48
#define ILB_IR21 0x4A
#define ILB_IR22 0x4C
#define ILB_IR23 0x4E
#define ILB_IR24 0x50
#define ILB_IR25 0x52
#define ILB_IR26 0x54
#define ILB_IR27 0x56
#define ILB_IR28 0x58
#define ILB_IR29 0x5A
#define ILB_IR30 0x5C
#define ILB_IR31 0x5E
#define ILB_OIC	0x60
#define  SIRQEN (1 << 12)
#define  AEN    (1 << 8)

#define RID_A_STEPPING_START 1
#define RID_B_STEPPING_START 5
#define RID_C_STEPPING_START 0xe
enum baytrail_stepping {
	STEP_A0,
	STEP_A1,
	STEP_B0,
	STEP_B1,
	STEP_B2,
	STEP_B3,
	STEP_C0,
};

/* Registers behind the RCBA_BASE_ADDRESS bar. */
#define GCS		0x00
# define BILD		(1 << 0)

/* Default IO range claimed by the LPC devices. The upper bound is exclusive. */
#define LPC_DEFAULT_IO_RANGE_LOWER 0
#define LPC_DEFAULT_IO_RANGE_UPPER 0x1000
#define IO_APIC_RANGE_SIZE	0x1000

#endif /* _BAYTRAIL_LPC_H_ */
