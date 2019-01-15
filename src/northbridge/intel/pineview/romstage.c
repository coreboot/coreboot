/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015  Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* Platform has no romstage entry point under mainboard directory,
 * so this one is named with prefix mainboard.
 */

#include <timestamp.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <cf9_reset.h>
#include <romstage_handoff.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <southbridge/intel/common/gpio.h>
#include <southbridge/intel/common/pmclib.h>
#include <cpu/intel/romstage.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/lapic.h>
#include "raminit.h"
#include "pineview.h"

static void rcba_config(void)
{
	/* Set up virtual channel 0 */
	RCBA32(0x0014) = 0x80000001;
	RCBA32(0x001c) = 0x03128010;

	/* Enable IOAPIC */
	RCBA8(OIC) = 0x03;
}

__weak void mb_pirq_setup(void)
{
}

#define LPC_DEV PCI_DEV(0x0, 0x1f, 0x0)

void mainboard_romstage_entry(unsigned long bist)
{
	u8 spd_addrmap[4] = {};
	int boot_path, cbmem_was_initted;
	int s3resume = 0;

	if (bist == 0)
		enable_lapic();

	/* Enable GPIOs */
	pci_write_config32(LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE | 1);
	pci_write_config8(LPC_DEV, GPIO_CNTL, 0x10);

	setup_pch_gpios(&mainboard_gpio_map);

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	enable_smbus();

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	pineview_early_initialization();

	post_code(0x30);

	s3resume = southbridge_detect_s3_resume();

	if (s3resume) {
		boot_path = BOOT_PATH_RESUME;
	} else {
		if (MCHBAR32(0xf14) & (1 << 8)) /* HOT RESET */
			boot_path = BOOT_PATH_RESET;
		else
			boot_path = BOOT_PATH_NORMAL;
	}

	get_mb_spd_addrmap(&spd_addrmap[0]);

	printk(BIOS_DEBUG, "Initializing memory\n");
	timestamp_add_now(TS_BEFORE_INITRAM);
	sdram_initialize(boot_path, spd_addrmap);
	timestamp_add_now(TS_AFTER_INITRAM);
	printk(BIOS_DEBUG, "Memory initialized\n");

	post_code(0x31);

	mb_pirq_setup();

	rcba_config();

	cbmem_was_initted = !cbmem_recovery(s3resume);

	if (!cbmem_was_initted && s3resume) {
		/* Failed S3 resume, reset to come up cleanly */
		system_reset();
	}

	romstage_handoff_init(s3resume);
}
