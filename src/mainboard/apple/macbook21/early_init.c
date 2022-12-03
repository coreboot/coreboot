/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/intel/i945/i945.h>
#include <southbridge/intel/i82801gx/i82801gx.h>

void mainboard_late_rcba_config(void)
{
	/* Device 1f interrupt pin register */
	RCBA32(0x3100) = 0x00042210;
	RCBA32(0x3108) = 0x10004321;

	/* PCIe Interrupts */
	RCBA32(D28IP) = 0x00214321;
	/* HD Audio Interrupt */
	RCBA32(D27IP) = 0x00000001;

	/* dev irq route register */
	RCBA16(D31IR) = 0x0232;
	RCBA16(D30IR) = 0x3246;
	RCBA16(D29IR) = 0x0235;
	RCBA16(D28IR) = 0x3201;
	RCBA16(D27IR) = 0x3216;

	/* Disable unused devices */
	RCBA32(FD) |= FD_INTLAN;
}
