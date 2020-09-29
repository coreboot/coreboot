/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/smm.h>

/* The X86 qemu target uses AMD64 save states but the APM port is not configurable. */
uint16_t pm_acpi_smi_cmd_port(void)
{
	return APM_CNT;
}
