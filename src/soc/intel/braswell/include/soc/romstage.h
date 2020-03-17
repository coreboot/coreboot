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

#ifndef _SOC_ROMSTAGE_H_
#define _SOC_ROMSTAGE_H_

#include <stdint.h>
#include <fsp/romstage.h>
#include <fsp/util.h>
#include <soc/pm.h>

void gfx_init(void);
void punit_init(void);

/* romstage.c functions */
int chipset_prev_sleep_state(struct chipset_power_state *ps);

/* Values for FSP's PcdMemoryTypeEnable */
#define MEM_DDR3	0
#define MEM_LPDDR3	1

#endif /* _SOC_ROMSTAGE_H_ */
