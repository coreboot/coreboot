/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <soc/qclib_common.h>
#include <soc/watchdog.h>

void platform_romstage_main(void)
{
	/* Watchdog must be checked first to avoid erasing watchdog info later. */
	check_wdog();
	/* QCLib: DDR init & train */
	qclib_load_and_run();
}
