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

#include <arch/io.h>
#include <compiler.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/funitcfg.h>
#include <soc/nvidia/tegra/usb.h>
#include <soc/padconfig.h>
#include <string.h>

struct clk_dev_control {
	uint32_t *clk_enb_set;
	uint32_t *rst_dev_clr;
};

struct funit_cfg_data {
	const char *name;
	void *ctlr_base;
	uint32_t *clk_src_reg;
	const struct clk_dev_control * const dev_control;
	uint32_t clk_enb_val;
};

enum {
	CLK_L_SET = 0,
	CLK_H_SET = 1,
	CLK_U_SET = 2,
	CLK_V_SET = 3,
	CLK_W_SET = 4,
	CLK_X_SET = 5,
	CLK_Y_SET = 6,
};

#define CLK_SET_REGS(x)					\
	{						\
		CLK_RST_REG(clk_enb_##x##_set),		\
		CLK_RST_REG(rst_dev_##x##_clr),		\
	}

static const struct clk_dev_control clk_data_arr[] = {
	[CLK_L_SET] = CLK_SET_REGS(l),
	[CLK_H_SET] = CLK_SET_REGS(h),
	[CLK_U_SET] = CLK_SET_REGS(u),
	[CLK_V_SET] = CLK_SET_REGS(v),
	[CLK_W_SET] = CLK_SET_REGS(w),
	[CLK_X_SET] = CLK_SET_REGS(x),
	[CLK_Y_SET] = CLK_SET_REGS(y),
};

#define FUNIT_DATA(funit_, loname_, clk_set_)				\
	[FUNIT_INDEX(funit_)] = {					\
		.name = STRINGIFY(loname_),				\
		.ctlr_base = (void *)(uintptr_t)TEGRA_##funit_##_BASE,	\
		.clk_src_reg = CLK_RST_REG(clk_src_##loname_),		\
		.dev_control = &clk_data_arr[CLK_##clk_set_##_SET],	\
		.clk_enb_val = CLK_##clk_set_##_##funit_,		\
	}

#define FUNIT_DATA_USB(funit_, clk_set_)				\
	[FUNIT_INDEX(funit_)] = {					\
		.name = STRINGIFY(funit_),				\
		.ctlr_base = (void *)(uintptr_t)TEGRA_##funit_##_BASE,	\
		.dev_control = &clk_data_arr[CLK_##clk_set_##_SET],	\
		.clk_enb_val = CLK_##clk_set_##_##funit_,		\
	}

static const struct funit_cfg_data funit_data[] =  {
	FUNIT_DATA(I2C1, i2c1, L),
	FUNIT_DATA(I2C2, i2c2, H),
	FUNIT_DATA(I2C3, i2c3, U),
	FUNIT_DATA(I2C5, i2c5, H),
	FUNIT_DATA(I2C6, i2c6, X),
	FUNIT_DATA(SDMMC1, sdmmc1, L),
	FUNIT_DATA(SDMMC4, sdmmc4, L),
	FUNIT_DATA_USB(USBD, L),
	FUNIT_DATA_USB(USB2, H),
	FUNIT_DATA(QSPI, qspi, Y),
	FUNIT_DATA(I2S1, i2s1, L),
};
_Static_assert(ARRAY_SIZE(funit_data) == FUNIT_INDEX_MAX,
		"funit_cfg_data array not filled out!");

static inline uint32_t get_clk_src_freq(uint32_t clk_src_freq_id)
{
	uint32_t freq = 0;

	switch (clk_src_freq_id) {
	case CLK_M:
		freq = TEGRA_CLK_M_KHZ;
		break;
	case PLLP:
		freq = TEGRA_PLLP_KHZ;
		break;
	default:
		printk(BIOS_SPEW, "%s ERROR: Unknown clk_src %d\n",
		       __func__, clk_src_freq_id);
	}

	return freq;
}

static void configure_clock(const struct funit_cfg * const entry,
				const struct funit_cfg_data * const funit)
{
	const char *funit_i2c = "i2c";
	uint32_t clk_div;
	uint32_t clk_div_mask;
	uint32_t clk_src_freq;

	clk_src_freq = get_clk_src_freq(entry->clk_src_freq_id);

	if (strncmp(funit->name, funit_i2c, strlen(funit_i2c)) == 0) {
		/* I2C funit */
		clk_div = get_i2c_clk_div(clk_src_freq,
					entry->clk_dev_freq_khz);
		clk_div_mask = CLK_DIV_MASK_I2C;
	} else {
		/* Non I2C */
		clk_div = get_clk_div(clk_src_freq, entry->clk_dev_freq_khz);
		clk_div_mask = CLK_DIV_MASK;
	}

	_clock_set_div(funit->clk_src_reg, funit->name, clk_div,
			clk_div_mask, entry->clk_src_id);
}

static inline int is_usb(uint32_t idx)
{
	return (idx == FUNIT_USBD || idx == FUNIT_USB2);
}

void soc_configure_funits(const struct funit_cfg * const entries, size_t num)
{
	size_t i;

	for (i = 0; i < num; i++) {
		const struct funit_cfg * const entry = &entries[i];
		const struct funit_cfg_data *funit;
		const struct clk_dev_control *dev_control;
		int funit_usb = is_usb(entry->funit_index);

		if (entry->funit_index >= FUNIT_INDEX_MAX) {
			printk(BIOS_ERR, "Error: Index out of bounds\n");
			continue;
		}

		funit = &funit_data[entry->funit_index];
		dev_control = funit->dev_control;

		/* USB controllers have a fixed clock source. */
		if (!funit_usb)
			configure_clock(entry, funit);

		clock_grp_enable_clear_reset(funit->clk_enb_val,
						dev_control->clk_enb_set,
						dev_control->rst_dev_clr);

		if (funit_usb)
			usb_setup_utmip(funit->ctlr_base);

		soc_configure_pads(entry->pad_cfg,entry->pad_cfg_size);
	}
}

void __weak usb_setup_utmip(void *usb_base)
{
	/* default empty implementation required if usb.c is not included */
	printk(BIOS_ERR, "USB setup is not supported in current stage\n");
}
