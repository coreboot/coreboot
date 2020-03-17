/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_APOLLOLAKE_ROMSTAGE_H_
#define _SOC_APOLLOLAKE_ROMSTAGE_H_

#include <fsp/api.h>

void set_max_freq(void);
void mainboard_memory_init_params(FSPM_UPD *mupd);
void mainboard_save_dimm_info(void);
void report_platform_info(void);

#endif /* _SOC_APOLLOLAKE_ROMSTAGE_H_ */
