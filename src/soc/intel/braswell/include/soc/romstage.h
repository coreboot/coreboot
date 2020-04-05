/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
