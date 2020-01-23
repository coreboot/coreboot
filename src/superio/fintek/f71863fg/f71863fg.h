/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#ifndef SUPERIO_FINTEK_F71863FG_H
#define SUPERIO_FINTEK_F71863FG_H

/* Logical Device Numbers (LDN). */
#define F71863FG_FDC	0x00	/* Floppy */
#define F71863FG_SP1	0x01	/* UART1 */
#define F71863FG_SP2	0x02	/* UART2 */
#define F71863FG_PP	0x03	/* Parallel port */
#define F71863FG_HWM	0x04	/* Hardware monitor */
#define F71863FG_KBC	0x05	/* PS/2 keyboard and mouse */
#define F71863FG_GPIO	0x06	/* General Purpose I/O (GPIO) */
#define F71863FG_VID	0x07	/* VID */
#define F71863FG_SPI	0x08	/* SPI */
#define F71863FG_PME	0x0a	/* Power Management Events (PME) and ACPI */

#endif /* SUPERIO_FINTEK_F71863FG_H */
