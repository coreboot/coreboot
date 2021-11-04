/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 5.6
 */

#ifndef MTK_MT8186_SPI_H
#define MTK_MT8186_SPI_H

#include <spi-generic.h>

enum {
	SPI_NOR_GPIO_SET0 = 0,
	SPI_NOR_GPIO_SET1,
	SPI_NOR_GPIO_SET_NUM,
};

void mtk_snfc_init(int gpio_set);

#endif
