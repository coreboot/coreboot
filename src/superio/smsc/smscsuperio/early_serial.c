/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/io.h>
#include <device/pnp_ops.h>
#include <device/pnp_def.h>
#include <stdint.h>

#include "smscsuperio.h"

#define SMSC_ENTRY_KEY 0x55
#define SMSC_EXIT_KEY 0xAA

/* Enable configuration: pass entry key '0x87' into index port dev. */
static void pnp_enter_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(SMSC_ENTRY_KEY, port);
}

/* Disable configuration: pass exit key '0xAA' into index port dev. */
static void pnp_exit_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(SMSC_EXIT_KEY, port);
}


/**
 * Enable the specified serial port.
 *
 * @param dev The device to use.
 * @param iobase The I/O base of the serial port (usually 0x3f8/0x2f8).
 */
void smscsuperio_enable_serial(pnp_devfn_t dev, u16 iobase)
{
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	switch (iobase) {
	case 0x03f8:
		pnp_set_irq(dev, PNP_IDX_IRQ0, 4);
		break;
	case 0x02f8:
		pnp_set_irq(dev, PNP_IDX_IRQ0, 3);
		break;
	}
	pnp_set_enable(dev, 1);
	pnp_exit_conf_state(dev);
}
