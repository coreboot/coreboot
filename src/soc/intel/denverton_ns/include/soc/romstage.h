/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_DENVERTON_NS_ROMSTAGE_H_
#define _SOC_DENVERTON_NS_ROMSTAGE_H_

#include <fsp/api.h>

/* These functions are weak and can be overridden by a mainboard functions. */
void mainboard_memory_init_params(FSPM_UPD *mupd);
void mainboard_config_gpios(void);

#endif /* _SOC_DENVERTON_NS_ROMSTAGE_H_ */
