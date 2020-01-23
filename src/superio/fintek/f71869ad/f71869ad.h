/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#ifndef SUPERIO_FINTEK_F71869AD_H
#define SUPERIO_FINTEK_F71869AD_H

/* Logical Device Numbers (LDN). */
#define F71869AD_FDC    0x00	/* Floppy */
#define F71869AD_SP1    0x01	/* UART1 */
#define F71869AD_SP2    0x02	/* UART2 */
#define F71869AD_PP     0x03	/* Parallel port */
#define F71869AD_HWM    0x04	/* Hardware monitor */
#define F71869AD_KBC    0x05	/* PS/2 keyboard and mouse */
#define F71869AD_GPIO   0x06	/* General Purpose I/O (GPIO) */
#define F71869AD_WDT    0x07	/* WDT */
#define F71869AD_CIR    0x08	/* CIR */
#define F71869AD_PME    0x0a	/* Power Management Events (PME) and ACPI */

#endif /* SUPERIO_FINTEK_F71869AD_H */
