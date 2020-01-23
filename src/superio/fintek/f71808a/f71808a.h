/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#ifndef SUPERIO_FINTEK_F71808A_H
#define SUPERIO_FINTEK_F71808A_H

/* Logical Device Numbers (LDN). */
#define F71808A_SP1	0x01	/* UART1 */
#define F71808A_HWM	0x04	/* Hardware monitor */
#define F71808A_KBC	0x05	/* PS/2 keyboard and mouse */
#define F71808A_GPIO	0x06	/* General Purpose I/O (GPIO) */
#define F71808A_WDT	0x07	/* WDT */
#define F71808A_CIR	0x08	/* CIR */
#define F71808A_PME	0x0a	/* Power Management Events (PME) and ACPI */

#endif /* SUPERIO_FINTEK_F71808A_H */
