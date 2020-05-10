/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef RAMINIT_H
#define RAMINIT_H

#include "pei_data.h"

void sdram_initialize(struct pei_data *pei_data);
void setup_sdram_meminfo(struct pei_data *pei_data);
int fixup_haswell_errata(void);
/* save_mrc_data() must be called after cbmem has been initialized. */
void save_mrc_data(struct pei_data *pei_data);

#endif				/* RAMINIT_H */
