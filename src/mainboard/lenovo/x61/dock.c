/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__
#include <console/console.h>
#include <arch/io.h>
#include <delay.h>
#include <device/pnp_ops.h>
#include <ec/acpi/ec.h>
#include <ec/lenovo/h8/h8.h>
#include "dock.h"
#include <southbridge/intel/i82801hx/i82801hx.h>
#include <superio/nsc/pc87382/pc87382.h>
#include <superio/nsc/pc87392/pc87392.h>

static const pnp_devfn_t dlpc_dev = PNP_DEV(0x164e, PC87382_DOCK);
static const pnp_devfn_t dlpc_gpio = PNP_DEV(0x164e, PC87382_GPIO);
static const pnp_devfn_t dock_gpio = PNP_DEV(0x2e, PC87392_GPIO);
static const pnp_devfn_t dock_parallel = PNP_DEV(0x2e, PC87392_PP);
static const pnp_devfn_t dock_serial = PNP_DEV(0x2e, PC87392_SP1);

static void select_logical_device(pnp_devfn_t dev)
{
	pnp_write_config(dev, 0x07, dev & 0xff);
}

static void dlpc_gpio_set_mode(int port, int mode)
{
	pnp_write_config(dlpc_gpio, 0xf0, port);
	pnp_write_config(dlpc_gpio, 0xf1, mode);
}

static void dock_gpio_set_mode(int port, int mode, int irq)
{
	pnp_write_config(dock_gpio, 0xf0, port);
	pnp_write_config(dock_gpio, 0xf1, mode);
	pnp_write_config(dock_gpio, 0xf2, irq);
}

static void dock_set_usb_power(bool enable)
{
	if (enable) {
		outb(inb(0x1628) | 0x02, 0x1628);
		ec_set_bit(H8_CONFIG2, 0);
	} else {
		outb(inb(0x1628) & ~0x02, 0x1628);
		ec_clr_bit(H8_CONFIG2, 0);
	}
}

static void dock_set_ultrabay_power(bool enable)
{
	if (enable)
		outb(inb(0x1628) | 0x01, 0x1628);
	else
		outb(inb(0x1628) & ~0x01, 0x1628);
}

static void dlpc_gpio_init(void)
{
	select_logical_device(dlpc_gpio);
	pnp_set_iobase(dlpc_gpio, PNP_IDX_IO0, 0x1680);
	pnp_set_enable(dlpc_gpio, 1);

	dlpc_gpio_set_mode(0x00, 3);
	dlpc_gpio_set_mode(0x01, 3);
	dlpc_gpio_set_mode(0x02, 0);
	dlpc_gpio_set_mode(0x03, 3);
	dlpc_gpio_set_mode(0x04, 4);
	dlpc_gpio_set_mode(0x20, 4);
	dlpc_gpio_set_mode(0x21, 4);
	dlpc_gpio_set_mode(0x23, 4);
}

int dlpc_init(void)
{
	int timeout = 1000;

	/* Enable 14.318MHz CLK on CLKIN */
	pnp_write_config(dlpc_gpio, 0x29, 0xa0);
	while (!(pnp_read_config(dlpc_gpio, 0x29) & 0x10) && timeout--)
		udelay(1000);

	if (!timeout)
		return 1;

	select_logical_device(dlpc_dev);
	pnp_set_iobase(dlpc_dev, PNP_IDX_IO0, 0x164c);
	pnp_set_enable(dlpc_dev, 1);

	dlpc_gpio_init();

	return 0;
}

int dock_connect(void)
{
	int timeout = 1000;

	/* Start from the vendor state: dock reset asserted, DLPC powered down. */
	outb(inb(0x1680) & 0xfc, 0x1680);

	outb(0x07, 0x164c);

	timeout = 1000;

	while (!(inb(0x164c) & 8) && timeout--)
		udelay(1000);

	if (!timeout) {
		/* docking failed, disable DLPC switch */
		outb(0x00, 0x164c);
		select_logical_device(dlpc_dev);
		pnp_set_enable(dlpc_dev, 0);
		return 1;
	}

	/* Power up DLPC while keeping D_PLTRST# asserted. */
	outb((inb(0x1680) & 0xfe) | 0x02, 0x1680);
	mdelay(100);
	/* Deassert D_PLTRST#. */
	outb(inb(0x1680) | 0x03, 0x1680);

	mdelay(100);

	/* startup 14.318MHz Clock */
	pnp_write_config(dock_gpio, 0x29, 0x06);
	/* wait until clock is settled */
	timeout = 1000;
	while (!(pnp_read_config(dock_gpio, 0x29) & 0x08) && timeout--)
		udelay(1000);

	if (!timeout)
		return 1;

	/* Pin  6: CLKRUN
	 * Pin 72:  #DR1
	 * Pin 19: #SMI
	 * Pin 73: #MTR
	 */
	pnp_write_config(dock_gpio, 0x24, 0x37);

	/* PNF active HIGH */
	pnp_write_config(dock_gpio, 0x25, 0xa0);

	/* disable FDC */
	pnp_write_config(dock_gpio, 0x26, 0x01);

	/* Enable GPIO IRQ to #SMI */
	pnp_write_config(dock_gpio, 0x28, 0x02);

	select_logical_device(dock_gpio);
	pnp_set_iobase(dock_gpio, PNP_IDX_IO0, 0x1620);

	/* init GPIO pins */
	dock_gpio_set_mode(0x00, PC87392_GPIO_PIN_DEBOUNCE | PC87392_GPIO_PIN_PULLUP, 0x00);

	dock_gpio_set_mode(0x01, PC87392_GPIO_PIN_DEBOUNCE | PC87392_GPIO_PIN_PULLUP,
			   PC87392_GPIO_PIN_TRIGGERS_SMI);

	dock_gpio_set_mode(0x02, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x03, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x04, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x05, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x06, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x07, PC87392_GPIO_PIN_PULLUP | PC87392_GPIO_PIN_DEBOUNCE,
			   PC87392_GPIO_PIN_TRIGGERS_SMI);

	dock_gpio_set_mode(0x10, PC87392_GPIO_PIN_DEBOUNCE | PC87392_GPIO_PIN_PULLUP,
			   PC87392_GPIO_PIN_TRIGGERS_SMI);

	dock_gpio_set_mode(0x11, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x12, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x13, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x14, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x15, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x16, PC87392_GPIO_PIN_PULLUP | PC87392_GPIO_PIN_OE, 0x00);

	dock_gpio_set_mode(0x17, PC87392_GPIO_PIN_PULLUP, 0x00);

	dock_gpio_set_mode(0x20, PC87392_GPIO_PIN_TYPE_PUSH_PULL | PC87392_GPIO_PIN_OE, 0x00);

	dock_gpio_set_mode(0x21, PC87392_GPIO_PIN_TYPE_PUSH_PULL | PC87392_GPIO_PIN_OE, 0x00);

	dock_gpio_set_mode(0x22, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x23, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x24, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x25, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x26, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x27, PC87392_GPIO_PIN_PULLUP, 0x00);

	dock_gpio_set_mode(0x30, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x31, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x32, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x33, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x34, PC87392_GPIO_PIN_PULLUP, 0x00);

	dock_gpio_set_mode(0x35, PC87392_GPIO_PIN_PULLUP | PC87392_GPIO_PIN_OE, 0x00);

	dock_gpio_set_mode(0x36, PC87392_GPIO_PIN_PULLUP, 0x00);
	dock_gpio_set_mode(0x37, PC87392_GPIO_PIN_PULLUP, 0x00);

	/* enable GPIO */
	pnp_set_enable(dock_gpio, 1);

	/* Enable UltraBay power if a device is present, preserving other dock power bits. */
	dock_set_ultrabay_power(dock_ultrabay_device_present());

	outb(0x00, 0x1623);
	outb(0x82, 0x1622);
	outb(inb(0x1624) | 0x40, 0x1624);

	/* Enable dock USB power and mirror slice/dock state into the EC. */
	dock_set_usb_power(true);

	select_logical_device(dock_parallel);
	pnp_set_iobase(dock_parallel, PNP_IDX_IO0, 0x3bc);
	pnp_set_irq(dock_parallel, PNP_IDX_IRQ0, 7);
	pnp_set_enable(dock_parallel, 1);

	select_logical_device(dock_serial);
	pnp_set_iobase(dock_serial, PNP_IDX_IO0, 0x3f8);
	pnp_set_irq(dock_serial, PNP_IDX_IRQ0, 4);
	pnp_set_enable(dock_serial, 1);
	return 0;
}

void dock_disconnect(void)
{
	printk(BIOS_DEBUG, "%s enter\n", __func__);
	/* Assert D_PLTRST# and DLPCPD before dropping dock power and LPC. */
	outb(inb(0x1680) & 0xfc, 0x1680);
	mdelay(10);

	/* Disable dock USB and UltraBay power. */
	dock_set_usb_power(false);
	dock_set_ultrabay_power(false);
	udelay(10000);

	/* Disconnect LPC bus. */
	outb(0x00, 0x164c);

	printk(BIOS_DEBUG, "%s finish\n", __func__);
}

int dock_present(void)
{
	return !((inw(DEFAULT_GPIOBASE + 0x0c) >> 13) & 1);
}

int dock_ultrabay_device_present(void)
{
	return inb(0x1621) & 0x02 ? 0 : 1;
}
