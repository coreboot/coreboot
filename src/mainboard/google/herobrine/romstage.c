/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <soc/qclib_common.h>

void platform_romstage_main(void)
{
	/* QCLib: DDR init & train */
	qclib_load_and_run();
}
