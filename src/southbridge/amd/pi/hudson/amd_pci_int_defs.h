/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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

#ifndef AMD_PCI_INT_DEFS_H
#define AMD_PCI_INT_DEFS_H

/*
 * PIRQ and device routing - these define the index
 * into the FCH PCI_INTR 0xC00/0xC01 interrupt
 * routing table
 */

#define PIRQ_NC		0x1F	/* Not Used */
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
#define PIRQ_MISC1	0x0A	/* Miscellaneous1 IRQ Settings */
#define PIRQ_MISC2	0x0B	/* Miscellaneous2 IRQ Settings */
#define PIRQ_SIRQA	0x0C	/* Serial IRQ INTA */
#define PIRQ_SIRQB	0x0D	/* Serial IRQ INTB */
#define PIRQ_SIRQC	0x0E	/* Serial IRQ INTC */
#define PIRQ_SIRQD	0x0F	/* Serial IRQ INTD */
#define PIRQ_SCI	0x10	/* SCI IRQ */
#define PIRQ_SMBUS	0x11	/* SMBUS	14h.0 */
#define PIRQ_ASF	0x12	/* ASF */
#define PIRQ_HDA	0x13	/* HDA		14h.2 */
#define PIRQ_FC		0x14	/* FC */
#define PIRQ_GEC	0x15	/* GEC */
#define PIRQ_PMON	0x16	/* Performance Monitor */
#define PIRQ_SD		0x17	/* SD */
#define PIRQ_IMC0	0x20	/* IMC INT0 */
#define PIRQ_IMC1	0x21	/* IMC INT1 */
#define PIRQ_IMC2	0x22	/* IMC INT2 */
#define PIRQ_IMC3	0x23	/* IMC INT3 */
#define PIRQ_IMC4	0x24	/* IMC INT4 */
#define PIRQ_IMC5	0x25	/* IMC INT5 */
#define PIRQ_OHCI1	0x30	/* USB OHCI	12h.0 */
#define PIRQ_EHCI1	0x31	/* USB EHCI	12h.2 */
#define PIRQ_OHCI2	0x32	/* USB OHCI	13h.0 */
#define PIRQ_EHCI2	0x33	/* USB EHCI	13h.2 */
#define PIRQ_OHCI3	0x34	/* USB OHCI	16h.0 */
#define PIRQ_EHCI3	0x35	/* USB EHCI	16h.2 */
#define PIRQ_OHCI4	0x36	/* USB OHCI	14h.5 */
#define PIRQ_IDE	0x40	/* IDE		14h.1 */
#define PIRQ_SATA	0x41	/* SATA		11h.0 */

#if IS_ENABLED(CONFIG_SOUTHBRIDGE_AMD_PI_AVALON)
#define FCH_INT_TABLE_SIZE 0x63
#define PIRQ_GPIO	0x62	/* GPIO Controller Interrupt */
#endif

#if IS_ENABLED(CONFIG_SOUTHBRIDGE_AMD_PI_BOLTON)
#define FCH_INT_TABLE_SIZE 0x54
#define PIRQ_GPP0	0x50	/* GPP INT 0 */
#define PIRQ_GPP1	0x51	/* GPP INT 1 */
#define PIRQ_GPP2	0x52	/* GPP INT 2 */
#define PIRQ_GPP3	0x53	/* GPP INT 3 */
#endif

#if IS_ENABLED(CONFIG_SOUTHBRIDGE_AMD_PI_KERN)
#define FCH_INT_TABLE_SIZE 0x76
#define PIRQ_GPIO	0x62	/* GPIO Controller Interrupt */
#define PIRQ_I2C0	0x70
#define PIRQ_I2C1	0x71
#define PIRQ_I2C2	0x72
#define PIRQ_I2C3	0x73
#define PIRQ_UART0	0x74
#define PIRQ_UART1	0x75
#endif

#endif /* AMD_PCI_INT_DEFS_H */
