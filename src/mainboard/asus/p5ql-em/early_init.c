/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <console/console.h>
#include <northbridge/intel/x4x/x4x.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/speedstep.h>
#include <cf9_reset.h>
#include <superio/winbond/w83627dhg/w83627dhg.h>
#include <superio/winbond/common/winbond.h>

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

void mb_pre_raminit_setup(int s3_resume)
{
	if (!s3_resume && setup_sio_gpio()) {
		printk(BIOS_DEBUG, "Needs reset to configure CPU BSEL straps\n");
		full_reset();
	}
}

void mb_get_spd_map(u8 spd_map[4])
{
	/* This board has first dimm slot of each channel hooked up to
	   rank0 and rank1, while the second dimm slot is only connected
	   to rank1. The raminit does not support such setups. So only the
	   first dimms of each channel are used. */
	spd_map[0] = 0x50;
	spd_map[2] = 0x52;
}
