/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

#ifndef _SOC_SPI_H_
#define _SOC_SPI_H_

#include <stdint.h>

/* These registers live behind SPI_BASE_ADDRESS. */
#define HSFSTS				0x04
#define FDATA0				0x10
# define FLOCKDN			(0x1 << 15)
#define SSFS				0x90
# define CYCLE_DONE_STATUS		(0x1 << 2)
# define FLASH_CYCLE_ERROR		(0x1 << 3)
#define SSFC				0x91
# define SPI_CYCLE_GO			(0x1 << 1)
# define DATA_CYCLE			(0x1 << 14)
#define PREOP				0x94
#define OPTYPE				0x96
#define OPMENU0				0x98
#define OPMENU1				0x9c
#define LVSCC				0xc4
# define VCL				(0x1 << 23)
# define EO(x)				(((x) & 0xff) << 8)
# define WG_1_BYTE			(0x0 << 2)
# define WG_64_BYTE			(0x1 << 2)
# define BES_256_BYTE			(0x0 << 0)
# define BES_4_KB			(0x1 << 0)
# define BES_8_KB			(0x2 << 0)
# define BES_64_KB			(0x3 << 0)
#define UVSCC				0xc8
#define SCS				0xf8
# define SMIWPEN			(0x1 << 7)
#define BCR				0xfc
# define EISS				(0x1 << 5)
# define SRC_MASK			(0x3 << 2)
# define SRC_CACHE_NO_PREFETCH		(0x0 << 2)
# define SRC_NO_CACHE_NO_PREFETCH	(0x1 << 2)
# define SRC_CACHE_PREFETCH		(0x2 << 2)
# define BCR_LE				(0x1 << 1)
# define BCR_WPD			(0x1 << 0)

/*
 * SPI lockdown configuration.
 */
struct spi_config {
	uint16_t preop;
	uint16_t optype;
	uint32_t opmenu[2];
	uint32_t lvscc;
	uint32_t uvscc;
};

/* Return 0 on success < 0 on failure. */
int mainboard_get_spi_config(struct spi_config *cfg);

#endif /* _SOC_SPI_H_ */
