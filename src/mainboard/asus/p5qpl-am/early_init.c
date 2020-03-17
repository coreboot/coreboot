/*
 * This file is part of the coreboot project.
 *
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

#include <bootblock_common.h>
#include <cf9_reset.h>
#include <device/pnp_ops.h>
#include <console/console.h>
#include <cpu/intel/speedstep.h>
#include <cpu/x86/msr.h>
#include <northbridge/intel/x4x/x4x.h>
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83627dhg/w83627dhg.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83627DHG_SP1)
#define GPIO_DEV PNP_DEV(0x2e, W83627DHG_GPIO2345_V)

void bootblock_mainboard_early_init(void)
{
	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

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
		pnp_write_config(GPIO_DEV, 0xf4, 0xa4); /* GPIO4 direction */

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

void mb_pre_raminit_setup(int s3_resume)
{
	if (!s3_resume && setup_sio_gpio()) {
		printk(BIOS_DEBUG, "Needs reset to configure CPU BSEL straps\n");
		full_reset();
	}
}

void mb_get_spd_map(u8 spd_map[4])
{
	spd_map[0] = 0x50;
	spd_map[2] = 0x52;
}
