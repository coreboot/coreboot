/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_SMSC_LPC47N217_LPC47N217_H
#define SUPERIO_SMSC_LPC47N217_LPC47N217_H

#include <device/pnp_type.h>
#include <stdint.h>

/*
 * These are arbitrary, but must match declarations in the mainboard
 * devicetree.cb file. Values chosen to match SMSC LPC47B37x.
 */
#define LPC47N217_PP               3   /* Parallel Port */
#define LPC47N217_SP1              4   /* Com1 */
#define LPC47N217_SP2              5   /* Com2 */

#define LPC47N217_MAX_CONFIG_REGISTER	0x39

void lpc47n217_enable_serial(pnp_devfn_t dev, u16 iobase);

#endif
