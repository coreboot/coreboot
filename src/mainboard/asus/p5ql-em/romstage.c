/*
 * This file is part of the coreboot project.
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

#include <device/pci_ops.h>
#include <device/pnp_ops.h>
#include <console/console.h>
#include <southbridge/intel/i82801jx/i82801jx.h>
#include <southbridge/intel/common/pmclib.h>
#include <northbridge/intel/x4x/x4x.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/speedstep.h>
#include <arch/romstage.h>
#include <cf9_reset.h>
#include <superio/winbond/w83627dhg/w83627dhg.h>
#include <superio/winbond/common/winbond.h>

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

	/*
	 * P5QL-EM:
	 * BSEL0 -> not hooked up (not supported anyways)
	 * BSEL1 -> GPIO33 (inverted)
	 * BSEL2 -> GPIO40
	 */
	reg = 0x92;
	/* Multi-function Pin Selection */
	old_reg = pnp_read_config(GPIO_DEV, 0x2c);
	pnp_write_config(GPIO_DEV, 0x2c, reg);
	need_reset = (reg != old_reg);

	pnp_write_config(GPIO_DEV, 0x30, 0x0e); /* Enable GPIO3x,4x,5x */
	pnp_write_config(GPIO_DEV, 0xf0, 0xf3); /* GPIO3x direction */
	pnp_write_config(GPIO_DEV, 0xf2, 0x08); /* GPIO3x inversion */
	pnp_write_config(GPIO_DEV, 0xf4, 0x06); /* GPIO4x direction */

	const int gpio33 = (bsel & 2) >> 1;
	const int gpio40 = (bsel & 4) >> 2;
	reg = (gpio33 << 3);
	old_reg = pnp_read_config(GPIO_DEV, 0xf1); /* GPIO3x data */
	/* Set GPIO32 high like vendor firmware */
	pnp_write_config(GPIO_DEV, 0xf1, old_reg | reg | 4);
	need_reset += ((reg & 0x8) != (old_reg & 0x8));

	reg = gpio40;
	old_reg = pnp_read_config(GPIO_DEV, 0xf5); /* GPIO4x data */
	pnp_write_config(GPIO_DEV, 0xf5, old_reg | reg);
	need_reset += ((reg & 0x1) != (old_reg & 0x1));
	pnp_exit_ext_func_mode(GPIO_DEV);

	return need_reset;
}

void mainboard_romstage_entry(void)
{
	/* This board has first dimm slot of each channel hooked up to
	   rank0 and rank1, while the second dimm slot is only connected
	   to rank1. The raminit does not support such setups
	   const u8 spd_addrmap[4] = { 0x50, 0x51, 0x52, 0x53 }; */
	const u8 spd_addrmap[4] = { 0x50, 0, 0x52, 0 };
	u8 boot_path = 0;
	u8 s3_resume;

	/* Set southbridge and Super I/O GPIOs. */
	i82801jx_lpc_setup();
	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	console_init();

	enable_smbus();

	i82801jx_early_init();
	x4x_early_init();

	s3_resume = southbridge_detect_s3_resume();
	if (s3_resume)
		boot_path = BOOT_PATH_RESUME;
	if (MCHBAR32(PMSTS_MCHBAR) & PMSTS_WARM_RESET)
		boot_path = BOOT_PATH_WARM_RESET;

	if (!s3_resume && setup_sio_gpio()) {
		printk(BIOS_DEBUG, "Needs reset to configure CPU BSEL straps\n");
		full_reset();
	}

	sdram_initialize(boot_path, spd_addrmap);

	x4x_late_init(s3_resume);

	printk(BIOS_DEBUG, "x4x late init complete\n");
}
