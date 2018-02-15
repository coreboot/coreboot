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
#include <soc/southbridge.h>
#include <stdlib.h>
#include <soc/gpio.h>

#include "gpio.h"

/*
 * As a rule of thumb, GPIO pins used by coreboot should be initialized at
 * bootblock while GPIO pins used only by the OS should be initialized at
 * ramstage.
 */
const struct soc_amd_stoneyridge_gpio gpio_set_stage_reset[] = {
	/* NFC PU */
	{GPIO_64, Function0, FCH_GPIO_PULL_UP_ENABLE | OUTPUT_H },
	/* PCIe presence detect */
	{GPIO_69, Function0, FCH_GPIO_PULL_UP_ENABLE | INPUT },
	/* MUX for Power Express Eval */
	{GPIO_116, Function1, FCH_GPIO_PULL_DOWN_ENABLE | INPUT },
	/* SD power */
	{GPIO_119, Function2, FCH_GPIO_PULL_UP_ENABLE | OUTPUT_H },
};

const struct soc_amd_stoneyridge_gpio gpio_set_stage_ram[] = {
	/* BT radio disable */
	{GPIO_14, Function1, FCH_GPIO_PULL_UP_ENABLE | OUTPUT_H },
	/* NFC wake */
	{GPIO_65, Function0, FCH_GPIO_PULL_UP_ENABLE | OUTPUT_H },
	/* Webcam */
	{GPIO_66, Function0, FCH_GPIO_PULL_UP_ENABLE | OUTPUT_H },
	/* GPS sleep */
	{GPIO_70, Function0, FCH_GPIO_PULL_UP_ENABLE | OUTPUT_H },
};

const struct soc_amd_stoneyridge_gpio *early_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(gpio_set_stage_reset);
	return gpio_set_stage_reset;
}

const struct soc_amd_stoneyridge_gpio *gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(gpio_set_stage_ram);
	return gpio_set_stage_ram;
}
