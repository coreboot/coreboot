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
 *
 */

#ifndef _SOC_DENVERTON_NS_ROMSTAGE_H_
#define _SOC_DENVERTON_NS_ROMSTAGE_H_

#include <fsp/api.h>

/* These functions are weak and can be overridden by a mainboard functions. */
void mainboard_memory_init_params(FSPM_UPD *mupd);
void mainboard_config_gpios(void);

#endif /* _SOC_DENVERTON_NS_ROMSTAGE_H_ */
