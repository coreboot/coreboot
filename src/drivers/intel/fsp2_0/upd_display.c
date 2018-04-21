/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <arch/cpu.h>
#include <compiler.h>
#include <console/console.h>
#include <fsp/util.h>
#include <lib.h>

void fsp_display_upd_value(const char *name, size_t size, uint64_t old,
	uint64_t new)
{
	size *= 2;
	if (old == new) {
		printk(BIOS_SPEW, "  0x%0*llx: %s\n", (int)size, new, name);
	} else {
		printk(BIOS_SPEW, "  0x%0*llx --> 0x%0*llx: %s\n", (int)size,
			old, (int)size, new, name);
	}
}

static void fspm_display_arch_params(const FSPM_ARCH_UPD *old,
	const FSPM_ARCH_UPD *new)
{
	/* Display the architectural parameters for MemoryInit */
	printk(BIOS_SPEW, "Architectural UPD values for MemoryInit at: 0x%p\n",
		new);
	fsp_display_upd_value("Revision", sizeof(old->Revision),
		old->Revision, new->Revision);
	fsp_display_upd_value("NvsBufferPtr", sizeof(old->NvsBufferPtr),
		(uintptr_t)old->NvsBufferPtr,
		(uintptr_t)new->NvsBufferPtr);
	fsp_display_upd_value("StackBase", sizeof(old->StackBase),
		(uintptr_t)old->StackBase,
		(uintptr_t)new->StackBase);
	fsp_display_upd_value("StackSize", sizeof(old->StackSize),
		old->StackSize, new->StackSize);
	fsp_display_upd_value("BootLoaderTolumSize",
		sizeof(old->BootLoaderTolumSize),
		old->BootLoaderTolumSize, new->BootLoaderTolumSize);
	fsp_display_upd_value("BootMode", sizeof(old->BootMode),
		old->BootMode, new->BootMode);
}

/* Display the UPD parameters for MemoryInit */
__weak void soc_display_fspm_upd_params(
	const FSPM_UPD *fspm_old_upd,
	const FSPM_UPD *fspm_new_upd)
{
	printk(BIOS_SPEW, "UPD values for MemoryInit:\n");
	hexdump(fspm_new_upd, sizeof(*fspm_new_upd));
}

void fspm_display_upd_values(const FSPM_UPD *old,
	const FSPM_UPD *new)
{
	/* Display the UPD data */
	fspm_display_arch_params(&old->FspmArchUpd, &new->FspmArchUpd);
	soc_display_fspm_upd_params(old, new);
}

/* Display the UPD parameters for SiliconInit */
__weak void soc_display_fsps_upd_params(
	const FSPS_UPD *fsps_old_upd,
	const FSPS_UPD *fsps_new_upd)
{
	printk(BIOS_SPEW, "UPD values for SiliconInit:\n");
	hexdump(fsps_new_upd, sizeof(*fsps_new_upd));
}
