/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <string.h>
#include <acpi/acpi.h>
#include <arch/cpu.h>
#include <commonlib/helpers.h>
#include <cpu/x86/smm.h>
#include <fallback.h>
#include <timestamp.h>

#define WAKEUP_BASE 0x600

asmlinkage void (*acpi_do_wakeup)(uintptr_t vector) = (void *)WAKEUP_BASE;

extern unsigned char __wakeup;
extern unsigned int __wakeup_size;

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
