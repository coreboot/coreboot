/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Joseph Smith <joe@settoplinux.org>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#define PME_DEV			PNP_DEV(0x2e, 0x0a)
#define PME_IO_BASE_ADDR	0x800      /* Runtime register base address */
#define ICH_IO_BASE_ADDR	0x00000500 /* GPIO base address register */

/* Early mainboard specific GPIO setup. */
static void mb_gpio_init(void)
{
	device_t dev;
	uint16_t port;
	uint32_t set_gpio;

	/* Southbridge GPIOs. */
	/* Set the LPC device statically. */
	dev = PCI_DEV(0x0, 0x1f, 0x0);

	/* Set the value for GPIO base address register and enable GPIO. */
	pci_write_config32(dev, GPIO_BASE_ICH0_5, (ICH_IO_BASE_ADDR | 1));
	pci_write_config8(dev, GPIO_CNTL_ICH0_5, 0x10);

	/* Set GPIO25 to input and drive GPIO23 to high,
	 * this enables the LAN controller. 
	 */
	udelay(10);
	set_gpio = 0x0000ffff;
	set_gpio |= 1 << 25;
	outl(set_gpio, ICH_IO_BASE_ADDR + 0x04);

	set_gpio = 0x1b3f0000;
	set_gpio |= 1 << 23;
	outl(set_gpio, ICH_IO_BASE_ADDR + 0x0c);

	/* Super I/O GPIOs. */
	dev = PME_DEV;
	port = dev >> 8;

	outb(0x55, port);		/* Enter the configuration state. */
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, PME_IO_BASE_ADDR);
	pnp_set_enable(dev, 1);
	outl(0x03, PME_IO_BASE_ADDR + 0x1e); /* Force Disk Change */
	outl(0x02, PME_IO_BASE_ADDR + 0x1f); /* Floppy Data Rate */
	outl(0x81, PME_IO_BASE_ADDR + 0x20); /* UART1 FIFO */
	outl(0x81, PME_IO_BASE_ADDR + 0x21); /* UART2 FIFO */
	outl(0x00, PME_IO_BASE_ADDR + 0x22); /* Device Disable */
	outl(0x01, PME_IO_BASE_ADDR + 0x23); /* GP10 */
	outl(0x01, PME_IO_BASE_ADDR + 0x24); /* GP11 */
	outl(0x01, PME_IO_BASE_ADDR + 0x25); /* GP12 */
	outl(0x01, PME_IO_BASE_ADDR + 0x26); /* GP13 */
	outl(0x01, PME_IO_BASE_ADDR + 0x27); /* GP14 */
	outl(0x01, PME_IO_BASE_ADDR + 0x28); /* GP15 */
	outl(0x01, PME_IO_BASE_ADDR + 0x29); /* GP16 */
	outl(0x01, PME_IO_BASE_ADDR + 0x2a); /* GP17 */
	outl(0x01, PME_IO_BASE_ADDR + 0x2b); /* GP20 */
	outl(0x01, PME_IO_BASE_ADDR + 0x2c); /* GP21 */
	outl(0x01, PME_IO_BASE_ADDR + 0x2d); /* GP22 */
	outl(0x01, PME_IO_BASE_ADDR + 0x2f); /* GP24 */
	outl(0x01, PME_IO_BASE_ADDR + 0x30); /* GP25 */
	outl(0x01, PME_IO_BASE_ADDR + 0x31); /* GP26 */
	outl(0x01, PME_IO_BASE_ADDR + 0x32); /* GP27 */
	outl(0x05, PME_IO_BASE_ADDR + 0x33); /* GP30 */
	outl(0x05, PME_IO_BASE_ADDR + 0x34); /* GP31 */
	outl(0x84, PME_IO_BASE_ADDR + 0x35); /* GP32 */
	outl(0x84, PME_IO_BASE_ADDR + 0x36); /* GP33 */
	outl(0x00, PME_IO_BASE_ADDR + 0x37); /* GP34 */
	outl(0x04, PME_IO_BASE_ADDR + 0x38); /* GP35 */
	outl(0x01, PME_IO_BASE_ADDR + 0x39); /* GP36 */
	outl(0x01, PME_IO_BASE_ADDR + 0x3a); /* GP37 */
	outl(0x01, PME_IO_BASE_ADDR + 0x3b); /* GP40 */
	outl(0x01, PME_IO_BASE_ADDR + 0x3c); /* GP41 */
	outl(0x86, PME_IO_BASE_ADDR + 0x3d); /* GP42 */
	outl(0x01, PME_IO_BASE_ADDR + 0x3e); /* GP43 */
	outl(0x05, PME_IO_BASE_ADDR + 0x3f); /* GP50 */
	outl(0x05, PME_IO_BASE_ADDR + 0x40); /* GP51 */
	outl(0x05, PME_IO_BASE_ADDR + 0x41); /* GP52 */
	outl(0x04, PME_IO_BASE_ADDR + 0x42); /* GP53 */
	outl(0x05, PME_IO_BASE_ADDR + 0x43); /* GP54 */
	outl(0x04, PME_IO_BASE_ADDR + 0x44); /* GP55 */
	outl(0x05, PME_IO_BASE_ADDR + 0x45); /* GP56 */
	outl(0x04, PME_IO_BASE_ADDR + 0x46); /* GP57 */
	outl(0x01, PME_IO_BASE_ADDR + 0x47); /* GP58 */
	outl(0x01, PME_IO_BASE_ADDR + 0x48); /* GP59 */
	outl(0x00, PME_IO_BASE_ADDR + 0x4b); /* GP1 */
	outl(0x04, PME_IO_BASE_ADDR + 0x4c); /* GP2 */
	outl(0xc0, PME_IO_BASE_ADDR + 0x4d); /* GP3 */
	outl(0x00, PME_IO_BASE_ADDR + 0x4e); /* GP4 */
	outl(0x04, PME_IO_BASE_ADDR + 0x4f); /* GP5 */
	outl(0x00, PME_IO_BASE_ADDR + 0x50); /* GP6 */
	outl(0x01, PME_IO_BASE_ADDR + 0x56); /* FAN1 */
	outl(0x01, PME_IO_BASE_ADDR + 0x57); /* FAN2 */
	outl(0x58, PME_IO_BASE_ADDR + 0x58); /* Fan Control */
	outl(0xff, PME_IO_BASE_ADDR + 0x59); /* Fan1 Tachometer */
	outl(0x50, PME_IO_BASE_ADDR + 0x5a); /* Fan2 Tachometer */
	outl(0x00, PME_IO_BASE_ADDR + 0x5b); /* Fan1 Preload */
	outl(0x00, PME_IO_BASE_ADDR + 0x5c); /* Fan2 Preload */
	outl(0x00, PME_IO_BASE_ADDR + 0x5d); /* LED1 */
	outl(0x00, PME_IO_BASE_ADDR + 0x5e); /* LED2 */
	outl(0x00, PME_IO_BASE_ADDR + 0x5f); /* Keyboard Scan Code */
	outb(0xaa, port);		/* Exit the configuration state. */
}
