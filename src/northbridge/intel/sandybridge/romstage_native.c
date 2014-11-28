/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2014 Vladimir Serbinenko
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
#include <string.h>
#include <console/console.h>
#include <arch/io.h>
#include <lib.h>
#include <cpu/x86/lapic.h>
#include <timestamp.h>
#include "sandybridge.h"
#include <cpu/x86/bist.h>
#include <cpu/intel/romstage.h>
#include <halt.h>
#include "raminit_native.h"
#include "southbridge/intel/bd82x6x/pch.h"
#include "southbridge/intel/bd82x6x/gpio.h"

void main(unsigned long bist)
{
	int s3resume = 0;
	spd_raw_data spd[4];

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		outb(0x6, 0xcf9);
		halt ();
	}

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (bist == 0)
		enable_lapic();

	pch_enable_lpc();

	/* Enable GPIOs */
	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE|1);
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);

	setup_pch_gpios(&mainboard_gpio_map);

	early_usb_init(mainboard_usb_ports);

	/* Initialize console device(s) */
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	sandybridge_early_initialization(SANDYBRIDGE_MOBILE);
	printk(BIOS_DEBUG, "Back from sandybridge_early_initialization()\n");

	s3resume = southbridge_detect_s3_resume();

	post_code(0x38);
	/* Enable SPD ROMs and DDR-III DRAM */
	enable_smbus();

	post_code(0x39);

	post_code(0x3a);

	memset (spd, 0, sizeof (spd));
	mainboard_get_spd(spd);

	timestamp_add_now(TS_BEFORE_INITRAM);

	init_dram_ddr3(spd, 1, TCK_800MHZ, s3resume);

	timestamp_add_now(TS_AFTER_INITRAM);
	post_code(0x3c);

	southbridge_configure_default_intmap();
	rcba_config();
	post_code(0x3d);

	northbridge_romstage_finalize(s3resume);

	post_code(0x3f);
	timestamp_add_now(TS_END_ROMSTAGE);
}
