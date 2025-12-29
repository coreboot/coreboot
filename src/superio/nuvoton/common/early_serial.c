/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * A generic romstage (pre-ram) driver for Nuvoton variant Super I/O chips.
 */

#include <arch/io.h>
#include <device/pnp_ops.h>
#include <device/pnp.h>
#include <stdint.h>
#include "nuvoton.h"

/* Bring up early serial debugging output before the RAM is initialized. */
void nuvoton_enable_serial(pnp_devfn_t dev, u16 iobase)
{
	if (!CONFIG(CONSOLE_SERIAL))
		return;

	nuvoton_pnp_enter_conf_state(dev);

	if (CONFIG(SUPERIO_NUVOTON_COMMON_COM_A))
		/* Route COM A to GPIO8 pin group */
		pnp_unset_and_set_config(dev, 0x2a, 1 << 7, 0);

	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	nuvoton_pnp_exit_conf_state(dev);
}
