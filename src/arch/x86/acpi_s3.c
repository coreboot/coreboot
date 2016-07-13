/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <cbmem.h>
#include <cpu/cpu.h>
#include <timestamp.h>
#include <romstage_handoff.h>
#include <rules.h>

#if ENV_RAMSTAGE

/* This is filled with acpi_is_wakeup() call early in ramstage. */
int acpi_slp_type = -1;

#if IS_ENABLED(CONFIG_EARLY_CBMEM_INIT)
int acpi_get_sleep_type(void)
{
	struct romstage_handoff *handoff;

	handoff = cbmem_find(CBMEM_ID_ROMSTAGE_INFO);

	if (handoff == NULL) {
		printk(BIOS_DEBUG, "Unknown boot method, assuming normal.\n");
		return ACPI_S0;
	} else if (handoff->s3_resume) {
		printk(BIOS_DEBUG, "S3 Resume.\n");
		return ACPI_S3;
	} else {
		printk(BIOS_DEBUG, "Normal boot.\n");
		return ACPI_S0;
	}
}
#endif

static void acpi_handoff_wakeup(void)
{
	if (acpi_slp_type < 0)
		acpi_slp_type = acpi_get_sleep_type();
}

int acpi_is_wakeup(void)
{
	acpi_handoff_wakeup();
	/* Both resume from S2 and resume from S3 restart at CPU reset */
	return (acpi_slp_type == ACPI_S3 || acpi_slp_type == ACPI_S2);
}

int acpi_is_wakeup_s3(void)
{
	acpi_handoff_wakeup();
	return (acpi_slp_type == ACPI_S3);
}

int acpi_is_wakeup_s4(void)
{
	acpi_handoff_wakeup();
	return (acpi_slp_type == ACPI_S4);
}

void acpi_fail_wakeup(void)
{
	if (acpi_slp_type == ACPI_S3 || acpi_slp_type == ACPI_S2)
		acpi_slp_type = ACPI_S0;
}
#endif /* ENV_RAMSTAGE */

void acpi_prepare_for_resume(void)
{
	if (!HIGH_MEMORY_SAVE)
		return;

	/* Back up the OS-controlled memory where ramstage will be loaded. */
	void *src = (void *)CONFIG_RAMBASE;
	void *dest = cbmem_find(CBMEM_ID_RESUME);
	if (dest != NULL)
		memcpy(dest, src, HIGH_MEMORY_SAVE);
}

void acpi_prepare_resume_backup(void)
{
	if (!acpi_s3_resume_allowed())
		return;

	/* Let's prepare the ACPI S3 Resume area now already, so we can rely on
	 * it being there during reboot time. We don't need the pointer, nor
	 * the result right now. If it fails, ACPI resume will be disabled.
	 */

	if (HIGH_MEMORY_SAVE)
		cbmem_add(CBMEM_ID_RESUME, HIGH_MEMORY_SAVE);
}

#define WAKEUP_BASE 0x600

void (*acpi_do_wakeup)(uintptr_t vector, u32 backup_source, u32 backup_target,
       u32 backup_size) asmlinkage = (void *)WAKEUP_BASE;

extern unsigned char __wakeup;
extern unsigned int __wakeup_size;

static void acpi_jump_to_wakeup(void *vector)
{
	uintptr_t acpi_backup_memory = 0;

	if (!acpi_s3_resume_allowed()) {
		printk(BIOS_WARNING, "ACPI: S3 resume not allowed.\n");
		return;
	}

	if (HIGH_MEMORY_SAVE) {
		acpi_backup_memory = (uintptr_t)cbmem_find(CBMEM_ID_RESUME);

		if (!acpi_backup_memory) {
			printk(BIOS_WARNING, "ACPI: Backup memory missing. "
				"No S3 resume.\n");
			return;
		}
	}

	/* Copy wakeup trampoline in place. */
	memcpy((void *)WAKEUP_BASE, &__wakeup, __wakeup_size);

	timestamp_add_now(TS_ACPI_WAKE_JUMP);

	acpi_do_wakeup((uintptr_t)vector, acpi_backup_memory, CONFIG_RAMBASE,
		       HIGH_MEMORY_SAVE);
}

void __attribute__((weak)) mainboard_suspend_resume(void)
{
}

void acpi_resume(void *wake_vec)
{
#if CONFIG_HAVE_SMI_HANDLER
	u32 *gnvs_address = cbmem_find(CBMEM_ID_ACPI_GNVS_PTR);

	/* Restore GNVS pointer in SMM if found */
	if (gnvs_address && *gnvs_address) {
		printk(BIOS_DEBUG, "Restore GNVS pointer to 0x%08x\n",
		       *gnvs_address);
		smm_setup_structures((void *)*gnvs_address, NULL, NULL);
	}
#endif

	/* Call mainboard resume handler first, if defined. */
	mainboard_suspend_resume();

	post_code(POST_OS_RESUME);
	acpi_jump_to_wakeup(wake_vec);
}
