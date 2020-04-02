/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>

#define SUPERIO_GPIO PNP_DEV(0x2e, IT8728F_GPIO)
#define SERIAL_DEV PNP_DEV(0x2e, 0x01)

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 1 },
	{ 1, 0, 1 },
	{ 1, 0, 2 },
	{ 1, 0, 2 },
	{ 1, 0, 3 },
	{ 1, 0, 3 },
	{ 1, 0, 4 },
	{ 1, 0, 4 },
	{ 1, 0, 6 },
	{ 1, 0, 5 },
	{ 1, 0, 5 },
	{ 1, 0, 6 },
};

void bootblock_mainboard_early_init(void)
{
	if (!CONFIG(NO_UART_ON_SUPERIO)) {
		/* Enable serial port */
		ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	}

	/* Disable SIO WDT which kicks in DualBIOS */
	ite_reg_write(SUPERIO_GPIO, 0xEF, 0x7E);
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}
