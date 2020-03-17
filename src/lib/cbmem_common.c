/*
 * This file is part of the coreboot project.
 *
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

#include <cbmem.h>
#include <bootstate.h>
#include <symbols.h>

void cbmem_run_init_hooks(int is_recovery)
{
	cbmem_init_hook_t *init_hook_ptr =
		(cbmem_init_hook_t *)&_cbmem_init_hooks;
	cbmem_init_hook_t *einit_hook_ptr =
		(cbmem_init_hook_t *)&_ecbmem_init_hooks;

	if (REGION_SIZE(cbmem_init_hooks) == 0)
		return;

	while (init_hook_ptr !=  einit_hook_ptr) {
		(*init_hook_ptr)(is_recovery);
		init_hook_ptr++;
	}
}
