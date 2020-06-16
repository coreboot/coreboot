/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>

int apm_control(u8 cmd)
{
	if (!CONFIG(HAVE_SMI_HANDLER))
		return -1;

	switch (cmd) {
	case APM_CNT_CST_CONTROL:
		break;
	case APM_CNT_PST_CONTROL:
		break;
	case APM_CNT_ACPI_DISABLE:
		printk(BIOS_DEBUG, "Disabling ACPI via APMC.\n");
		break;
	case APM_CNT_ACPI_ENABLE:
		printk(BIOS_DEBUG, "Enabling ACPI via APMC.\n");
		break;
	case APM_CNT_FINALIZE:
		printk(BIOS_DEBUG, "Finalizing SMM.\n");
		break;
	case APM_CNT_ELOG_GSMI:
		break;
	case APM_CNT_SMMSTORE:
		break;
	case APM_CNT_SMMINFO:
		break;
	default:
		break;
	}

	/* Now raise the SMI. */
	outb(cmd, APM_CNT);

	printk(BIOS_DEBUG, "APMC done.\n");
	return 0;
}
