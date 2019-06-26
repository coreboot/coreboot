/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Sergej Ivanov <getinaks@gmail.com>
 * Copyright (C) 2018 Gergely Kiss <mail.gery@gmail.com>
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
#include <device/pnp_ops.h>
#include <device/pci_ops.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <southbridge/amd/common/amd_defs.h>
#include <southbridge/amd/agesa/hudson/hudson.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8623e/it8623e.h>

#define ITE_CONFIG_REG_CC	0x02

#if CONFIG_UART_FOR_CONSOLE == 0
#define SERIAL_DEV		PNP_DEV(0x2e, IT8623E_SP1)
#elif CONFIG_UART_FOR_CONSOLE == 1
#define SERIAL_DEV		PNP_DEV(0x2e, IT8623E_SP2)
#else
#error "Invalid value for CONFIG_UART_FOR_CONSOLE"
#endif

#define GPIO_DEV		PNP_DEV(0x2e, IT8623E_GPIO)
#define CLKIN_DEV		PNP_DEV(0x2e, IT8623E_GPIO)
#define ENVC_DEV		PNP_DEV(0x2e, IT8623E_EC)

/* Sets up EC configuration as per vendor defaults */
static void ite_evc_conf(pnp_devfn_t dev)
{
	pnp_set_enable(dev, 0);
	ite_reg_write(dev, 0x70, 0x00);
	ite_reg_write(dev, 0xf0, 0x00);
	ite_reg_write(dev, 0xf1, 0x00);
	ite_reg_write(dev, 0xf2, 0x06);
	ite_reg_write(dev, 0xf3, 0x00);
	ite_reg_write(dev, 0xf4, 0x00);
	ite_reg_write(dev, 0xf5, 0x36);
	ite_reg_write(dev, 0xf6, 0x03);
	ite_reg_write(dev, 0xf9, 0x48);
	ite_reg_write(dev, 0xfa, 0x00);
	ite_reg_write(dev, 0xfb, 0x10);
	pnp_set_enable(dev, 1);
}

/*
 * Sets up GPIO configuration as per vendor defaults
 * SIO defaults are unknown therefore all GPIO pins are configured
*/
static void ite_gpio_conf(pnp_devfn_t dev)
{
	ite_reg_write(dev, 0x23, 0x08);
	ite_reg_write(dev, 0x25, 0x10);
	ite_reg_write(dev, 0x26, 0x00);
	ite_reg_write(dev, 0x27, 0x80);
	ite_reg_write(dev, 0x28, 0x45);
	ite_reg_write(dev, 0x29, 0x00);
	ite_reg_write(dev, 0x2a, 0x00);
	ite_reg_write(dev, 0x2b, 0x48);
	ite_reg_write(dev, 0x2c, 0x10);
	ite_reg_write(dev, 0x2d, 0x80);
	ite_reg_write(dev, 0x71, 0x00);
	ite_reg_write(dev, 0x72, 0x00);
	ite_reg_write(dev, 0x73, 0x38);
	ite_reg_write(dev, 0x74, 0x00);
	ite_reg_write(dev, 0xb0, 0x00);
	ite_reg_write(dev, 0xb1, 0x00);
	ite_reg_write(dev, 0xb2, 0x00);
	ite_reg_write(dev, 0xb3, 0x00);
	ite_reg_write(dev, 0xb4, 0x00);
	ite_reg_write(dev, 0xb8, 0x00);
	ite_reg_write(dev, 0xb9, 0x00);
	ite_reg_write(dev, 0xba, 0x00);
	ite_reg_write(dev, 0xbb, 0x00);
	ite_reg_write(dev, 0xbc, 0x00);
	ite_reg_write(dev, 0xbd, 0x00);
	ite_reg_write(dev, 0xc0, 0x01);
	ite_reg_write(dev, 0xc1, 0x00);
	ite_reg_write(dev, 0xc2, 0x00);
	ite_reg_write(dev, 0xc3, 0x00);
	ite_reg_write(dev, 0xc4, 0x00);
	ite_reg_write(dev, 0xc8, 0x01);
	ite_reg_write(dev, 0xc9, 0x00);
	ite_reg_write(dev, 0xca, 0x00);
	ite_reg_write(dev, 0xcb, 0x00);
	ite_reg_write(dev, 0xcc, 0x00);
	ite_reg_write(dev, 0xcd, 0x20);
	ite_reg_write(dev, 0xce, 0x00);
	ite_reg_write(dev, 0xcf, 0x00);
	ite_reg_write(dev, 0xe0, 0x00);
	ite_reg_write(dev, 0xe1, 0x00);
	ite_reg_write(dev, 0xe2, 0x00);
	ite_reg_write(dev, 0xe3, 0x00);
	ite_reg_write(dev, 0xe4, 0x00);
	ite_reg_write(dev, 0xe9, 0x21);
	ite_reg_write(dev, 0xf0, 0x00);
	ite_reg_write(dev, 0xf1, 0x00);
	ite_reg_write(dev, 0xf2, 0x00);
	ite_reg_write(dev, 0xf3, 0x00);
	ite_reg_write(dev, 0xf4, 0x00);
	ite_reg_write(dev, 0xf5, 0x00);
	ite_reg_write(dev, 0xf6, 0x00);
	ite_reg_write(dev, 0xf7, 0x00);
	ite_reg_write(dev, 0xf8, 0x00);
	ite_reg_write(dev, 0xf9, 0x00);
	ite_reg_write(dev, 0xfa, 0x00);
	ite_reg_write(dev, 0xfb, 0x00);
}

void board_BeforeAgesa(struct sysinfo *cb)
{
	int i;
	u32 val;
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

	/* Disable PCI-PCI bridge and release GPIO32/33 for other uses. */
	outb(0xEA, 0xcd6);
	outb(0x1, 0xcd7);

	/* Set LPC decode enables. */
	pci_devfn_t dev2 = PCI_DEV(0, 0x14, 3);
	pci_write_config32(dev2, 0x44, 0xff03ffd5);

	hudson_lpc_port80();

	/* Enable the AcpiMmio space */
	outb(0x24, 0xcd6);
	outb(0x1, 0xcd7);

	/* Configure ClkDrvStr1 settings */
	addr32 = (u32 *)0xfed80e24;
	*addr32 = 0x030800aa;

	/* Configure MiscClkCntl1 settings */
	addr32 = (u32 *)0xfed80e40;
	*addr32 = 0x000c4050;

	/* enable SIO LPC decode */
	dev = PCI_DEV(0, 0x14, 3);
	byte = pci_read_config8(dev, 0x48);
	byte |= 3;	/* 2e, 2f & 4e, 4f */
	pci_write_config8(dev, 0x48, byte);

	ite_gpio_conf(GPIO_DEV);
	ite_evc_conf(ENVC_DEV);

	ite_conf_clkin(CLKIN_DEV, ITE_UART_CLK_PREDIVIDE_48);
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	ite_kill_watchdog(GPIO_DEV);

	/*
	* On Larne, after LpcClkDrvSth is set, it needs some time to be stable,
	* because of the buffer ICS551M
	*/
	for (i = 0; i < 200000; i++)
		val = inb(0xcd6);
}
