/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef RAMINIT_H
#define RAMINIT_H

#include <stdint.h>
#include "pei_data.h"

/* Optional function to copy SPD data for on-board memory */
void copy_spd(struct pei_data *peid);

/* Mainboard callback to fill in the SPD addresses in MRC format */
void mb_get_spd_map(uint8_t spd_map[4]);

/* Necessary function to initialize pei_data with mainboard-specific settings */
void mainboard_fill_pei_data(struct pei_data *pei_data);

void sdram_initialize(struct pei_data *pei_data);
void setup_sdram_meminfo(struct pei_data *pei_data);
int fixup_haswell_errata(void);
/* save_mrc_data() must be called after cbmem has been initialized. */
void save_mrc_data(struct pei_data *pei_data);

#endif				/* RAMINIT_H */
