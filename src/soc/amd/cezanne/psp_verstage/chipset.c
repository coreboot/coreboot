/* SPDX-License-Identifier: GPL-2.0-only */

/* This file contains stub for not-yet-implemented svc in cezanne PSP.
 * So this file will and should be removed eventually when psp_verstage works
 * correctly in cezanne.
 */

#include <bl_uapp/bl_syscall_public.h>
#include <console/console.h>
#include <psp_verstage.h>

uint32_t update_psp_bios_dir(uint32_t *psp_dir_offset, uint32_t *bios_dir_offset)
{
	return svc_update_psp_bios_dir(psp_dir_offset, bios_dir_offset);
}

uint32_t save_uapp_data(void *address, uint32_t size)
{
	return svc_save_uapp_data(address, size);
}

uint32_t get_bios_dir_addr(struct psp_ef_table *ef_table)
{
	return ef_table->bios3_entry;
}


/* Functions below are stub functions for not-yet-implemented PSP features.
 * These functions should be replaced with proper implementations later.
 */

uint32_t svc_write_postcode(uint32_t postcode)
{
	return 0;
}
