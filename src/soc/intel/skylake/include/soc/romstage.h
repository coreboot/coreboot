/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ROMSTAGE_H_
#define _SOC_ROMSTAGE_H_

#include <fsp/api.h>

void mainboard_memory_init_params(FSPM_UPD *mupd);
void systemagent_early_init(void);
void romstage_pch_init(void);
int smbus_read_byte(unsigned int device, unsigned int address);
/* Board type */
enum board_type {
	BOARD_TYPE_MOBILE  = 0,
	BOARD_TYPE_DESKTOP = 1,
	BOARD_TYPE_ULT_ULX = 5,
	BOARD_TYPE_SERVER  = 7
};

#endif /* _SOC_ROMSTAGE_H_ */
