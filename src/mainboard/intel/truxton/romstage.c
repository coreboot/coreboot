/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <stdlib.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include "drivers/pc80/udelay_io.c"
#include <console/console.h>
#include "southbridge/intel/i3100/early_smbus.c"
#include "southbridge/intel/i3100/early_lpc.c"
#include <northbridge/intel/i3100/raminit_ep80579.h>
#include <superio/intel/i3100/i3100.h>
#include "cpu/x86/mtrr/earlymtrr.c"
#include "lib/debug.c" // XXX
#include <cpu/x86/bist.h>
#include <spd.h>

#define DEVPRES_CONFIG  (DEVPRES_D1F0 | DEVPRES_D2F0 | DEVPRES_D3F0 | DEVPRES_D4F0)

static inline int spd_read_byte(u16 device, u8 address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/intel/i3100/raminit_ep80579.c"
#include "lib/generic_sdram.c"

#define SERIAL_DEV PNP_DEV(0x4e, I3100_SP1)

#include <cpu/intel/romstage.h>
void main(unsigned long bist)
{
	static const struct mem_controller mch[] = {
		{
			.node_id = 0,
			.f0 = PCI_DEV(0, 0x00, 0),
			.channel0 = { DIMM2, DIMM3 },
		}
	};

	if (bist == 0) {
		/* Skip this if there was a built in self test failure */
		if (memory_initialized())
			return;
	}

	/* Set up the console */
	i3100_enable_superio();
	i3100_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	i3100_configure_uart_clk(SERIAL_DEV, I3100_UART_CLK_PREDIVIDE_26);

	console_init();

	/* Prevent the TCO timer from rebooting us */
	i3100_halt_tco_timer();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

#ifdef TRUXTON_DEBUG
	print_pci_devices();
#endif
	enable_smbus();

	sdram_initialize(ARRAY_SIZE(mch), mch);
	dump_pci_devices();
	dump_pci_device(PCI_DEV(0, 0x00, 0));
#ifdef TRUXTON_DEBUG
	dump_bar14(PCI_DEV(0, 0x00, 0));
#endif
}
