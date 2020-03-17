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

#ifndef SPD_UTIL_H
#define SPD_UTIL_H

#include <fsp/soc_binding.h>

uint8_t get_ramid(void);
int get_variant_spd_index(int ram_id, int *dual);
void spd_memory_init_params(MEMORY_INIT_UPD *memory_params);

#endif /* SPD_UTIL_H */
