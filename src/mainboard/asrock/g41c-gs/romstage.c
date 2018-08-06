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
#include <northbridge/intel/x4x/x4x.h>
#include <cpu/x86/bist.h>
#include <cpu/intel/romstage.h>
#include <superio/nuvoton/nct6776/nct6776.h>
#include <superio/winbond/w83627dhg/w83627dhg.h>
#include <superio/winbond/common/winbond.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <lib.h>
#include <arch/stages.h>
#include <northbridge/intel/x4x/iomap.h>
#include <device/pnp_def.h>
#include <timestamp.h>

#define SERIAL_DEV_R2 PNP_DEV(0x2e, NCT6776_SP1)
#define SERIAL_DEV_R1 PNP_DEV(0x2e, W83627DHG_SP1)
#define SUPERIO_DEV PNP_DEV(0x2e, 0)
#define LPC_DEV PCI_DEV(0, 0x1f, 0)

static void mb_lpc_setup(void)
{
	u32 reg32;
	/* Set the value for GPIO base address register and enable GPIO. */
	pci_write_config32(LPC_DEV, GPIO_BASE, (DEFAULT_GPIOBASE | 1));
	pci_write_config8(LPC_DEV, GPIO_CNTL, 0x10);

	setup_pch_gpios(&mainboard_gpio_map);

	/* Set GPIOs on superio, enable UART */
	if (IS_ENABLED(CONFIG_SUPERIO_NUVOTON_NCT6776)) {
		nuvoton_pnp_enter_conf_state(SERIAL_DEV_R2);
		pnp_set_logical_device(SERIAL_DEV_R2);

		pnp_write_config(SERIAL_DEV_R2, 0x1c, 0x80);
		pnp_write_config(SERIAL_DEV_R2, 0x27, 0x80);
		pnp_write_config(SERIAL_DEV_R2, 0x2a, 0x60);

		nuvoton_pnp_exit_conf_state(SERIAL_DEV_R2);
		nuvoton_enable_serial(SERIAL_DEV_R2, CONFIG_TTYS0_BASE);
	} else {
		winbond_enable_serial(SERIAL_DEV_R1, CONFIG_TTYS0_BASE);
	}
	/* IRQ routing */
	RCBA16(D31IR) = 0x0132;
	RCBA16(D29IR) = 0x0237;

	/* Enable IOAPIC */
	RCBA8(OIC) = 0x03;
	RCBA8(OIC);

	reg32 = RCBA32(GCS);
	reg32 |= (1 << 5);
	RCBA32(GCS) = reg32;
	RCBA32(FD) = FD_PCIE6 | FD_PCIE5 | FD_PCIE4 | FD_PCIE3 | FD_ACMOD
		| FD_ACAUD | 1;
	RCBA32(CG) = 0x00000001;
}

static void ich7_enable_lpc(void)
{
	pci_write_config8(PCI_DEV(0, 0x1f, 0), SERIRQ_CNTL, 0xd0);
	/* Decode range */
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_EN, CNF1_LPC_EN
			| KBC_LPC_EN | LPT_LPC_EN | COMA_LPC_EN);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN1_DEC, 0x000c0291);
}

void mainboard_romstage_entry(unsigned long bist)
{
	//                          ch0      ch1
	const u8 spd_addrmap[4] = { 0x50, 0, 0x52, 0 };
	u8 boot_path = 0;
	u8 s3_resume;

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	/* Set southbridge and Super I/O GPIOs. */
	ich7_enable_lpc();
	mb_lpc_setup();

	console_init();

	report_bist_failure(bist);
	enable_smbus();

	x4x_early_init();

	s3_resume = southbridge_detect_s3_resume();
	if (s3_resume)
		boot_path = BOOT_PATH_RESUME;
	if (MCHBAR32(PMSTS_MCHBAR) & PMSTS_WARM_RESET)
		boot_path = BOOT_PATH_WARM_RESET;

	printk(BIOS_DEBUG, "Initializing memory\n");
	timestamp_add_now(TS_BEFORE_INITRAM);
	sdram_initialize(boot_path, spd_addrmap);
	timestamp_add_now(TS_AFTER_INITRAM);
	quick_ram_check();
	printk(BIOS_DEBUG, "Memory initialized\n");

	x4x_late_init(s3_resume);

	printk(BIOS_DEBUG, "x4x late init complete\n");

}
