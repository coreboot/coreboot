/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/io.h>
#include <device/pnp.h>
#include <stdint.h>
#include "nct5104d.h"

#define NUVOTON_ENTRY_KEY 0x87
#define NUVOTON_EXIT_KEY 0xAA

/* Enable configuration: pass entry key '0x87' into index port dev
 * two times. */
static void pnp_enter_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(NUVOTON_ENTRY_KEY, port);
	outb(NUVOTON_ENTRY_KEY, port);
}

/* Disable configuration: pass exit key '0xAA' into index port dev. */
static void pnp_exit_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(NUVOTON_EXIT_KEY, port);
}

/* Route UARTD to pins 41-48 */
void nct5104d_enable_uartd(pnp_devfn_t dev)
{
	u8 tmp;
	u16 port = dev >> 8;
	pnp_enter_conf_state(dev);
	outb(0x1c, port);
	tmp = inb(port + 1);
	tmp |= 0x04;
	outb(tmp, port + 1);
	pnp_exit_conf_state(dev);
}
