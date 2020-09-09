/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <soc/usb.h>
#include <soc/qclib_common.h>
#include <soc/watchdog.h>

static void prepare_usb(void)
{
	/*
	 * Do DWC3 core and phy reset. Kick these resets
	 * off early so they get at least 1ms to settle.
	 */
	reset_usb0();
}

void platform_romstage_main(void)
{
	prepare_usb();

	check_wdog();

	/* QCLib: DDR init & train */
	qclib_load_and_run();
}
