/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef RAMINIT_H
#define RAMINIT_H

#include "pei_data.h"

struct sys_info {
	u8 boot_path;
#define BOOT_PATH_NORMAL	0
#define BOOT_PATH_RESET		1
#define BOOT_PATH_RESUME	2
} __packed;

void sdram_initialize(struct pei_data *pei_data);
void save_mrc_data(struct pei_data *pei_data);
void mainboard_fill_pei_data(struct pei_data *pei_data);
int fixup_sandybridge_errata(void);

#endif /* RAMINIT_H */
