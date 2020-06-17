/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>

static void set_smm_gnvs_ptr(void);

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
	case APM_CNT_GNVS_UPDATE:
		set_smm_gnvs_ptr();
		return 0;
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

static void set_smm_gnvs_ptr(void)
{
	uintptr_t gnvs_address;

	if (CONFIG(ACPI_NO_SMI_GNVS)) {
		printk(BIOS_WARNING, "%s() is not implemented\n", __func__);
		return;
	}

	gnvs_address = (uintptr_t)acpi_get_gnvs();
	if (!gnvs_address)
		return;

	/*
	 * Issue SMI to set the gnvs pointer in SMM.
	 *
	 * EAX = APM_CNT_GNVS_UPDATE
	 * EBX = gnvs pointer
	 * EDX = APM_CNT
	 */
	asm volatile (
		"outb %%al, %%dx\n\t"
		: /* ignore result */
		: "a" (APM_CNT_GNVS_UPDATE),
		  "b" (gnvs_address),
		  "d" (APM_CNT)
	);
}
