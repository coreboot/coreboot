/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/smm.h>

/* default implementation of the !HAVE_CONFIGURABLE_APMC_SMI_PORT case */
uint16_t pm_acpi_smi_cmd_port(void)
{
	return APM_CNT;
}
