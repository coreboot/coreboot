/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <symbols.h>

void cbmem_run_init_hooks(int is_recovery)
{
	cbmem_init_hook_t *init_hook_ptr =
		(cbmem_init_hook_t *)&_cbmem_init_hooks;
	cbmem_init_hook_t *einit_hook_ptr =
		(cbmem_init_hook_t *)&_ecbmem_init_hooks;

	if (REGION_SIZE(cbmem_init_hooks) == 0)
		return;

	while (init_hook_ptr != einit_hook_ptr) {
		(*init_hook_ptr)(is_recovery);
		init_hook_ptr++;
	}
}
