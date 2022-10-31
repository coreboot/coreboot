/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/speedstep.h>
#include <cpu/x86/msr.h>
#include <device/pnp_ops.h>
#include <northbridge/intel/i945/i945.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <stdint.h>
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83627dhg/w83627dhg.h>


#define SERIAL_DEV PNP_DEV(0x2e, W83627DHG_SP1)
#define GPIO_DEV PNP_DEV(0x2e, W83627DHG_GPIO2345_V)

/*
 * BSEL0 is connected with GPIO32
 * BSEL1 is connected with GPIO33 with inversed logic
 * BSEL2 is connected with GPIO55
 */
static int setup_sio_gpio(u8 bsel)
{
	int need_reset = 0;
	u8 reg, old_reg;

	pnp_enter_ext_func_mode(GPIO_DEV);
	pnp_set_logical_device(GPIO_DEV);

	reg = 0x9a;
	old_reg = pnp_read_config(GPIO_DEV, 0x2c);
	pnp_write_config(GPIO_DEV, 0x2c, reg);
	need_reset = (reg != old_reg);

	pnp_write_config(GPIO_DEV, 0x30, 0x0e);
	pnp_write_config(GPIO_DEV, 0xe0, 0xde);
	pnp_write_config(GPIO_DEV, 0xf0, 0xf3);
	pnp_write_config(GPIO_DEV, 0xf4, 0x80);
	pnp_write_config(GPIO_DEV, 0xf5, 0x80);

	/* Invert GPIO33 */
	pnp_write_config(GPIO_DEV, 0xf2, 0x08);

	reg = (bsel & 3) << 2;
	old_reg = pnp_read_config(GPIO_DEV, 0xf1);
	pnp_write_config(GPIO_DEV, 0xf1, reg);
	need_reset += ((reg & 0xc) != (old_reg & 0xc));

	reg = (bsel >> 2) << 5;
	old_reg = pnp_read_config(GPIO_DEV, 0xe1);
	pnp_write_config(GPIO_DEV, 0xe1, reg);
	need_reset += ((reg & 0x20) != (old_reg & 0x20));

	pnp_exit_ext_func_mode(GPIO_DEV);

	return need_reset;
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

void mainboard_late_rcba_config(void)
{
	/* Enable only PCIe Root Port Clock Gate */
	RCBA32(CG) = 0x00000001;
}

void mainboard_pre_raminit_config(int s3_resume)
{
	u8 c_bsel = msr_get_fsb();
	/*
	 * Result is that FSB is incorrect on s3 resume (fixed at 800MHz).
	 * Some CPU accept this others don't.
	 */
	if (!s3_resume && setup_sio_gpio(c_bsel)) {
		printk(BIOS_DEBUG,
			"Needs reset to configure CPU BSEL straps\n");
		full_reset();
	}
}

void bootblock_mainboard_early_init(void)
{
	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
