/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef CIMX_UTIL_H
#define CIMX_UTIL_H

#include <stdint.h>

/*
 * PIRQ and device routing - these define the index
 * into the FCH PCI_INTR 0xC00/0xC01 interrupt
 * routing table
 */
#if IS_ENABLED(CONFIG_SOUTHBRIDGE_AMD_CIMX_SB800) || \
	IS_ENABLED(CONFIG_SOUTHBRIDGE_AMD_CIMX_SB900)
#define FCH_INT_TABLE_SIZE 0x54
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
#define PIRQ_SIRQB	0x0D	/* Serial IRQ INTA */
#define PIRQ_SIRQC	0x0E	/* Serial IRQ INTA */
#define PIRQ_SIRQD	0x0F	/* Serial IRQ INTA */
#define PIRQ_SCI	0x10	/* SCI IRQ */
#define PIRQ_SMBUS	0x11	/* SMBUS	14h.0 */
#define PIRQ_ASF	0x12	/* ASF */
#define PIRQ_HDA	0x13	/* HDA		14h.2 */
#define PIRQ_FC		0x14	/* FC */
#define PIRQ_GEC	0x15	/* GEC */
#define PIRQ_PMON	0x16	/* Performance Monitor */
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
#define PIRQ_GPP0	0x50	/* GPP INT 0 */
#define PIRQ_GPP1	0x51	/* GPP INT 1 */
#define PIRQ_GPP2	0x52	/* GPP INT 2 */
#define PIRQ_GPP3	0x53	/* GPP INT 3 */
#elif IS_ENABLED(CONFIG_SOUTHBRIDGE_AMD_CIMX_SB700)
#define FCH_INT_TABLE_SIZE 0xD
#define PIRQ_NC		0x1F	/* Not Used */
#define PIRQ_A		0x00	/* INT A */
#define PIRQ_B		0x01	/* INT B */
#define PIRQ_C		0x02	/* INT C */
#define PIRQ_D		0x03	/* INT D */
#define PIRQ_ACPI	0x04	/* ACPI */
#define PIRQ_SMBUS	0x05	/* SMBUS */
/* Index 6, 7, 8 are all reserved */
#define PIRQ_E		0x09	/* INT E */
#define PIRQ_F		0x0A	/* INT F */
#define PIRQ_G		0x0B	/* INT G */
#define PIRQ_H		0x0C	/* INT H */
#endif

/* FCH index/data registers */
#define BIOSRAM_INDEX	0xcd4
#define BIOSRAM_DATA	0xcd5
#define PM_INDEX		0xcd6
#define PM_DATA			0xcd7
#define PM2_INDEX		0xcd0
#define PM2_DATA		0xcd1
#define PCI_INTR_INDEX	0xc00
#define PCI_INTR_DATA	0xc01

void pm_iowrite(u8 reg, u8 value);
u8 pm_ioread(u8 reg);
void pm2_iowrite(u8 reg, u8 value);
u8 pm2_ioread(u8 reg);

#ifndef __PRE_RAM__

struct pirq_struct {
u8 devfn;
u8 PIN[4];	/* PINA/B/C/D are index 0/1/2/3 */
};

extern const struct pirq_struct * pirq_data_ptr;
extern u32 pirq_data_size;
extern const u8 * intr_data_ptr;
extern const u8 * picr_data_ptr;

u8 read_pci_int_idx(u8 index, int mode);
void write_pci_int_idx(u8 index, int mode, u8 data);
void write_pci_cfg_irqs(void);
void write_pci_int_table (void);
#endif /* __PRE_RAM */

#endif /* CIMX_UTIL_H */
