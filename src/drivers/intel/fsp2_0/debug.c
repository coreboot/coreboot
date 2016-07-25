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

#include <console/console.h>
#include <fsp/util.h>

/*-----------
 * MemoryInit
 *-----------
 */
void fsp_debug_before_memory_init(fsp_memory_init_fn memory_init,
	const struct FSPM_UPD *fspm_old_upd,
	const struct FSPM_UPD *fspm_new_upd, void **hob_list_ptr)
{
	/* Display the call entry point and paramters */
	if (!IS_ENABLED(CONFIG_DISPLAY_FSP_CALLS_AND_STATUS))
		return;
	printk(BIOS_DEBUG, "Calling FspMemoryInit: 0x%p\n", memory_init);
	printk(BIOS_SPEW, "\t0x%p: raminit_upd\n", fspm_new_upd);
	printk(BIOS_SPEW, "\t0x%p: &hob_list_ptr\n", hob_list_ptr);
}

void fsp_debug_after_memory_init(enum fsp_status status,
	const struct hob_header *hob_list_ptr)
{
	if (IS_ENABLED(CONFIG_DISPLAY_FSP_CALLS_AND_STATUS))
		printk(BIOS_DEBUG, "FspMemoryInit returned 0x%08x\n", status);
}

/*-----------
 * SiliconInit
 *-----------
 */
void fsp_debug_before_silicon_init(fsp_silicon_init_fn silicon_init,
	const struct FSPS_UPD *fsps_old_upd,
	const struct FSPS_UPD *fsps_new_upd)
{
	/* Display the call to FSP SiliconInit */
	if (!IS_ENABLED(CONFIG_DISPLAY_FSP_CALLS_AND_STATUS))
		return;
	printk(BIOS_SPEW, "Calling FspSiliconInit: 0x%p\n", silicon_init);
	printk(BIOS_SPEW, "\t0x%p: upd\n", fsps_new_upd);
}

void fsp_debug_after_silicon_init(enum fsp_status status)
{
	if (IS_ENABLED(CONFIG_DISPLAY_FSP_CALLS_AND_STATUS))
		printk(BIOS_SPEW, "FspSiliconInit returned 0x%08x\n", status);
}

/*-----------
 * FspNotify
 *-----------
 */
void fsp_before_debug_notify(fsp_notify_fn notify,
	const struct fsp_notify_params *notify_params)
{
	/* Display the call to FSP SiliconInit */
	if (!IS_ENABLED(CONFIG_DISPLAY_FSP_CALLS_AND_STATUS))
		return;
	printk(BIOS_SPEW, "0x%08x: notify_params->phase\n",
		notify_params->phase);
	printk(BIOS_SPEW, "Calling FspNotify: 0x%p\n", notify);
	printk(BIOS_SPEW, "\t0x%p: notify_params\n", notify_params);
}

void fsp_debug_after_notify(enum fsp_status status)
{
	if (IS_ENABLED(CONFIG_DISPLAY_FSP_CALLS_AND_STATUS))
		printk(BIOS_SPEW, "FspNotify returned 0x%08x\n", status);
}
