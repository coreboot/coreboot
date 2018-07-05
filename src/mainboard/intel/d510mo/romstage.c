/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015  Damien Zammit <damien@zamaudio.com>
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
 */

#include <stdint.h>
#include <stdlib.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <console/console.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <southbridge/intel/common/gpio.h>
#include <northbridge/intel/pineview/raminit.h>
#include <northbridge/intel/pineview/pineview.h>
#include <cpu/x86/bist.h>
#include <cpu/intel/romstage.h>
#include <cpu/x86/lapic.h>
#include <superio/winbond/w83627thg/w83627thg.h>
#include <superio/winbond/common/winbond.h>
#include <lib.h>
#include <arch/stages.h>
#include <cbmem.h>
#include <romstage_handoff.h>
#include <timestamp.h>

#define SERIAL_DEV PNP_DEV(0x4e, W83627THG_SP1)
#define SUPERIO_DEV PNP_DEV(0x4e, 0)


/* Early mainboard specific GPIO setup */
static void mb_gpio_init(void)
{
	pci_devfn_t dev;

	/* Southbridge GPIOs. */
	dev = PCI_DEV(0x0, 0x1f, 0x0);

	/* Set the value for GPIO base address register and enable GPIO. */
	pci_write_config32(dev, GPIO_BASE, (DEFAULT_GPIOBASE | 1));
	pci_write_config8(dev, GPIO_CNTL, 0x10);

	setup_pch_gpios(&mainboard_gpio_map);
}

static void nm10_enable_lpc(void)
{
	/* Disable Serial IRQ */
	pci_write_config8(PCI_DEV(0, 0x1f, 0), SERIRQ_CNTL, 0x00);
	/* Decode range */
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_IO_DEC,
		pci_read_config16(PCI_DEV(0, 0x1f, 0), LPC_IO_DEC) | 0x0010);
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_EN, CNF1_LPC_EN
			| CNF2_LPC_EN | KBC_LPC_EN | COMA_LPC_EN
			| COMB_LPC_EN);

	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN2_DEC, 0x7c0291);
}

static void rcba_config(void)
{
	/* Set up virtual channel 0 */
	RCBA32(0x0014) = 0x80000001;
	RCBA32(0x001c) = 0x03128010;

	/* dev irq route register */
	RCBA16(D31IR) = 0x0132;
	RCBA16(D30IR) = 0x0146;
	RCBA16(D29IR) = 0x0237;
	RCBA16(D28IR) = 0x3201;
	RCBA16(D27IR) = 0x0146;

	/* Enable IOAPIC */
	RCBA8(OIC) = 0x03;

	RCBA32(FD) = FD_PCIE6 | FD_PCIE5 | FD_INTLAN | FD_ACMOD | FD_ACAUD
		| FD_PATA;
	RCBA32(FD) |= 1;
}

void mainboard_romstage_entry(unsigned long bist)
{
	const u8 spd_addrmap[4] = { 0x50, 0x51, 0, 0 };
	int cbmem_was_initted;
	int s3resume = 0;
	int boot_path;

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (bist == 0)
		enable_lapic();

	/* Disable watchdog timer */
	RCBA32(GCS) = RCBA32(GCS) | 0x20;

	/* Set southbridge and Super I/O GPIOs. */
	mb_gpio_init();

	nm10_enable_lpc();
	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	report_bist_failure(bist);
	enable_smbus();

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

	printk(BIOS_DEBUG, "Initializing memory\n");
	timestamp_add_now(TS_BEFORE_INITRAM);
	sdram_initialize(boot_path, spd_addrmap);
	timestamp_add_now(TS_AFTER_INITRAM);
	printk(BIOS_DEBUG, "Memory initialized\n");

	post_code(0x31);

	quick_ram_check();

	rcba_config();

	cbmem_was_initted = !cbmem_recovery(s3resume);

	if (!cbmem_was_initted && s3resume) {
		/* Failed S3 resume, reset to come up cleanly */
		outb(0x6, 0xcf9);
		halt();
	}

	romstage_handoff_init(s3resume);
}
