/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <delay.h>
#include <stdint.h>
#include <device/pnp_ops.h>
#include <device/pci_ops.h>
#include <device/pnp_def.h>
#include <northbridge/intel/i945/i945.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <southbridge/intel/common/gpio.h>
#include "dock.h"

/* Override the default lpc decode ranges */
void mainboard_lpc_decode(void)
{
	// decode range
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_IO_DEC, 0x0210);
}

static void early_superio_config(void)
{
	int timeout = 100000;
	const pnp_devfn_t dev = PNP_DEV(0x2e, 3);

	pnp_write_config(dev, 0x29, 0xa0);

	while (!(pnp_read_config(dev, 0x29) & 0x10) && timeout--)
		udelay(1000);

	/* Enable COM1 */
	pnp_set_logical_device(dev);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x3f8);
	pnp_set_enable(dev, 1);
}

void bootblock_mainboard_early_init(void)
{
	/* Set up GPIO's early since it is needed for dock init */
	i82801gx_setup_bars();
	setup_pch_gpios(&mainboard_gpio_map);

	int dock_err = dlpc_init();

	/* We prefer Legacy I/O module over docking */
	if (legacy_io_present()) {
		legacy_io_init();
		early_superio_config();
	} else if (!dock_err && dock_present()) {
		dock_connect();
		early_superio_config();
	}
}

void mainboard_late_rcba_config(void)
{
	/* Device 1f interrupt pin register */
	RCBA32(D31IP) = 0x00001230;
	RCBA32(D29IP) = 0x40004321;

	/* PCIe Interrupts */
	RCBA32(D28IP) = 0x00004321;
	/* HD Audio Interrupt */
	RCBA32(D27IP) = 0x00000002;

	/* dev irq route register */
	RCBA16(D31IR) = 0x1007;
	RCBA16(D30IR) = 0x0076;
	RCBA16(D29IR) = 0x3210;
	RCBA16(D28IR) = 0x7654;
	RCBA16(D27IR) = 0x0010;

	/* Disable unused devices */
	RCBA32(FD) |= FD_INTLAN;

	/* Set up I/O Trap #3 for 0x800-0x80c (Trap) */
	RCBA64(IOTR3) = 0x000200f0000c0801ULL;
}

void mainboard_get_spd_map(u8 spd_map[4])
{
	spd_map[0] = 0x50;
	spd_map[2] = 0x51;
}
