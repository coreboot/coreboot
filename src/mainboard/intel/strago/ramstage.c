/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Intel Corporation
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

#include <soc/ramstage.h>
#include <boardid.h>
#include "onboard.h"

void mainboard_silicon_init_params(SILICON_INIT_UPD *params)
{
	if (board_id() == BOARD_BCRD2) {
			params->ChvSvidConfig = SVID_PMIC_CONFIG;
			params->PMIC_I2CBus = BCRD2_PMIC_I2C_BUS;
	}
}
