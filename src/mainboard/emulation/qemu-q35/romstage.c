/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Stefan Reinauer
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <southbridge/intel/i82801ix/i82801ix.h>
#include <cpu/x86/bist.h>
#include <timestamp.h>
#include "drivers/pc80/udelay_io.c"
#include "lib/delay.c"
#include "cpu/x86/lapic.h"

#include "../qemu-i440fx/memory.c"

void main(unsigned long bist)
{
	int cbmem_was_initted;

	/* init_timer(); */
	post_code(0x05);

	i82801ix_early_init();
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	//print_pci_devices();
	//dump_pci_devices();

	cbmem_was_initted = !cbmem_initialize();

	timestamp_init(rdtsc());
	timestamp_add_now(TS_START_ROMSTAGE);

}
