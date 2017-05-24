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

#include <AGESA.h>
#include <BiosCallOuts.h>
#include <FchPlatform.h>
#include <soc/hudson.h>
#include <stdlib.h>

static const GPIO_CONTROL oem_kahlee_gpio[] = {
	/* SER TX */
	{8, Function1, FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE
						| FCH_GPIO_OUTPUT_ENABLE},
	/* SER RX */
	{9, Function1, FCH_GPIO_PULL_UP_ENABLE },

	/* AGPIO 12 */
	{12, Function2, FCH_GPIO_PULL_UP_ENABLE },

	/* TS_EN_SOC (TouchScreen enable GPIO) */
	{13, Function1, FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE
						| FCH_GPIO_OUTPUT_ENABLE},

	/* CAM_PWRON (Camera enable GPIO) */
	{14, Function1,  FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE
						| FCH_GPIO_OUTPUT_ENABLE},

	/* EC_IN_RW */
	{15, Function1, FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE
						| FCH_GPIO_OUTPUT_ENABLE},

	/* APU_I2C_3_SCL */
	{19, Function1, FCH_GPIO_PULL_UP_ENABLE },

	/* APU_I2C_3_SDA */
	{20, Function1, FCH_GPIO_PULL_UP_ENABLE },


	/* APU_BT_ON# */
	{24, Function1, FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE
						| FCH_GPIO_OUTPUT_ENABLE},

	/* DEVSLP1_SSD */
	{67, Function1, FCH_GPIO_PULL_UP_ENABLE},

	/* DEVSLP1_EMMC */
	/* No Connect for now.
	 * {70, Function1, FCH_GPIO_PULL_UP_ENABLE},
	 */

	/* SPI_TPM_CS_L */
	{76, Function1, FCH_GPIO_PULL_DOWN_ENABLE | FCH_GPIO_OUTPUT_VALUE
						  | FCH_GPIO_OUTPUT_ENABLE},

	/* CAM_LED# */
	{84, Function1, FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE
						| FCH_GPIO_OUTPUT_ENABLE},

	/* TS_RST#  (TouchScreen Reset) */
	{85, Function1, FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE
						| FCH_GPIO_OUTPUT_ENABLE},

	/* WLAN_RST#_AUX */
	{119, Function2, FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE
						 | FCH_GPIO_OUTPUT_ENABLE},

	/* BD_ID1 */
	{135, Function1, 0 },

	/* TPM_SERIRQ# */
	{139, Function1, FCH_GPIO_PULL_UP_ENABLE },

	/* BD_ID2 */
	{140, Function1, 0 },

	/* APU_SPI_WP */
	{142, Function1, FCH_GPIO_PULL_UP_ENABLE },

	/* BD_ID3 */
	{144, Function1, 0 },

	{-1}
};

static AGESA_STATUS fch_initreset(UINT32 Func, UINTN FchData, VOID *ConfigPtr)
{
	AMD_CONFIG_PARAMS *StdHeader = ConfigPtr;

	if (StdHeader->Func == AMD_INIT_RESET) {
		FCH_RESET_DATA_BLOCK *FchParams_reset;
		FchParams_reset = (FCH_RESET_DATA_BLOCK *)FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT RESET ");
		FchParams_reset->FchReset.SataEnable = hudson_sata_enable();
		FchParams_reset->FchReset.IdeEnable = hudson_ide_enable();
		FchParams_reset->EarlyOemGpioTable = oem_kahlee_gpio;
		printk(BIOS_DEBUG, "Done\n");
	}

	return AGESA_SUCCESS;
}

const BIOS_CALLOUT_STRUCT BiosCallouts[] = {
	{AGESA_FCH_OEM_CALLOUT,          fch_initreset },
};

const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);
