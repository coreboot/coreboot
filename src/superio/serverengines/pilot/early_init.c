/* SPDX-License-Identifier: GPL-2.0-or-later */

/* PILOT Super I/O is only based on LPC observation done on factory system. */

#define BLUBB_DEV PNP_DEV(port, 0x04)

#include <device/pnp_ops.h>
#include <console/console.h>
#include <device/pnp.h>
#include <device/pnp_def.h>
#include "pilot.h"

/*
 * Logical device 4, 5 and 7 are being deactivated. Logical Device 1 seems to
 * be another serial (?), it is also deactivated on the HP machine.
 */
void pilot_early_init(pnp_devfn_t dev)
{
	u16 port = dev >> 8;

	printk(BIOS_DEBUG, "Using port: %04x\n", port);
	pilot_disable_serial(PNP_DEV(port, 0x1));
	printk(BIOS_DEBUG, "disable serial 1\n");

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x3));
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x0b00);
	pnp_set_iobase(dev, PNP_IDX_IO1, 0x0b80);
	pnp_set_iobase(dev, PNP_IDX_IO2, 0x0b84);
	pnp_set_iobase(dev, PNP_IDX_IO3, 0x0b86);
	pnp_set_enable(dev, 1);
	pnp_exit_ext_func_mode(dev);

/*
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x3));
	pnp_exit_ext_func_mode(dev);
	pnp_enter_ext_func_mode(dev);
	pnp_set_enable(PNP_DEV(port, 0x3), 0);
	pnp_exit_ext_func_mode(dev);
*/

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x4));
	pnp_exit_ext_func_mode(dev);
	pnp_enter_ext_func_mode(dev);
	pnp_set_enable(PNP_DEV(port, 0x4), 0);
	pnp_exit_ext_func_mode(dev);

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x5));
	pnp_exit_ext_func_mode(dev);
	pnp_enter_ext_func_mode(dev);
	pnp_set_enable(PNP_DEV(port, 0x5), 0);
	pnp_exit_ext_func_mode(dev);

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x6));
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x60);
	pnp_set_iobase(dev, PNP_IDX_IO1, 0x64);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 1);
	pnp_set_drq(dev, 0x71, 3);
	pnp_set_enable(dev, 0);
	pnp_exit_ext_func_mode(dev);

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0xe));
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x70);
	pnp_set_iobase(dev, PNP_IDX_IO1, 0x72);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 8);
	pnp_set_drq(dev, 0x71, 3);
	pnp_set_enable(dev, 0);
	pnp_exit_ext_func_mode(dev);

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x7));
	pnp_exit_ext_func_mode(dev);
	pnp_enter_ext_func_mode(dev);
	pnp_set_enable(PNP_DEV(port, 0x7), 0);
	pnp_exit_ext_func_mode(dev);

/*
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x8));
	pnp_exit_ext_func_mode(dev);
	pnp_enter_ext_func_mode(dev);
	pnp_set_enable(PNP_DEV(port, 0x8), 0);
	pnp_exit_ext_func_mode(dev);

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x9));
	pnp_exit_ext_func_mode(dev);
	pnp_enter_ext_func_mode(dev);
	pnp_set_enable(PNP_DEV(port, 0x9), 0);
	pnp_exit_ext_func_mode(dev);

	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(PNP_DEV(port, 0x10));
	pnp_exit_ext_func_mode(dev);
	pnp_enter_ext_func_mode(dev);
	pnp_set_enable(PNP_DEV(port, 0x10), 0);
	pnp_exit_ext_func_mode(dev);
*/
}
