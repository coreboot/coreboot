/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 * Copyright (C) 2013 Vladimir Serbinenko <phcoder@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define __SIMPLE_DEVICE__
#include <console/console.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pnp.h>
#include <delay.h>
#include "dock.h"
#include <superio/nsc/pc87382/pc87382.h>

#include <southbridge/intel/i82801ix/i82801ix.h>
#include "ec/lenovo/h8/h8.h"
#include <ec/acpi/ec.h>

struct pin_config {
	u8 port;
	u8 mode;
};

static int poll_clk_stable(pnp_devfn_t dev)
{
	int timeout = 1000;

	/* Enable 14.318MHz CLK on CLKIN */
	pnp_write_config(dev, 0x29, 0xa0);
	while(!(pnp_read_config(dev, 0x29) & 0x10) && timeout--)
		udelay(1000);
	if (!timeout)
		return 1;

	return 0;
}

static int gpio_init(pnp_devfn_t gpio, u16 gpio_base,
	const struct pin_config pincfg[], int num_cfgs)
{
	int i;

	/* Enable GPIO LDN. */
	pnp_set_logical_device(gpio);
	pnp_set_iobase(gpio, PNP_IDX_IO0, gpio_base);
	pnp_set_enable(gpio, 1);

	for (i = 0; i < num_cfgs; i++) {
		pnp_write_config(gpio, 0xf0, pincfg[i].port);
		pnp_write_config(gpio, 0xf1, pincfg[i].mode);
		pnp_write_config(gpio, 0xf2, 0x0);
	}
	return 0;
}

static const pnp_devfn_t l_dlpc = PNP_DEV(0x164e, PC87382_DOCK);
static const pnp_devfn_t l_gpio = PNP_DEV(0x164e, PC87382_GPIO);

#define DLPC_CONTROL	0x164c
#define DLPC_GPIO_BASE	0x1680

#define DLPC_GPDO0		(DLPC_GPIO_BASE + 0x0)
#define DLPC_GPDI0		(DLPC_GPIO_BASE + 0x1)
#define		D_PLTRST	0x01
#define		D_LPCPD		0x02

#define DLPC_GPDO2		(DLPC_GPIO_BASE + 0x8)
#define DLPC_GPDI2		(DLPC_GPIO_BASE + 0x9)

static int pc87382_init(pnp_devfn_t dlpc, u16 dlpc_base)
{
	int timeout = 1000;

	/* Enable LPC bridge LDN. */
	pnp_set_logical_device(dlpc);
	pnp_set_iobase(dlpc, PNP_IDX_IO0, dlpc_base);
	pnp_set_enable(dlpc, 1);

	/* Reset docking state */
	outb(0x00, dlpc_base);
	outb(0x07, dlpc_base);
	while (!(inb(dlpc_base) & 8) && timeout--)
		udelay(1000);
	if (!timeout)
		return 1;

	return 0;
}

static void pc87382_close(pnp_devfn_t dlpc)
{
	pnp_set_logical_device(dlpc);

	/* Disconnect LPC bus */
	u16 dlpc_base = pnp_read_iobase(dlpc, PNP_IDX_IO0);
	outb(0x00, dlpc_base);
	pnp_set_enable(dlpc, 0);
}

static const struct pin_config local_gpio[] = {
	{0x00, 3},	{0x01, 3},	{0x02, 0},	{0x03, 3},
	{0x04, 4},	{0x20, 4},	{0x21, 4},	{0x23, 4},
};

static int pc87382_connect(void)
{
	u8 reg;

	if (poll_clk_stable(l_gpio) != 0)
		return 1;

	if (gpio_init(l_gpio, DLPC_GPIO_BASE,
		local_gpio, ARRAY_SIZE(local_gpio)) != 0) {
		return 1;
	}

	reg = inb(DLPC_GPDO0);
	reg |= D_PLTRST | D_LPCPD;
	/* Deassert D_PLTRST# and D_LPCPD# */
	outb(reg, DLPC_GPDO0);

	if (pc87382_init(l_dlpc, DLPC_CONTROL) != 0)
		return 1;

	/* Assert D_PLTRST# */
	reg &= ~D_PLTRST;
	outb(reg, DLPC_GPDO0);
	udelay(1000);

	/* Deassert D_PLTRST# */
	reg |= D_PLTRST;
	outb(reg, DLPC_GPDO0);
	mdelay(10);

	return 0;
}

static void pc87382_disconnect(void)
{
	pc87382_close(l_dlpc);

	/* Assert D_PLTRST# and D_LPCPD# */
	u8 reg = inb(DLPC_GPDO0);
	reg &= ~(D_PLTRST | D_LPCPD);
	outb(reg, DLPC_GPDO0);
}

static u8 dock_identify(void)
{
	u8 id;

	id = (inb(DLPC_GPDI0) >> 4) & 1;
	id |= (inb(DLPC_GPDI2) & 3) << 1;

	return id;
}

/* Docking station side. */

#include <superio/nsc/pc87384/pc87384.h>

static const pnp_devfn_t r_gpio = PNP_DEV(0x2e, PC87384_GPIO);
static const pnp_devfn_t r_serial = PNP_DEV(0x2e, PC87384_SP1);

#define DOCK_GPIO_BASE	0x1620

static const struct pin_config remote_gpio[] = {
	{0x00, PC87384_GPIO_PIN_DEBOUNCE | PC87384_GPIO_PIN_PULLUP},
	{0x01, PC87384_GPIO_PIN_TYPE_PUSH_PULL | PC87384_GPIO_PIN_OE},
	{0x02, PC87384_GPIO_PIN_TYPE_PUSH_PULL | PC87384_GPIO_PIN_OE},
	{0x03, PC87384_GPIO_PIN_DEBOUNCE | PC87384_GPIO_PIN_PULLUP},
	{0x04, PC87384_GPIO_PIN_DEBOUNCE | PC87384_GPIO_PIN_PULLUP},
	{0x05, PC87384_GPIO_PIN_DEBOUNCE | PC87384_GPIO_PIN_PULLUP},
	{0x06, PC87384_GPIO_PIN_DEBOUNCE | PC87384_GPIO_PIN_PULLUP},
	{0x07, PC87384_GPIO_PIN_DEBOUNCE | PC87384_GPIO_PIN_PULLUP},
};

static int pc87384_init(void)
{
	if (poll_clk_stable(r_gpio) != 0)
		return 1;

	/* set GPIO pins to Serial/Parallel Port
	 * functions
	 */
	pnp_write_config(r_gpio, 0x22, 0xa9);

	/* enable serial port */
	pnp_set_logical_device(r_serial);
	pnp_set_iobase(r_serial, PNP_IDX_IO0, 0x3f8);
	pnp_set_enable(r_serial, 1);

	if (gpio_init(r_gpio, DOCK_GPIO_BASE,
		remote_gpio, ARRAY_SIZE(remote_gpio)) != 0)
		return 1;

	/* no GPIO events enabled for PORT0 */
	outb(0x00, DOCK_GPIO_BASE + 0x02);
	/* clear GPIO events on PORT0 */
	outb(0xff, DOCK_GPIO_BASE + 0x03);
	outb(0xff, DOCK_GPIO_BASE + 0x04);

	/* no GPIO events enabled for PORT1 */
	outb(0x00, DOCK_GPIO_BASE + 0x06);
	/* clear GPIO events on PORT1*/
	outb(0xff, DOCK_GPIO_BASE + 0x07);
	outb(0x1f, DOCK_GPIO_BASE + 0x08);

	outb(0xfd, DOCK_GPIO_BASE + 0x00);

	return 0;
}

/* Mainboard */

void dock_connect(void)
{
	if (dock_identify() == 0)
		return;

	if (pc87382_connect() != 0) {
		pc87382_disconnect();
		return;
	}
	pc87384_init();

	ec_write(H8_LED_CONTROL,
		 H8_LED_CONTROL_OFF | H8_LED_CONTROL_DOCK_LED1);
	ec_write(H8_LED_CONTROL,
		 H8_LED_CONTROL_ON  | H8_LED_CONTROL_DOCK_LED2);
}

void dock_disconnect(void)
{
	pc87382_disconnect();

	ec_write(H8_LED_CONTROL,
		 H8_LED_CONTROL_OFF | H8_LED_CONTROL_DOCK_LED1);
	ec_write(H8_LED_CONTROL,
		 H8_LED_CONTROL_OFF | H8_LED_CONTROL_DOCK_LED2);
}

void h8_mainboard_init_dock(void)
{
	u8 id = dock_identify();

	if (id != 0) {
		printk(BIOS_DEBUG, "dock (id=%d) is present\n", id);
		dock_connect();
	} else
		printk(BIOS_DEBUG, "dock is not connected\n");
}
