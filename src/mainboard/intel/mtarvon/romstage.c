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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#define ASSEMBLY 1

#include <stdint.h>
#include <stdlib.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include "pc80/mc146818rtc_early.c"
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "lib/ramtest.c"
#include "southbridge/intel/i3100/i3100_early_smbus.c"
#include "southbridge/intel/i3100/i3100_early_lpc.c"
#include "northbridge/intel/i3100/raminit.h"
#include "superio/intel/i3100/i3100.h"
#include "cpu/x86/lapic/boot_cpu.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "superio/intel/i3100/i3100_early_serial.c"
#include "northbridge/intel/i3100/memory_initialized.c"
#include "cpu/x86/bist.h"

#define SIO_GPIO_BASE 0x680
#define SIO_XBUS_BASE 0x4880

#define DEVPRES_CONFIG  (DEVPRES_D1F0 | DEVPRES_D2F0)
#define DEVPRES1_CONFIG (DEVPRES1_D0F1 | DEVPRES1_D8F0)

static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
	/* nothing to do */
}
static inline int spd_read_byte(u16 device, u8 address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/intel/i3100/raminit.c"
#include "lib/generic_sdram.c"
#include "../jarrell/debug.c"


static void main(unsigned long bist)
{
	msr_t msr;
	u16 perf;
	static const struct mem_controller mch[] = {
		{
			.node_id = 0,
			.f0 = PCI_DEV(0, 0x00, 0),
			.f1 = PCI_DEV(0, 0x00, 1),
			.f2 = PCI_DEV(0, 0x00, 2),
			.f3 = PCI_DEV(0, 0x00, 3),
			.channel0 = { (0xa<<3)|3, (0xa<<3)|2, (0xa<<3)|1, (0xa<<3)|0 },
			.channel1 = { (0xa<<3)|7, (0xa<<3)|6, (0xa<<3)|5, (0xa<<3)|4 },
		}
	};

	if (bist == 0) {
		/* Skip this if there was a built in self test failure */
		early_mtrr_init();
		if (memory_initialized()) {
			asm volatile ("jmp __cpu_reset");
		}
	}
	/* Set up the console */
	i3100_enable_superio();
	i3100_enable_serial(0x4e, I3100_SP1, CONFIG_TTYS0_BASE);
	uart_init();
	console_init();

	/* Prevent the TCO timer from rebooting us */
	i3100_halt_tco_timer();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	/* print_pci_devices(); */
	enable_smbus();
	/* dump_spd_registers(); */

	/* Enable SpeedStep and automatic thermal throttling */
	/* FIXME: move to Pentium M init code */
	msr = rdmsr(0x1a0);
	msr.lo |= (1 << 3) | (1 << 16);
	wrmsr(0x1a0, msr);
	msr = rdmsr(0x19d);
	msr.lo |= (1 << 16);
	wrmsr(0x19d, msr);

	/* Set CPU frequency/voltage to maximum */
	/* FIXME: move to Pentium M init code */
	msr = rdmsr(0x198);
	perf = msr.hi & 0xffff;
	msr = rdmsr(0x199);
	msr.lo &= 0xffff0000;
	msr.lo |= perf;
	wrmsr(0x199, msr);

	sdram_initialize(ARRAY_SIZE(mch), mch);
	/* dump_pci_devices(); */
	/* dump_pci_device(PCI_DEV(0, 0x00, 0)); */
	/* dump_bar14(PCI_DEV(0, 0x00, 0)); */

	ram_check(0, 1024 * 1024);
}
