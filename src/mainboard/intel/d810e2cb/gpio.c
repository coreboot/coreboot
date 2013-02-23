/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Joseph Smith <joe@settoplinux.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#define PME_DEV			PNP_DEV(0x4e, 0x0a)
#define PME_IO_BASE_ADDR	0x800      /* Runtime register base address */

/* Early mainboard specific GPIO setup. */
static void mb_gpio_init(void)
{
	device_t dev;
	uint16_t port;

	/* Southbridge GPIOs. */
	/* Set the LPC device statically. */
	dev = PCI_DEV(0x0, 0x1f, 0x0);

	/* Set the value for GPIO base address register and enable GPIO. */
	pci_write_config32(dev, GPIO_BASE, (GPIO_BASE_ADDR | 1));
	pci_write_config8(dev, GPIO_CNTL, 0x10);

	udelay(10);
	outl(0x1a203180, GPIO_BASE_ADDR + 0x00); /* GPIO_USE_SEL */
	outl(0x0000ffff, GPIO_BASE_ADDR + 0x04); /* GP_IO_SEL */
	outl(0x13bf0000, GPIO_BASE_ADDR + 0x0c); /* GP_LVL */
	outl(0x00040000, GPIO_BASE_ADDR + 0x18); /* GPO_BLINK */
	outl(0x000039ff, GPIO_BASE_ADDR + 0x2c); /* GPI_INV */

	/* Super I/O GPIOs. */
	dev = PME_DEV;
	port = dev >> 8;

	/* Enter the configuration state. */
	outb(0x55, port);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, PME_IO_BASE_ADDR);
	pnp_set_enable(dev, 1);

	/* GP10 - J1B1 */
	outl(0x01, PME_IO_BASE_ADDR + 0x23);

	/* GP11 - J1B2 */
	outl(0x01, PME_IO_BASE_ADDR + 0x24);

	/* GP12 - J2B1 */
	outl(0x01, PME_IO_BASE_ADDR + 0x25);

	/* GP13 - J2B2 */
	outl(0x01, PME_IO_BASE_ADDR + 0x26);

	/* GP14 - J1X */
	outl(0x01, PME_IO_BASE_ADDR + 0x27);

	/* GP15 - J1Y */
	outl(0x01, PME_IO_BASE_ADDR + 0x28);

	/* GP16 - J2X */
	outl(0x01, PME_IO_BASE_ADDR + 0x29);

	/* GP17 - J2Y */
	outl(0x01, PME_IO_BASE_ADDR + 0x2a);

	/* GP20 - 8042 P17 */
	outl(0x01, PME_IO_BASE_ADDR + 0x2b);

	/* GP21 - 8042 P16 */
	outl(0x00, PME_IO_BASE_ADDR + 0x2c);

	/* GP22 - 8042 P12 */
	outl(0x00, PME_IO_BASE_ADDR + 0x2d);

	/* GP24 */
	outl(0x00, PME_IO_BASE_ADDR + 0x2f);

	/* GP25 - MIDI_IN */
	outl(0x01, PME_IO_BASE_ADDR + 0x30);

	/* GP26 - MIDI_OUT */
	outl(0x01, PME_IO_BASE_ADDR + 0x31);

	/* GP27 - nIO_SMI */
	outl(0x04, PME_IO_BASE_ADDR + 0x32);

	/* GP30 - FAN_TACH2 */
	outl(0x05, PME_IO_BASE_ADDR + 0x33);

	/* GP31 - FAN_TACH1 */
	outl(0x05, PME_IO_BASE_ADDR + 0x34);

	/* GP32 - FAN2 */
	outl(0x04, PME_IO_BASE_ADDR + 0x35);

	/* GP33 - FAN1 */
	outl(0x04, PME_IO_BASE_ADDR + 0x36);

	/* GP34 - IRRX2 */
	outl(0x05, PME_IO_BASE_ADDR + 0x37);

	/* GP35 - IRTX2 */
	outl(0x04, PME_IO_BASE_ADDR + 0x38);

	/* GP36 - nKBDRST */
	outl(0x84, PME_IO_BASE_ADDR + 0x39);

	/* GP37 - A20M */
	outl(0x84, PME_IO_BASE_ADDR + 0x3a);

	/* GP40 - DRVDEN0 */
	outl(0x04, PME_IO_BASE_ADDR + 0x3b);

	/* GP41 - DRVDEN1 */
	outl(0x04, PME_IO_BASE_ADDR + 0x3c);

	/* GP42 - nIO_PME */
	outl(0x84, PME_IO_BASE_ADDR + 0x3d);

	/* GP43 */
	outl(0x00, PME_IO_BASE_ADDR + 0x3e);

	/* GP50 - nIR2 */
	outl(0x05, PME_IO_BASE_ADDR + 0x3f);

	/* GP51 - nDCD2 */
	outl(0x05, PME_IO_BASE_ADDR + 0x40);

	/* GP52 - RXD2 */
	outl(0x05, PME_IO_BASE_ADDR + 0x41);

	/* GP53 - TXD2 */
	outl(0x04, PME_IO_BASE_ADDR + 0x42);

	/* GP54 - nDSR2 */
	outl(0x05, PME_IO_BASE_ADDR + 0x43);

	/* GP55 - nRTS2 */
	outl(0x04, PME_IO_BASE_ADDR + 0x44);

	/* GP56 - nCTS2 */
	outl(0x05, PME_IO_BASE_ADDR + 0x45);

	/* GP57 - nDTR2 */
	outl(0x04, PME_IO_BASE_ADDR + 0x46);

	/* GP60 - LED1 */
	outl(0x84, PME_IO_BASE_ADDR + 0x47);

	/* GP61 - LED2 */
	outl(0x84, PME_IO_BASE_ADDR + 0x48);

	/* GP1 */
	outl(0x00, PME_IO_BASE_ADDR + 0x4b);

	/* GP2 */
	outl(0x14, PME_IO_BASE_ADDR + 0x4c);

	/* GP3 */
	outl(0xda, PME_IO_BASE_ADDR + 0x4d);

	/* GP4 */
	outl(0x08, PME_IO_BASE_ADDR + 0x4e);

	/* GP5 */
	outl(0x00, PME_IO_BASE_ADDR + 0x4f);

	/* GP6 */
	outl(0x00, PME_IO_BASE_ADDR + 0x50);

	/* FAN1 */
	outl(0x01, PME_IO_BASE_ADDR + 0x56);

	/* FAN2 */
	outl(0x01, PME_IO_BASE_ADDR + 0x57);

	/* Fan Control */
	outl(0xf0, PME_IO_BASE_ADDR + 0x58);

	/* Fan1 Preload */
	outl(0x00, PME_IO_BASE_ADDR + 0x5b);

	/* Fan2 Preload */
	outl(0x00, PME_IO_BASE_ADDR + 0x5c);

	/* LED1 */
	outl(0x03, PME_IO_BASE_ADDR + 0x5d);

	/* LED2 */
	outl(0x03, PME_IO_BASE_ADDR + 0x5e);

	/* Keyboard Scan Code */
	outl(0x00, PME_IO_BASE_ADDR + 0x5f);

	/* Exit the configuration state. */
	outb(0xaa, port);
}
