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
#include <console/streams.h>
#include <fsp/util.h>
#include <soc/intel/common/util.h>

asmlinkage size_t fsp_write_line(uint8_t *buffer, size_t number_of_bytes)
{
	console_write_line(buffer, number_of_bytes);
	return number_of_bytes;
}

/*-----------
 * MemoryInit
 *-----------
 */
void fsp_debug_before_memory_init(fsp_memory_init_fn memory_init,
	const FSPM_UPD *fspm_old_upd,
	const FSPM_UPD *fspm_new_upd)
{
	/* Display the MTRRs */
	if (IS_ENABLED(CONFIG_DISPLAY_MTRRS))
		soc_display_mtrrs();

	/* Display the UPD values */
	if (IS_ENABLED(CONFIG_DISPLAY_UPD_DATA))
		fspm_display_upd_values(fspm_old_upd, fspm_new_upd);

	/* Display the call entry point and parameters */
	if (!IS_ENABLED(CONFIG_DISPLAY_FSP_CALLS_AND_STATUS))
		return;
	printk(BIOS_SPEW, "Calling FspMemoryInit: 0x%p\n", memory_init);
	printk(BIOS_SPEW, "\t0x%p: raminit_upd\n", fspm_new_upd);
	printk(BIOS_SPEW, "\t0x%p: &hob_list_ptr\n", fsp_get_hob_list_ptr());
}

void fsp_debug_after_memory_init(uint32_t status)
{
	if (IS_ENABLED(CONFIG_DISPLAY_FSP_CALLS_AND_STATUS))
		printk(BIOS_SPEW, "FspMemoryInit returned 0x%08x\n", status);

	if (status != FSP_SUCCESS)
		return;

	/* Verify that the HOB list pointer was set */
	if (fsp_get_hob_list() == NULL)
		die("ERROR - HOB list pointer was not returned!\n");

	/* Display and verify the HOBs */
	if (IS_ENABLED(CONFIG_DISPLAY_HOBS))
		fsp_display_hobs();
	if (IS_ENABLED(CONFIG_VERIFY_HOBS))
		fsp_verify_memory_init_hobs();

	/* Display the MTRRs */
	if (IS_ENABLED(CONFIG_DISPLAY_MTRRS))
		soc_display_mtrrs();
}

/*-----------
 * SiliconInit
 *-----------
 */
void fsp_debug_before_silicon_init(fsp_silicon_init_fn silicon_init,
	const FSPS_UPD *fsps_old_upd,
	const FSPS_UPD *fsps_new_upd)
{
	/* Display the MTRRs */
	if (IS_ENABLED(CONFIG_DISPLAY_MTRRS))
		soc_display_mtrrs();

	/* Display the UPD values */
	if (IS_ENABLED(CONFIG_DISPLAY_UPD_DATA))
		soc_display_fsps_upd_params(fsps_old_upd, fsps_new_upd);

	/* Display the call to FSP SiliconInit */
	if (!IS_ENABLED(CONFIG_DISPLAY_FSP_CALLS_AND_STATUS))
		return;
	printk(BIOS_SPEW, "Calling FspSiliconInit: 0x%p\n", silicon_init);
	printk(BIOS_SPEW, "\t0x%p: upd\n", fsps_new_upd);
}

void fsp_debug_after_silicon_init(uint32_t status)
{
	if (IS_ENABLED(CONFIG_DISPLAY_FSP_CALLS_AND_STATUS))
		printk(BIOS_SPEW, "FspSiliconInit returned 0x%08x\n", status);

	/* Display the HOBs */
	if (IS_ENABLED(CONFIG_DISPLAY_HOBS))
		fsp_display_hobs();

	/* Display the MTRRs */
	if (IS_ENABLED(CONFIG_DISPLAY_MTRRS))
		soc_display_mtrrs();
}

/*-----------
 * FspNotify
 *-----------
 */
void fsp_before_debug_notify(fsp_notify_fn notify,
	const struct fsp_notify_params *notify_params)
{
	/* Display the call to FspNotify */
	if (!IS_ENABLED(CONFIG_DISPLAY_FSP_CALLS_AND_STATUS))
		return;
	printk(BIOS_SPEW, "0x%08x: notify_params->phase\n",
		notify_params->phase);
	printk(BIOS_SPEW, "Calling FspNotify: 0x%p\n", notify);
	printk(BIOS_SPEW, "\t0x%p: notify_params\n", notify_params);
}

void fsp_debug_after_notify(uint32_t status)
{
	if (IS_ENABLED(CONFIG_DISPLAY_FSP_CALLS_AND_STATUS))
		printk(BIOS_SPEW, "FspNotify returned 0x%08x\n", status);

	/* Display the HOBs */
	if (IS_ENABLED(CONFIG_DISPLAY_HOBS))
		fsp_display_hobs();

	/* Display the MTRRs */
	if (IS_ENABLED(CONFIG_DISPLAY_MTRRS))
		soc_display_mtrrs();
}
