/*
 * This file is part of the coreboot project.
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

#include <device/device.h>

void bsp_init_and_start_aps(struct bus *cpu_bus);

/* These helpers are for performing SMM relocation. */
void southbridge_smm_init(void);
u32 northbridge_get_tseg_base(void);
u32 northbridge_get_tseg_size(void);
void northbridge_write_smram(u8 smram);

bool cpu_has_alternative_smrr(void);

/* parallel MP helper functions */
void smm_info(uintptr_t *perm_smbase, size_t *perm_smsize,
		size_t *smm_save_state_size);
void smm_initialize(void);
void southbridge_smm_clear_state(void);
void smm_relocation_handler(int cpu, uintptr_t curr_smbase,
			uintptr_t staggered_smbase);
void smm_relocate(void);
