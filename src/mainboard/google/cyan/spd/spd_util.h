/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef SPD_UTIL_H
#define SPD_UTIL_H

#include <fsp/soc_binding.h>

uint8_t get_ramid(void);
int get_variant_spd_index(int ram_id, int *dual);
void spd_memory_init_params(MEMORY_INIT_UPD *memory_params);

#endif /* SPD_UTIL_H */
