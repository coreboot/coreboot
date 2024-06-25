/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _MAINBOARD_FROST_CREEK_ROMSTAGE_H_
#define _MAINBOARD_FROST_CREEK_ROMSTAGE_H_

#include <fsp/soc_binding.h>

void mainboard_config_gpios(void);
void mainboard_memory_init_params(FSPM_UPD *m_upd);

#endif // _MAINBOARD_FROST_CREEK_ROMSTAGE_H_
