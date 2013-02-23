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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
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
	pci_write_config32(dev, GPIO_BASE, (ICH_IO_BASE_ADDR | 1));
	pci_write_config8(dev, GPIO_CNTL, 0x10);

	/* Set GPIO23 to high, this enables the LAN controller. */
	udelay(10);
	set_gpio = inl(ICH_IO_BASE_ADDR + 0x0c);
	set_gpio |= 1 << 23;
	outl(set_gpio, ICH_IO_BASE_ADDR + 0x0c);

	/* Disable AC97 Modem */
	pci_write_config8(dev, 0xf2, 0x40);

	/* Super I/O GPIOs. */
	dev = PME_DEV;
	port = dev >> 8;

	/* Enter the configuration state. */
	outb(0x55, port);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, PME_IO_BASE_ADDR);
	pnp_set_enable(dev, 1);

	/* GP21 - LED_RED */
	outl(0x01, PME_IO_BASE_ADDR + 0x2c);

	/* GP30 - FAN2_TACH */
	outl(0x05, PME_IO_BASE_ADDR + 0x33);

	/* GP31 - FAN1_TACH */
	outl(0x05, PME_IO_BASE_ADDR + 0x34);

	/* GP32 - FAN2_CTRL */
	outl(0x04, PME_IO_BASE_ADDR + 0x35);

	/* GP33 - FAN1_CTRL */
	outl(0x04, PME_IO_BASE_ADDR + 0x36);

	/* GP34 - AUD_MUTE_OUT_R */
	outl(0x00, PME_IO_BASE_ADDR + 0x37);

	/* GP36 - KBRST */
	outl(0x00, PME_IO_BASE_ADDR + 0x39);

	/* GP37 - A20GATE */
	outl(0x00, PME_IO_BASE_ADDR + 0x3a);

	/* GP42 - GPIO_PME_OUT */
	outl(0x00, PME_IO_BASE_ADDR + 0x3d);

	/* GP50 - SER2_RI */
	outl(0x05, PME_IO_BASE_ADDR + 0x3f);

	/* GP51 - SER2_DCD */
	outl(0x05, PME_IO_BASE_ADDR + 0x40);

	/* GP52 - SER2_RX */
	outl(0x05, PME_IO_BASE_ADDR + 0x41);

	/* GP53 - SER2_TX */
	outl(0x04, PME_IO_BASE_ADDR + 0x42);

	/* GP55 - SER2_RTS */
	outl(0x04, PME_IO_BASE_ADDR + 0x44);

	/* GP56 - SER2_CTS */
	outl(0x05, PME_IO_BASE_ADDR + 0x45);

	/* GP57 - SER2_DTR */
	outl(0x04, PME_IO_BASE_ADDR + 0x46);

	/* GP60 - LED_GREEN */
	outl(0x01, PME_IO_BASE_ADDR + 0x47);

	/* GP61 - LED_YELLOW */
	outl(0x01, PME_IO_BASE_ADDR + 0x48);

	/* GP3 */
	outl(0xc0, PME_IO_BASE_ADDR + 0x4d);

	/* GP4 */
	outl(0x04, PME_IO_BASE_ADDR + 0x4e);

	/* FAN1 */
	outl(0x01, PME_IO_BASE_ADDR + 0x56);

	/* FAN2 */
	outl(0x01, PME_IO_BASE_ADDR + 0x57);

	/* Fan Control */
	outl(0x50, PME_IO_BASE_ADDR + 0x58);

	/* Fan1 Tachometer */
	outl(0xff, PME_IO_BASE_ADDR + 0x59);

	/* Fan2 Tachometer */
	outl(0xff, PME_IO_BASE_ADDR + 0x5a);

	/* LED1 */
	outl(0x00, PME_IO_BASE_ADDR + 0x5d);

	/* LED2 */
	outl(0x00, PME_IO_BASE_ADDR + 0x5e);

	/* Keyboard Scan Code */
	outl(0x00, PME_IO_BASE_ADDR + 0x5f);

	/* Exit the configuration state. */
	outb(0xaa, port);
}
