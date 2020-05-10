/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * A generic romstage (pre-ram) driver for various Winbond Super I/O chips.
 *
 * The following is derived directly from the vendor Winbond's data-sheets:
 *
 * To toggle between `configuration mode` and `normal operation mode` as to
 * manipulation the various LDN's in Winbond Super I/O's we are required to
 * pass magic numbers `passwords keys`.
 *
 *  WINBOUND_ENTRY_KEY :=  enable  configuration : 0x87
 *  WINBOUND_EXIT_KEY  :=  disable configuration : 0xAA
 *
 * To modify a LDN's configuration register, we use the index port to select
 * the index of the LDN and then write to the data port to alter the
 * parameters. A default index, data port pair is 0x4E, 0x4F respectively, a
 * user modified pair is 0x2E, 0x2F respectively.
 *
 */

#include <arch/io.h>
#include <device/pnp_ops.h>
#include <device/pnp.h>
#include <stdint.h>
#include "winbond.h"

#define WINBOND_ENTRY_KEY 0x87
#define WINBOND_EXIT_KEY 0xAA

/* Enable configuration: pass entry key '0x87' into index port dev. */
void pnp_enter_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(WINBOND_ENTRY_KEY, port);
	outb(WINBOND_ENTRY_KEY, port);
}

/* Disable configuration: pass exit key '0xAA' into index port dev. */
void pnp_exit_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(WINBOND_EXIT_KEY, port);
}

/* Bring up early serial debugging output before the RAM is initialized. */
void winbond_enable_serial(pnp_devfn_t dev, u16 iobase)
{
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_conf_state(dev);
}

void winbond_set_pinmux(pnp_devfn_t dev, uint8_t offset, uint8_t mask, uint8_t state)
{
	uint8_t byte;

	/* Configure pin mux */
	pnp_enter_conf_state(dev);
	byte = pnp_read_config(dev, offset);
	byte &= ~mask;
	byte |= state;
	pnp_write_config(dev, offset, byte);
	pnp_exit_conf_state(dev);
}

void winbond_set_clksel_48(pnp_devfn_t dev)
{
	u8 reg8;

	pnp_enter_conf_state(dev);
	reg8 = pnp_read_config(dev, 0x24);
	reg8 |= (1 << 6); /* Set the clock input to 48MHz. */
	pnp_write_config(dev, 0x24, reg8);
	pnp_exit_conf_state(dev);
}
