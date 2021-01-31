/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include "onboard.h"

void mainboard_silicon_init_params(SILICON_INIT_UPD *params)
{
	params->ChvSvidConfig = SVID_PMIC_CONFIG;
	params->PMIC_I2CBus = BCRD2_PMIC_I2C_BUS;
}
