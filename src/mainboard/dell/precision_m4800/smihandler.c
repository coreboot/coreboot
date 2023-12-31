/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/smm.h>
#include <ec/dell/mec5035/mec5035.h>
#include <stdint.h>

int mainboard_smi_apmc(u8 data)
{
	switch (data) {
	case APM_CNT_ACPI_ENABLE:
		mec5035_acpi_enable();
		break;
	default:
		break;
	}
	return 0;
}
