/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <soc/cpucp.h>
#include <soc/qclib_common.h>
#include <soc/shrm.h>
#include <soc/usb/usb_common.h>
#include <soc/watchdog.h>
#include "board.h"

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
	prepare_usb();
	/* This rail needs to be stable by the time we take the FPMCU out of
	   reset in ramstage, so already turn it on here. This needs to happen
	   at least 200ms after this pin was first driven low in the bootblock. */
	if (CONFIG(HEROBRINE_HAS_FINGERPRINT))
		gpio_output(GPIO_EN_FP_RAILS, 1);
}
