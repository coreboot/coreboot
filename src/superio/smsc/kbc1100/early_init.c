/* SPDX-License-Identifier: GPL-2.0-only */

/* Pre-RAM driver for the SMSC KBC1100 Super I/O chip */

#include <arch/io.h>
#include <device/pnp_ops.h>
#include <device/pnp.h>
#include <stdint.h>

#include "kbc1100.h"

static void pnp_enter_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(0x55, port);
}

static void pnp_exit_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(0xaa, port);
}

void kbc1100_early_serial(pnp_devfn_t dev, u16 iobase)
{
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_conf_state(dev);
}

void kbc1100_early_init(u16 port)
{
	pnp_devfn_t dev;
	dev = PNP_DEV(port, KBC1100_KBC);
	pnp_enter_conf_state(dev);

	/* Serial IRQ enabled */
	outb(0x25, port);
	outb(0x04, port + 1);

	/* Enable keyboard */
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x60);
	pnp_set_iobase(dev, PNP_IDX_IO1, 0x64);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 1);   /* IRQ 1 */
	pnp_set_irq(dev, PNP_IDX_IRQ1, 12);   /* IRQ 12 */
	pnp_set_enable(dev, 1);

	/* Enable EC Channel 0 */
	dev = PNP_DEV(port, KBC1100_EC0);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1);

	pnp_exit_conf_state(dev);

	/* disable the 1s timer */
	outb(0xE7, 0x64);
}
