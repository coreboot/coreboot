/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
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

#ifndef __AMD_PCI_INT_DEFS_H__
#define __AMD_PCI_INT_DEFS_H__

/*
 * PIRQ and device routing - these define the index into the
 * FCH PCI_INTR 0xC00/0xC01 interrupt routing table.
 */

#define PIRQ_NC		0x1f	/* Not Used */
#define PIRQ_A		0x00	/* INT A */
#define PIRQ_B		0x01	/* INT B */
#define PIRQ_C		0x02	/* INT C */
#define PIRQ_D		0x03	/* INT D */
#define PIRQ_E		0x04	/* INT E */
#define PIRQ_F		0x05	/* INT F */
#define PIRQ_G		0x06	/* INT G */
#define PIRQ_H		0x07	/* INT H */
#define PIRQ_MISC	0x08	/* Miscellaneous IRQ Settings - See FCH Spec */
#define PIRQ_MISC0	0x09	/* Miscellaneous0 IRQ Settings */
#define PIRQ_MISC1	0x0a	/* Miscellaneous1 IRQ Settings */
#define PIRQ_MISC2	0x0b	/* Miscellaneous2 IRQ Settings */
#define PIRQ_SIRQA	0x0c	/* Serial IRQ INTA */
#define PIRQ_SIRQB	0x0d	/* Serial IRQ INTB */
#define PIRQ_SIRQC	0x0e	/* Serial IRQ INTC */
#define PIRQ_SIRQD	0x0f	/* Serial IRQ INTD */
#define PIRQ_SCI	0x10	/* SCI IRQ */
#define PIRQ_SMBUS	0x11	/* SMBUS	14h.0 */
#define PIRQ_ASF	0x12	/* ASF */
#define PIRQ_HDA	0x13	/* HDA		14h.2 */
#define PIRQ_FC		0x14	/* FC */
#define PIRQ_PMON	0x16	/* Performance Monitor */
#define PIRQ_SD		0x17	/* SD */
#define PIRQ_SDIO	0x1a	/* SDIO */
#define PIRQ_EHCI	0x30	/* USB EHCI	12h.0 */
#define PIRQ_XHCI	0x34	/* USB XHCI	10h.0 */
#define PIRQ_SATA	0x41	/* SATA		11h.0 */
#define PIRQ_GPIO	0x62	/* GPIO Controller Interrupt */
#define PIRQ_I2C0	0x70
#define PIRQ_I2C1	0x71
#define PIRQ_I2C2	0x72
#define PIRQ_I2C3	0x73
#define PIRQ_UART0	0x74
#define PIRQ_UART1	0x75

#endif /* __AMD_PCI_INT_DEFS_H__ */
