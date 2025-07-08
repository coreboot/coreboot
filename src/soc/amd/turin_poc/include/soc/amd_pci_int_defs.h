/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_TURIN_POC_AMD_PCI_INT_DEFS_H
#define AMD_TURIN_POC_AMD_PCI_INT_DEFS_H

/*
 *  * PIRQ and device routing - these define the index into the
 *   * FCH PCI_INTR 0xC00/0xC01 interrupt routing table.
 *    */

#define PIRQ_NC		0x1f	/* Not Used */
#define PIRQ_A		0x00	/* INT A */
#define PIRQ_B		0x01	/* INT B */
#define PIRQ_C		0x02	/* INT C */
#define PIRQ_D		0x03	/* INT D */
#define PIRQ_E		0x04	/* INT E */
#define PIRQ_F		0x05	/* INT F */
#define PIRQ_G		0x06	/* INT G */
#define PIRQ_H		0x07	/* INT H */
#define PIRQ_MISC	0x08	/* Miscellaneous IRQ Settings */
#define PIRQ_MISC0	0x09	/* Miscellaneous0 IRQ Settings */
#define PIRQ_HPET_L	0x0a	/* Miscellaneous1 IRQ Settings */
#define PIRQ_HPET_H	0x0b	/* Miscellaneous2 IRQ Settings */
#define PIRQ_SIRQA	0x0c	/* Serial IRQ INTA */
#define PIRQ_SIRQB	0x0d	/* Serial IRQ INTB */
#define PIRQ_SIRQC	0x0e	/* Serial IRQ INTC */
#define PIRQ_SIRQD	0x0f	/* Serial IRQ INTD */
#define PIRQ_SCI	0x10	/* SCI IRQ */
#define PIRQ_SMBUS	0x11	/* SMBUS0 */
#define PIRQ_ASF	0x12	/* ASF */
#define PIRQ_HDA	0x13	/* HDA */
#define PIRQ_GBE0	0x14	/* GBE0 */
#define PIRQ_GBE1	0x15	/* GBE1 */
#define PIRQ_PMON	0x16	/* Performance Monitor */
#define PIRQ_SD		0x17	/* SD */
/* 0x18-0x19 reserved */
#define PIRQ_SDIO	0x1a	/* SDIO */
/* 0x1b-0x1f reserved */
#define PIRQ_CIR	0x20	/* CIR */
#define PIRQ_GPIOA	0x21	/* GPIOa */
#define PIRQ_GPIOB	0x22	/* GPIOb */
#define PIRQ_GPIOC	0x23	/* GPIOc */
/* 0x24-0x2f reserved */
#define PIRQ_USB_EMU	0x30	/* USB Emulation */
#define PIRQ_USB_DR0	0x31	/* USB Dual Role 1 */
#define PIRQ_USB_DR1	0x32	/* USB Dual Role 2 */
/* 0x33 reserved */
#define PIRQ_XHCI0	0x34	/* XHCI0 */
#define PIRQ_SSIC	0x35	/* USB SSIC */
/* 0x36-0x40 reserved */
#define PIRQ_SATA	0x41	/* SATA */
#define PIRQ_UFS	0x42	/* UFS */
/* 0x43-0x4f reserved */
#define PIRQ_GPP0	0x50	/* GPPInt0 */
#define PIRQ_GPP1	0x51	/* GPPInt1 */
#define PIRQ_GPP2	0x52	/* GPPInt2 */
#define PIRQ_GPP3	0x53	/* GPPInt3 */
/* 0x54-0x59 reserved */
#define PIRQ_GSCI	0x60	/* SCI Interrupt */
#define PIRQ_GSMI	0x61	/* SMI Interrupt */
#define PIRQ_GPIO	0x62	/* GPIO Interrupt */
/* 0x63-0x6f reserved */
#define PIRQ_I2C0	0x70	/* I2C0/I3C0 */
#define PIRQ_I2C1	0x71	/* I2C1/I3C1 */
#define PIRQ_I2C2	0x72	/* I2C2/I3C2 */
#define PIRQ_I2C3	0x73	/* I2C3/I3C3 */
#define PIRQ_UART0	0x74	/* UART0 */
#define PIRQ_UART1	0x75	/* UART1 */
#define PIRQ_I2C4	0x76	/* I2C4 */
#define PIRQ_I2C5	0x77	/* I2C5 */
#define PIRQ_UART2	0x78	/* UART2 */
#define PIRQ_UART3	0x79	/* UART3 */

#endif /* AMD_TURIN_POC_AMD_PCI_INT_DEFS_H */
