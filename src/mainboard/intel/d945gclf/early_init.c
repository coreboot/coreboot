/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <superio/smsc/lpc47m15x/lpc47m15x.h>
#include <northbridge/intel/i945/i945.h>
#include <southbridge/intel/i82801gx/i82801gx.h>

#define SERIAL_DEV PNP_DEV(0x2e, LPC47M15X_SP1)
#define PME_DEV PNP_DEV(0x2e, LPC47M15X_PME)

void mainboard_late_rcba_config(void)
{
	/* dev irq route register */
	RCBA16(D31IR) = 0x0132;
	RCBA16(D30IR) = 0x0146;
	RCBA16(D29IR) = 0x0237;
	RCBA16(D28IR) = 0x3201;
	RCBA16(D27IR) = 0x0146;

	/* Disable unused devices */
	RCBA32(FD) |= FD_INTLAN;
}

void bootblock_mainboard_early_init(void)
{
	/* Enable SuperIO PM */
	lpc47m15x_enable_serial(PME_DEV, 0x680);
	lpc47m15x_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE); /* 0x3f8 */
}
