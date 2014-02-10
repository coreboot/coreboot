/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <console/console.h>
#include <stdlib.h>

#include "chip.h"
#include "f71869ad.h"

static void f71869ad_init(device_t dev)
{
	struct superio_fintek_f71869ad_config *conf = dev->chip_info;

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	/* TODO: Might potentially need code for HWM or FDC etc. */
	case F71869AD_KBC:
		pc_keyboard_init(&conf->keyboard);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = f71869ad_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

/*
 * io_info contains the mask 0x07f8. Given 16 register, each 8 bits wide of a
 * logical device we need a mask of the following form:
 *
 *  MSB                 LSB
 *    v                 v
 * 0x[15..11][10..3][2..0]
 *    ------  ^^^^^  ^^^^
 *    null      |      |
 *              |      +------ Register index
 *              |
 *              +------------- Compare against base address and
 *                             asserts a chip_select on match.
 *
 *  i.e., 0x07F8 = [00000][11111111][000]
 *
 * NOTE: Be sure to set these in your devicetree.cb, i.e.
 *
 *			chip superio/fintek/f71869ad
 *				device pnp 4e.00 off        # Floppy
 *					io 0x60 = 0x3f0
 *					irq 0x70 = 6
 *					drq 0x74 = 2
 *				end
 *				device pnp 4e.01 on         # COM1
 *					io 0x60 = 0x3f8
 *					irq 0x70 = 4
 *				end
 *					device pnp 4e.02 off    # COM2
 *					io 0x60 = 0x2f8
 *					irq 0x70 = 3
 *				end
 *				device pnp 4e.03 off        # Parallel Port
 *					io 0x60 = 0x378
 *					irq 0x70 = 7
 *					drq 0x74 = 3
 *				end
 *				device pnp 4e.04 on         # Hardware Monitor
 *					io 0x60 = 0x295
 *					irq 0x70 = 0
 *				end
 *				device pnp 4e.05 on         # KBC
 *					io 0x60 = 0x060
 *					irq 0x70 = 1            # Keyboard IRQ
 *					irq 0x72 = 12           # Mouse IRQ
 *				end
 *				device pnp 4e.06 off end    # GPIO
 *				device pnp 4e.07 on end     # BSEL
 *				device pnp 4e.0a off end    # PME
 *			end # f71869ad
 *
 */
static struct pnp_info pnp_dev_info[] = {
	{ &ops, F71869AD_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, F71869AD_SP1,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, F71869AD_SP2,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, F71869AD_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, F71869AD_HWM,  PNP_IO0 | PNP_IRQ0, {0x0ff8, 0}, },
	{ &ops, F71869AD_KBC,  PNP_IO0 | PNP_IRQ0 | PNP_IRQ1, {0x07ff, 0}, },
	{ &ops, F71869AD_GPIO, },
	{ &ops, F71869AD_BSEL,  PNP_IO0, {0x07f8, 0}, },
	{ &ops, F71869AD_PME, },
};

static void enable_dev(device_t dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_fintek_f71869ad_ops = {
	CHIP_NAME("Fintek F71869AD Super I/O")
	.enable_dev = enable_dev
};
