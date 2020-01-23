/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef SUPERIO_SMSC_LPC47N227_LPC47N227_H
#define SUPERIO_SMSC_LPC47N227_LPC47N227_H

#include <device/pnp_type.h>

/*
 * Since the LPC47N227 does not have logical devices but a flat configuration
 * space, these are arbitrary, but must match declarations in the mainboard
 * devicetree.cb.
 */
#define LPC47N227_PP               1	/* Parallel Port */
#define LPC47N227_SP1              2	/* COM1 */
#define LPC47N227_SP2              3	/* COM2 */
#define LPC47N227_KBDC             5	/* Keyboard */

#define LPC47N227_MAX_CONFIG_REGISTER	0x39

void pnp_enter_conf_state(pnp_devfn_t dev);
void pnp_exit_conf_state(pnp_devfn_t dev);

void lpc47n227_enable_serial(pnp_devfn_t dev, u16 iobase);

#endif
