/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
 * Copyright (C) 2017 Arthur Heymans <arthur@aheymans.xyz>
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

#include <console/console.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <southbridge/intel/common/gpio.h>
#include <southbridge/intel/common/pmclib.h>
#include <northbridge/intel/x4x/x4x.h>
#include <cpu/x86/bist.h>
#include <cpu/intel/romstage.h>
#include <device/pci_ops.h>
#include <superio/smsc/smscsuperio/smscsuperio.h>
#include <northbridge/intel/x4x/iomap.h>

#define SERIAL_DEV PNP_DEV(0x2e, SMSCSUPERIO_SP1)
#define LPC_DEV PCI_DEV(0, 0x1f, 0)

static void mb_lpc_setup(void)
{
	u32 reg32;
	/* Set the value for GPIO base address register and enable GPIO. */
	pci_write_config32(LPC_DEV, GPIO_BASE, (DEFAULT_GPIOBASE | 1));
	pci_write_config8(LPC_DEV, GPIO_CNTL, 0x10);

	setup_pch_gpios(&mainboard_gpio_map);

	/* Enable IOAPIC */
	RCBA8(0x31ff) = 0x03;
	RCBA8(0x31ff);

	reg32 = RCBA32(GCS);
	reg32 |= (1 << 5);
	RCBA32(GCS) = reg32;
	RCBA32(CG) = 0x00000001;
}

static void ich7_enable_lpc(void)
{
	pci_write_config8(LPC_DEV, SERIRQ_CNTL, 0xd0);
	/* Fixed IO decode ranges */
	pci_write_config16(LPC_DEV, LPC_IO_DEC, 0x0010);
	/* LPC enable devices */
	pci_write_config16(LPC_DEV, LPC_EN, CNF1_LPC_EN | KBC_LPC_EN
			   | FDD_LPC_EN | LPT_LPC_EN |  COMA_LPC_EN);
	/* IO decode range: HWM on 0xa00 */
	pci_write_config32(LPC_DEV, GEN1_DEC, 0x00fc0a01);
}

void mainboard_romstage_entry(unsigned long bist)
{
	//                          ch0      ch1
	const u8 spd_addrmap[4] = { 0x50, 0, 0x52, 0 };
	u8 boot_path = 0;
	u8 s3_resume;

	/* Set southbridge and Super I/O GPIOs. */
	ich7_enable_lpc();
	mb_lpc_setup();
	smscsuperio_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	console_init();

	report_bist_failure(bist);
	enable_smbus();

	x4x_early_init();

	s3_resume = southbridge_detect_s3_resume();
	if (s3_resume)
		boot_path = BOOT_PATH_RESUME;
	if (MCHBAR32(PMSTS_MCHBAR) & PMSTS_WARM_RESET)
		boot_path = BOOT_PATH_WARM_RESET;

	sdram_initialize(boot_path, spd_addrmap);

	x4x_late_init(s3_resume);

	printk(BIOS_DEBUG, "x4x late init complete\n");

}
