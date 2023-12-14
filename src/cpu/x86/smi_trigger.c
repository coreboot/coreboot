/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <stdint.h>

static void apmc_log(const char *fn, u8 cmd)
{
	switch (cmd) {
	case APM_CNT_ACPI_DISABLE:
		printk(BIOS_DEBUG, "%s: Disabling ACPI.\n", fn);
		break;
	case APM_CNT_ACPI_ENABLE:
		printk(BIOS_DEBUG, "%s: Enabling ACPI.\n", fn);
		break;
	case APM_CNT_FINALIZE:
		printk(BIOS_DEBUG, "%s: Finalizing SMM.\n", fn);
		break;
	case APM_CNT_ELOG_GSMI:
		break;
	case APM_CNT_SMMSTORE:
		break;
	case APM_CNT_SMMINFO:
		break;
	default:
		printk(BIOS_DEBUG, "%s: Unknown APMC 0x%02x.\n", fn, cmd);
		break;
	}
}

int apm_control(u8 cmd)
{
	/* Never proceed inside SMI handler or without one. */
	if (ENV_SMM || !CONFIG(HAVE_SMI_HANDLER))
		return -1;

	apmc_log(__func__, cmd);

	/* Now raise the SMI. */
	outb(cmd, pm_acpi_smi_cmd_port());

	printk(BIOS_DEBUG, "APMC done.\n");
	return 0;
}

u8 apm_get_apmc(void)
{
	/* Read command byte from APMC SMI IO port */
	u8 cmd = inb(pm_acpi_smi_cmd_port());

	apmc_log("SMI#", cmd);
	return cmd;
}
