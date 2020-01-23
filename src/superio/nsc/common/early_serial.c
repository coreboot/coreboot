/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#include <device/pnp_ops.h>
#include <device/pnp_def.h>
#include <stdint.h>
#include "nsc.h"

void nsc_enable_serial(pnp_devfn_t dev, u16 iobase)
{
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
}
