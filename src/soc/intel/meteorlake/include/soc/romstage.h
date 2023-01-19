/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ROMSTAGE_H_
#define _SOC_ROMSTAGE_H_

#include <fsp/api.h>
#include <soc/soc_chip.h>

void mainboard_update_premem_soc_chip_config(struct soc_intel_meteorlake_config *config);
void mainboard_memory_init_params(FSPM_UPD *memupd);
void systemagent_early_init(void);

/* Board type */
enum board_type {
	BOARD_TYPE_MOBILE  = 0,
	BOARD_TYPE_DESKTOP = 1,
	BOARD_TYPE_ULT_ULX = 5,
	BOARD_TYPE_SERVER  = 7
};

#endif /* _SOC_ROMSTAGE_H_ */
