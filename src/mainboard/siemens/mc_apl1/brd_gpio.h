/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corporation. All Rights Reserved.
 * Copyright (C) 2017 Siemens AG
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

#include <soc/gpio.h>

#if ENV_ROMSTAGE

static const struct pad_config gpio_table[] = {
	/* Debug tracing. */
	PAD_CFG_NF(GPIO_0, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPIO_1, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPIO_2, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPIO_3, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPIO_4, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPIO_5, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPIO_6, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPIO_7, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPIO_8, NATIVE, DEEP, NF1),

	PAD_CFG_GPI(GPIO_152, DN_20K, DEEP), /* Unused */
	PAD_CFG_GPI(GPIO_19, UP_20K, DEEP), /* Unused */
	PAD_CFG_GPI(GPIO_13, UP_20K, DEEP), /* Unused */
	PAD_CFG_GPI(GPIO_17, UP_20K, DEEP), /* Unused */
	PAD_CFG_GPI(GPIO_15, UP_20K, DEEP), /* Unused */

	PAD_CFG_NF(GPIO_210, NATIVE, DEEP, NF1), /* CLKREQ# */

	PAD_CFG_NF(SMB_CLK, NATIVE, DEEP, NF1),
	PAD_CFG_NF(SMB_DATA, NATIVE, DEEP, NF1),
	PAD_CFG_NF(LPC_ILB_SERIRQ, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_CLKOUT0, NATIVE, DEEP, NF1),
	PAD_CFG_GPI(LPC_CLKOUT1, UP_20K, DEEP), /* LPC_CLKOUT1 -- unused */
	PAD_CFG_NF(LPC_AD0, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_AD1, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_AD2, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_AD3, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_CLKRUNB, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_FRAMEB, UP_20K, DEEP, NF1),
};

#endif
