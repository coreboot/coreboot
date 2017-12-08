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

#include <amdblocks/agesawrapper.h>
#include <amdblocks/BiosCallOuts.h>
#include <soc/imc.h>
#include <soc/southbridge.h>
#include <stdlib.h>
#include <string.h>

/* Hardware Monitor Fan Control
 * Hardware limitation:
 *  HWM will fail to read the input temperature via I2C if other
 *  software switches the I2C address.  AMD recommends using IMC
 *  to control fans, instead of HWM.
 */
void oem_fan_control(FCH_DATA_BLOCK *FchParams)
{
	/* Enable IMC fan control. the recommand way */
	imc_reg_init();

	FchParams->Imc.ImcEnable = TRUE;

	/* 1 IMC, 0 HWM */
	FchParams->Hwm.HwmControl = 1;

	/* 2 disable IMC, 1 enable IMC, 0 following hw strap setting */
	FchParams->Imc.ImcEnableOverWrite = 1;

	memset(&FchParams->Imc.EcStruct, 0, sizeof(FCH_EC));
}

void platform_FchParams_env(FCH_DATA_BLOCK *FchParams_env)
{

}
