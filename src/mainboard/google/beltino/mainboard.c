/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cpu/x86/smm.h>

void mainboard_suspend_resume(void)
{
	/* Call SMM finalize() handlers before resume */
	apm_control(APM_CNT_FINALIZE);
}
