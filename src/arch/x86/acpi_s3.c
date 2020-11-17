/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <string.h>
#include <acpi/acpi.h>
#include <arch/cpu.h>
#include <commonlib/helpers.h>
#include <cpu/x86/smm.h>
#include <fallback.h>
#include <timestamp.h>
#include <romstage_handoff.h>

#if ENV_RAMSTAGE || ENV_POSTCAR

/* This is filled with acpi_is_wakeup_s3() call early in ramstage. */
static int acpi_slp_type = -1;

static void acpi_handoff_wakeup(void)
{
	if (acpi_slp_type < 0) {
		if (romstage_handoff_is_resume()) {
			printk(BIOS_DEBUG, "S3 Resume\n");
			acpi_slp_type = ACPI_S3;
		} else {
			printk(BIOS_DEBUG, "Normal boot\n");
			acpi_slp_type = ACPI_S0;
		}
	}
}

int acpi_is_wakeup_s3(void)
{
	acpi_handoff_wakeup();
	return (acpi_slp_type == ACPI_S3);
}

#endif /* ENV_RAMSTAGE */

#define WAKEUP_BASE 0x600

asmlinkage void (*acpi_do_wakeup)(uintptr_t vector) = (void *)WAKEUP_BASE;

extern unsigned char __wakeup;
extern unsigned int __wakeup_size;

void __weak mainboard_suspend_resume(void)
{
}

void __noreturn acpi_resume(void *wake_vec)
{
	/* Restore GNVS pointer in SMM if found. */
	apm_control(APM_CNT_GNVS_UPDATE);

	/* Call mainboard resume handler first, if defined. */
	mainboard_suspend_resume();

	/* Copy wakeup trampoline in place. */
	memcpy((void *)WAKEUP_BASE, &__wakeup, __wakeup_size);

	set_boot_successful();

	timestamp_add_now(TS_ACPI_WAKE_JUMP);

	post_code(POST_OS_RESUME);
	acpi_do_wakeup((uintptr_t)wake_vec);

	die("Failed the jump to wakeup vector\n");
}
