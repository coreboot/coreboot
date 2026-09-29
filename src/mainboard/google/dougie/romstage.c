/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <soc/aop_common.h>
#include <soc/cpucp.h>
#include <soc/qclib_common.h>
#include <soc/shrm.h>
#include <soc/usb/usb_common.h>
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
	/* Watchdog must be checked first to avoid erasing watchdog info later. */
	check_wdog();
	shrm_fw_load_reset();
	cpucp_prepare();
	/* QCLib: DDR init & train */
	qclib_load_and_run();
	aop_fw_load_reset();
	prepare_usb();
}
