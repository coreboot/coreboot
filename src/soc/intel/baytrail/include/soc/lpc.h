/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BAYTRAIL_LPC_H_
#define _BAYTRAIL_LPC_H_

/* PCI config registers in LPC bridge. */
#define REVID		0x08
#define ABASE		0x40
#define PBASE		0x44
#define GBASE		0x48
#define IOBASE		0x4c
#define IBASE		0x50
#define SBASE		0x54
#define MPBASE		0x58
#define PUBASE		0x5c
#define UART_CONT	0x80
#define RCBA		0xf0

#define RID_A_STEPPING_START 1
#define RID_B_STEPPING_START 5
#define RID_C_STEPPING_START 0xe
#define RID_D_STEPPING_START 0x11

enum baytrail_stepping {
	STEP_A0,
	STEP_A1,
	STEP_B0,
	STEP_B1,
	STEP_B2,
	STEP_B3,
	STEP_C0,
	STEP_D0,
};

/* Registers behind the RCBA_BASE_ADDRESS bar. */
#define GCS		0x00
# define BILD		(1 << 0)

#endif /* _BAYTRAIL_LPC_H_ */
