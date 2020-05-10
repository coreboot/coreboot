/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Datasheet:
 *  - Name: F81865F/F-I
 */
#ifndef SUPERIO_FINTEK_F81865_H
#define SUPERIO_FINTEK_F81865_H

/* Logical Device Numbers (LDN). */
#define F81865F_FDC	0x00	/* Floppy */
#define F81865F_SP1	0x10	/* UART1 */
#define F81865F_SP2	0x11	/* UART2 */
#define F81865F_PP	0x03	/* Parallel Port */
#define F81865F_HWM	0x04	/* Hardware Monitor */
#define F81865F_KBC	0x05	/* Keyboard/Mouse */
#define F81865F_GPIO	0x06	/* General Purpose I/O (GPIO) */
#define F81865F_PME	0x0a	/* Power Management Events (PME) */

#endif /* SUPERIO_FINTEK_F81865_H */
