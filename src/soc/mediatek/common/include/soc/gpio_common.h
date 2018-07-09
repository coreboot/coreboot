/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

#ifndef SOC_MEDIATEK_COMMON_GPIO_H
#define SOC_MEDIATEK_COMMON_GPIO_H

enum pull_enable {
	GPIO_PULL_DISABLE = 0,
	GPIO_PULL_ENABLE = 1,
};

enum pull_select {
	GPIO_PULL_DOWN = 0,
	GPIO_PULL_UP = 1,
};

#endif
