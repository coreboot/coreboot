/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Advanced Micro Devices, Inc.
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

#include <agesawrapper.h>
#include <BiosCallOuts.h>
#include <soc/southbridge.h>
#include <stdlib.h>
#include <baseboard/variants.h>

void platform_FchParams_reset(FCH_RESET_DATA_BLOCK *FchParams_reset)
{
	FchParams_reset->EarlyOemGpioTable = (void *)get_gpio_table();
}

void platform_FchParams_env(FCH_DATA_BLOCK *FchParams_env)
{
	FchParams_env->PostOemGpioTable = (void *)get_gpio_table();

	/* SDHCI/MMC configuration */
	if (IS_ENABLED(CONFIG_BOARD_GOOGLE_KAHLEE))
		FchParams_env->Sd.SdSlotType = 1; // EMMC
}
