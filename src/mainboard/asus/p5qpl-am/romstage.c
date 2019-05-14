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

#include <cf9_reset.h>
#include <device/pnp_ops.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <cpu/intel/romstage.h>
#include <cpu/intel/speedstep.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/msr.h>
#include <northbridge/intel/x4x/iomap.h>
#include <northbridge/intel/x4x/x4x.h>
#include <southbridge/intel/common/gpio.h>
#include <southbridge/intel/common/pmclib.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83627dhg/w83627dhg.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83627DHG_SP1)
#define GPIO_DEV PNP_DEV(0x2e, W83627DHG_GPIO2345_V)
#define LPC_DEV PCI_DEV(0, 0x1f, 0)

static u8 msr_get_fsb(void)
{
	u8 fsbcfg;
	msr_t msr;
	const u32 eax = cpuid_eax(1);

	/* Netburst */
	if (((eax >> 8) & 0xf) == 0xf) {
		msr = rdmsr(MSR_EBC_FREQUENCY_ID);
		fsbcfg = (msr.lo >> 16) & 0x7;
	} else { /* Intel Core 2 */
		msr = rdmsr(MSR_FSB_FREQ);
		fsbcfg = msr.lo & 0x7;
	}

	return fsbcfg;
}

/* BSEL MCH straps are not hooked up to the CPU as usual but to the SIO */

static int setup_sio_gpio(void)
{
	int need_reset = 0;
	u8 reg, old_reg;

	u8 bsel = msr_get_fsb();
	switch (bsel) {
	case 0:
	case 2:
	case 4:
		break;
	default:
		printk(BIOS_WARNING,
		       "BSEL: Unsupported FSB frequency, using 800MHz\n");
		bsel = 2; /* 800MHz */
		break;
	}

	pnp_enter_ext_func_mode(GPIO_DEV);
	pnp_set_logical_device(GPIO_DEV);

	if (CONFIG(BOARD_ASUS_P5QPL_AM)) {
		/*
		 * P5QPL-AM:
		 * BSEL0 -> not hooked up (not supported anyways)
		 * BSEL1 -> GPIO33
		 * BSEL2 -> GPIO40
		 */
		reg = 0x92;
		old_reg = pnp_read_config(GPIO_DEV, 0x2c);
		pnp_write_config(GPIO_DEV, 0x2c, reg);
		need_reset = (reg != old_reg);

		pnp_write_config(GPIO_DEV, 0x30, 0x06);
		pnp_write_config(GPIO_DEV, 0xf0, 0xf3); /* GPIO3 direction */
		pnp_write_config(GPIO_DEV, 0xf4, 0x00); /* GPIO4 direction */

		const int gpio33 = (bsel & 2) >> 1;
		const int gpio40 = (bsel & 4) >> 2;
		reg = (gpio33 << 3);
		old_reg = pnp_read_config(GPIO_DEV, 0xf1);
		pnp_write_config(GPIO_DEV, 0xf1, old_reg | reg);
		need_reset += ((reg & 0x8) != (old_reg & 0x8));

		reg = gpio40;
		old_reg = pnp_read_config(GPIO_DEV, 0xf5);
		pnp_write_config(GPIO_DEV, 0xf5, old_reg | reg);
		need_reset += ((reg & 0x1) != (old_reg & 0x1));
	} else {
		/*
		 * P5G41T-M LX:
		 * BSEL0 -> not hooked up
		 * BSEL1 -> GPIO43 (inverted)
		 * BSEL2 -> GPIO44
		 */
		reg = 0xf2;
		old_reg = pnp_read_config(GPIO_DEV, 0x2c);
		pnp_write_config(GPIO_DEV, 0x2c, reg);
		need_reset = (reg != old_reg);
		pnp_write_config(GPIO_DEV, 0x30, 0x05);
		pnp_write_config(GPIO_DEV, 0xf6, 0x08); /* invert GPIO43 */
		pnp_write_config(GPIO_DEV, 0xf4, 0x00); /* GPIO4 direction */

		const int gpio43 = (bsel & 2) >> 1;
		const int gpio44 = (bsel & 4) >> 2;
		reg = (gpio43 << 3) | (gpio44 << 4);
		old_reg = pnp_read_config(GPIO_DEV, 0xf5);
		pnp_write_config(GPIO_DEV, 0xf5, old_reg | reg);
		need_reset += ((reg & 0x18) != (old_reg & 0x18));
	}
	pnp_exit_ext_func_mode(GPIO_DEV);

	return need_reset;
}

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
			   | FDD_LPC_EN | LPT_LPC_EN
			   | COMB_LPC_EN |  COMA_LPC_EN);
	/* IO decode range: HWM on 0x295 */
	pci_write_config32(LPC_DEV, 0x84, 0x000295);
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
	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	console_init();

	report_bist_failure(bist);
	enable_smbus();

	x4x_early_init();

	s3_resume = southbridge_detect_s3_resume();
	if (s3_resume)
		boot_path = BOOT_PATH_RESUME;
	if (MCHBAR32(PMSTS_MCHBAR) & PMSTS_WARM_RESET)
		boot_path = BOOT_PATH_WARM_RESET;

	if (!s3_resume && setup_sio_gpio()) {
		printk(BIOS_DEBUG,
		       "Needs reset to configure CPU BSEL straps\n");
		full_reset();
	}

	sdram_initialize(boot_path, spd_addrmap);

	x4x_late_init(s3_resume);

	printk(BIOS_DEBUG, "x4x late init complete\n");

}
