/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/io.h>
#include <device/pnp_ops.h>
#include <device/pnp.h>
#include <stdint.h>
#include "pc87417.h"

void pc87417_disable_dev(pnp_devfn_t dev)
{
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
}

void pc87417_enable_dev(pnp_devfn_t dev)
{
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1);
}

void xbus_cfg(pnp_devfn_t dev)
{
	u8 i;
	u16 xbus_index;

	pnp_set_logical_device(dev);
	/* Select proper BIOS size (4MB). */
	pnp_write_config(dev, PC87417_XMEMCNF2,
			 (pnp_read_config(dev, PC87417_XMEMCNF2)) | 0x04);
	xbus_index = pnp_read_iobase(dev, PNP_IDX_IO0);

	/* Enable writes to devices attached to XCS0 (XBUS Chip Select 0). */
	for (i = 0; i <= 0xf; i++)
		outb((i << 4), xbus_index + PC87417_HAP0);
}
