/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __SOC_MEDIATEK_MT8183_FLASH_CONTROLLER_H__
#define __SOC_MEDIATEK_MT8183_FLASH_CONTROLLER_H__

#include <spi-generic.h>

int mtk_spi_flash_probe(const struct spi_slave *spi, struct spi_flash *flash);

#endif
