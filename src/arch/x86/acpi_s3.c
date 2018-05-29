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
#include <compiler.h>
#include <cpu/cpu.h>
#include <fallback.h>
#include <timestamp.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <rules.h>
#include <symbols.h>

#if ENV_RAMSTAGE || ENV_POSTCAR

/* This is filled with acpi_is_wakeup() call early in ramstage. */
static int acpi_slp_type = -1;

static void acpi_handoff_wakeup(void)
{
	if (acpi_slp_type < 0) {
		if (romstage_handoff_is_resume()) {
			printk(BIOS_DEBUG, "S3 Resume.\n");
			acpi_slp_type = ACPI_S3;
		} else {
			printk(BIOS_DEBUG, "Normal boot.\n");
			acpi_slp_type = ACPI_S0;
		}
	}
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

struct resume_backup {
	uint64_t cbmem;
	uint64_t lowmem;
	uint64_t size;
	uint8_t valid;
};

#define BACKUP_PAGE_SZ 4096

static int backup_create_or_update(struct resume_backup *backup_mem,
				uintptr_t base, size_t size)
{
	uintptr_t top;

	if (IS_ENABLED(CONFIG_ACPI_HUGE_LOWMEM_BACKUP)) {
		base = CONFIG_RAMBASE;
		size = HIGH_MEMORY_SAVE;
	}

	/* Align backup region to complete pages. */
	top = ALIGN_UP(base + size, BACKUP_PAGE_SZ);
	base = ALIGN_DOWN(base, BACKUP_PAGE_SZ);
	size = top - base;

	/* Cannot extend existing region, should not happen. */
	if (backup_mem && (backup_mem->size < size))
		return -1;

	/* Allocate backup with room for header. */
	if (!backup_mem) {
		size_t header_sz = ALIGN_UP(sizeof(*backup_mem),
			BACKUP_PAGE_SZ);
		backup_mem = cbmem_add(CBMEM_ID_RESUME, header_sz + size);
		if (!backup_mem)
			return -1;

		/* Container starts from boundary after header. */
		backup_mem->cbmem = (uintptr_t)backup_mem + header_sz;
	}

	backup_mem->valid = 0;
	backup_mem->lowmem = base;
	backup_mem->size = size;
	return 0;
}

void *acpi_backup_container(uintptr_t base, size_t size)
{
	struct resume_backup *backup_mem = cbmem_find(CBMEM_ID_RESUME);
	if (!backup_mem)
		return NULL;

	if (!IS_ALIGNED(base, BACKUP_PAGE_SZ) || !IS_ALIGNED(size,
		BACKUP_PAGE_SZ))
		return NULL;

	if (backup_create_or_update(backup_mem, base, size) < 0)
		return NULL;

	backup_mem->valid = 1;
	return (void *)(uintptr_t)backup_mem->cbmem;
}

void backup_ramstage_section(uintptr_t base, size_t size)
{
	struct resume_backup *backup_mem = cbmem_find(CBMEM_ID_RESUME);

	/* For first boot we exit here as CBMEM_ID_RESUME is only
	 * created late in ramstage with acpi_prepare_resume_backup().
	 */
	if (!backup_mem)
		return;

	/* Check that the backup is not done twice. */
	if (backup_mem->valid)
		return;

	/* When we are called from ramstage loader, update header with
	 * properties of the ramstage we will load.
	 */
	if (backup_create_or_update(backup_mem, base, size) < 0)
		return;

	/* Back up the OS-controlled memory where ramstage will be loaded. */
	memcpy((void *)(uintptr_t)backup_mem->cbmem,
		(void *)(uintptr_t)backup_mem->lowmem,
		(size_t)backup_mem->size);
	backup_mem->valid = 1;
}

/* Let's prepare the ACPI S3 Resume area now already, so we can rely on
 * it being there during reboot time. If this fails, ACPI resume will
 * be disabled. We assume that ramstage does not change while in suspend,
 * so base and size of the currently running ramstage are used
 * for allocation.
 */
void acpi_prepare_resume_backup(void)
{
	if (!acpi_s3_resume_allowed())
		return;

	if (IS_ENABLED(CONFIG_RELOCATABLE_RAMSTAGE))
		return;

	backup_create_or_update(NULL, (uintptr_t)_program, _program_size);
}

#define WAKEUP_BASE 0x600

asmlinkage void (*acpi_do_wakeup)(uintptr_t vector, u32 backup_source,
	u32 backup_target, u32 backup_size) = (void *)WAKEUP_BASE;

extern unsigned char __wakeup;
extern unsigned int __wakeup_size;

static void acpi_jump_to_wakeup(void *vector)
{
	uintptr_t source = 0, target = 0;
	size_t size = 0;

	if (!acpi_s3_resume_allowed()) {
		printk(BIOS_WARNING, "ACPI: S3 resume not allowed.\n");
		return;
	}

	if (!IS_ENABLED(CONFIG_RELOCATABLE_RAMSTAGE)) {
		struct resume_backup *backup_mem = cbmem_find(CBMEM_ID_RESUME);
		if (backup_mem && backup_mem->valid) {
			backup_mem->valid = 0;
			target = backup_mem->lowmem;
			source = backup_mem->cbmem;
			size = backup_mem->size;
		} else  {
			printk(BIOS_WARNING, "ACPI: Backup memory missing. "
				"No S3 resume.\n");
			return;
		}
	}

	/* Copy wakeup trampoline in place. */
	memcpy((void *)WAKEUP_BASE, &__wakeup, __wakeup_size);

	set_boot_successful();

	timestamp_add_now(TS_ACPI_WAKE_JUMP);

	acpi_do_wakeup((uintptr_t)vector, source, target, size);
}

void __weak mainboard_suspend_resume(void)
{
}

void acpi_resume(void *wake_vec)
{
	if (IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)) {
		u32 *gnvs_address = cbmem_find(CBMEM_ID_ACPI_GNVS_PTR);

		/* Restore GNVS pointer in SMM if found */
		if (gnvs_address && *gnvs_address) {
			printk(BIOS_DEBUG, "Restore GNVS pointer to 0x%08x\n",
			       *gnvs_address);
			smm_setup_structures((void *)*gnvs_address, NULL, NULL);
		}
	}

	/* Call mainboard resume handler first, if defined. */
	mainboard_suspend_resume();

	post_code(POST_OS_RESUME);
	acpi_jump_to_wakeup(wake_vec);
}
