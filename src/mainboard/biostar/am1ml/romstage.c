/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Sergej Ivanov <getinaks@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <device/pnp_def.h>
#include <console/console.h>
#include <commonlib/loglevel.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <southbridge/amd/common/amd_defs.h>
#include <southbridge/amd/agesa/hudson/hudson.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>


#define ITE_CONFIG_REG_CC	0x02
#define SERIAL_DEV	PNP_DEV(0x2e, IT8728F_SP1)
#define GPIO_DEV	PNP_DEV(0x2e, IT8728F_GPIO)
#define ENVC_DEV	PNP_DEV(0x2e, IT8728F_EC)

#define MMIO_NON_POSTED_START	0xfed00000
#define MMIO_NON_POSTED_END	0xfedfffff
#define SB_MMIO_MISC32(x)	*(volatile u32 *)(AMD_SB_ACPI_MMIO_ADDR + 0xE00 + (x))


static void it_sio_write(pnp_devfn_t dev, u8 reg, u8 value)
{
	pnp_set_logical_device(dev);
	pnp_write_config(dev, reg, value);
}

static void ite_enter_conf(pnp_devfn_t dev)
{
	u16 port = dev >> 8;

	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	outb((port == 0x4e) ? 0xaa : 0x55, port);
}

static void ite_exit_conf(pnp_devfn_t dev)
{
	it_sio_write(dev, ITE_CONFIG_REG_CC, 0x02);
}

static void ite_evc_conf(pnp_devfn_t dev)
{
	ite_enter_conf(dev);
	it_sio_write(dev, 0xf1 , 0x40 );
	it_sio_write(dev, 0xf4 , 0x80 );
	it_sio_write(dev, 0xf5 , 0x00 );
	it_sio_write(dev, 0xf6 , 0xf0 );
	it_sio_write(dev, 0xf9 , 0x48 );
	it_sio_write(dev, 0xfa , 0x00 );
	it_sio_write(dev, 0xfb , 0x00 );
	ite_exit_conf(dev);
}

static void ite_gpio_conf(pnp_devfn_t dev)
{
	ite_enter_conf (dev);
	it_sio_write (dev, 0x25 , 0x80 );
	it_sio_write (dev, 0x26 , 0x07 );
	it_sio_write (dev, 0x28 , 0x81 );
	it_sio_write (dev, 0x2c , 0x06 );
	it_sio_write (dev, 0x72 , 0x00 );
	it_sio_write (dev, 0x73 , 0x00 );
	it_sio_write (dev, 0xb3 , 0x01 );
	it_sio_write (dev, 0xb8 , 0x00 );
	it_sio_write (dev, 0xc0 , 0x00 );
	it_sio_write (dev, 0xc3 , 0x00 );
	it_sio_write (dev, 0xc8 , 0x00 );
	it_sio_write (dev, 0xc9 , 0x07 );
	it_sio_write (dev, 0xcb , 0x01 );
	it_sio_write (dev, 0xf0 , 0x10 );
	it_sio_write (dev, 0xf4 , 0x27 );
	it_sio_write (dev, 0xf8 , 0x20 );
	it_sio_write (dev, 0xf9 , 0x01 );
	ite_exit_conf (dev);
}

void board_BeforeAgesa(struct sysinfo *cb)
{
	u32 val, t32;
	u8 byte;
	pci_devfn_t dev;
	u32 *addr32;

	/* In Hudson RRG, PMIOxD2[5:4] is "Drive strength control for
	 * LpcClk[1:0]".  To be consistent with Parmer, setting to 4mA
	 * even though the register is not documented in the Kabini BKDG.
	 * Otherwise the serial output is bad code.
	 */
	outb(0xD2, 0xcd6);
	outb(0x00, 0xcd7);

	/* Set LPC decode enables. */
	pci_devfn_t dev2 = PCI_DEV(0, 0x14, 3);
	pci_write_config32(dev2, 0x44, 0xff03ffd5);

	hudson_lpc_port80();

	/* Enable the AcpiMmio space */
	outb(0x24, 0xcd6);
	outb(0x1, 0xcd7);

	/* Set auxiliary output clock frequency on OSCOUT1 pin to be 48MHz */
	addr32 = (u32 *)0xfed80e28;
	t32 = *addr32;
	t32 &= 0xfff8ffff;
	*addr32 = t32;

	/* Enable Auxiliary Clock1, disable FCH 14 MHz OscClk */
	addr32 = (u32 *)0xfed80e40;
	t32 = *addr32;
	t32 &= 0xffffbffb;
	*addr32 = t32;

	/* enable SIO LPC decode */
	dev = PCI_DEV(0, 0x14, 3);
	byte = pci_read_config8(dev, 0x48);
	byte |= 3;	/* 2e, 2f */
	pci_write_config8(dev, 0x48, byte);

	/* enable serial decode */
	byte = pci_read_config8(dev, 0x44);
	byte |= (1 << 6);  /* 0x3f8 */
	pci_write_config8(dev, 0x44, byte);

	/* This functions configure SIO as it been done under vendor bios */
	printk(BIOS_DEBUG, "ITE CONFIG ENVC\n");
	ite_evc_conf(ENVC_DEV);
	printk(BIOS_DEBUG, "ITE CONFIG GPIO\n");
	ite_gpio_conf(GPIO_DEV);
	printk(BIOS_DEBUG, "ITE CONFIG DONE\n");


	ite_kill_watchdog(GPIO_DEV);
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	/* On Larne, after LpcClkDrvSth is set, it needs some time to be stable, because of the buffer ICS551M */
	int i;
	for (i = 0; i < 200000; i++)
		val = inb(0xcd6);

	outb(0xEA, 0xCD6);
	outb(0x1, 0xcd7);

	post_code(0x50);
}
