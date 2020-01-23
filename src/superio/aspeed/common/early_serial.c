/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

/*
 * A generic pre-ram driver for Aspeed variant Super I/O chips.
 *
 * The following is derived directly from the vendor Aspeed's data-sheets:
 *
 * To toggle between `configuration mode` and `normal operation mode` as to
 * manipulation the various LDN's in Aspeed Super I/O's we are required to
 * pass magic numbers `passwords keys`.
 *
 *  ASPEED_ENTRY_KEY :=  enable  configuration : 0xA5 (twice!)
 *  ASPEED_EXIT_KEY  :=  disable configuration : 0xAA
 *
 * To modify a LDN's configuration register, we use the index port to select
 * the index of the LDN and then writing to the data port to alter the
 * parameters. A default index, data port pair is 0x4E, 0x4F respectively, a
 * user modified pair is 0x2E, 0x2F respectively.
 *
 */

#include <arch/io.h>
#include <delay.h>
#include <device/pnp_def.h>
#include <device/pnp_ops.h>
#include <stdint.h>
#include "aspeed.h"

#define ASPEED_ENTRY_KEY 0xA5
#define ASPEED_EXIT_KEY 0xAA

/* Enable configuration: pass entry key '0xA5' into index port dev. */
void pnp_enter_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(ASPEED_ENTRY_KEY, port);
	outb(ASPEED_ENTRY_KEY, port);
}

/* Disable configuration: pass exit key '0xAA' into index port dev. */
void pnp_exit_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(ASPEED_EXIT_KEY, port);
}

/* Bring up early serial debugging output before the RAM is initialized. */
void aspeed_enable_serial(pnp_devfn_t dev, u16 iobase)
{
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_conf_state(dev);

	if (CONFIG(SUPERIO_ASPEED_USE_UART_DELAY_WORKAROUND))
		mdelay(500);
}
