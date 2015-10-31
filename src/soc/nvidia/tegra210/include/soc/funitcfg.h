/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#ifndef __SOC_NVIDIA_TEGRA210_FUNIT_CFG_H
#define __SOC_NVIDIA_TEGRA210_FUNIT_CFG_H

#include <soc/clock.h>
#include <soc/padconfig.h>
#include <soc/pinmux.h>
#include <stdint.h>

#define FUNIT_INDEX(_name)  FUNIT_##_name

enum {
	FUNIT_INDEX(I2C1),
	FUNIT_INDEX(I2C2),
	FUNIT_INDEX(I2C3),
	FUNIT_INDEX(I2C5),
	FUNIT_INDEX(I2C6),
	FUNIT_INDEX(SDMMC1),
	FUNIT_INDEX(SDMMC4),
	FUNIT_INDEX(USBD),
	FUNIT_INDEX(USB2),
	FUNIT_INDEX(QSPI),
	FUNIT_INDEX(I2S1),
	FUNIT_INDEX_MAX,
};

/*
 * Note: these bus numbers are dependent on the driver implementations, and
 * currently the I2C is 0-based and SPI is 1-based in its indexing.
 */
enum {

	I2C1_BUS = 0,
	I2C2_BUS = 1,
	I2C3_BUS = 2,
	I2C5_BUS = 4,
	I2CPWR_BUS = I2C5_BUS,
	I2C6_BUS = 5,
	QSPI_BUS = 7,

	SPI1_BUS = 1,
	SPI4_BUS = 4,
};

struct funit_cfg {
	uint32_t funit_index;
	uint32_t clk_src_id;
	uint32_t clk_src_freq_id;
	uint32_t clk_dev_freq_khz;
	struct pad_config const* pad_cfg;
	size_t pad_cfg_size;
};

#define FUNIT_CFG(_funit,_clk_src,_clk_freq,_cfg,_cfg_size)		\
	{								\
		.funit_index = FUNIT_INDEX(_funit),			\
		.clk_src_id = CLK_SRC_DEV_ID(_funit, _clk_src),		\
		.clk_src_freq_id = CLK_SRC_FREQ_ID(_funit, _clk_src),	\
		.clk_dev_freq_khz = _clk_freq,				\
		.pad_cfg = _cfg,					\
		.pad_cfg_size = _cfg_size,				\
	}

#define FUNIT_CFG_USB(_funit)						\
	{								\
		.funit_index = FUNIT_INDEX(_funit),			\
		.pad_cfg = NULL,					\
		.pad_cfg_size = 0,					\
	}

/*
 * Configure the funits associated with entry according to the configuration.
 */
void soc_configure_funits(const struct funit_cfg * const entries, size_t num);

#endif /* __SOC_NVIDIA_TEGRA210_FUNIT_CFG_H */
