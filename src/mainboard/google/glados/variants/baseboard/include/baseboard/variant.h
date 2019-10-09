/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef GLADOS_VARIANT_H
#define GLADOS_VARIANT_H

#include <fsp/soc_binding.h>

int is_dual_channel(const int spd_index);
void mainboard_gpio_smi_sleep(void);
void variant_memory_init_params(FSPM_UPD *mupd, const int spd_index);

#endif /* GLADOS_VARIANT_H */
