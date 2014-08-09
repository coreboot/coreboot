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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <soc/addressmap.h>
#include <soc/funitcfg.h>
#include <soc/clock.h>
#include <soc/padconfig.h>
#include <string.h>

struct clk_set_data {
	size_t clk_enb_set_offset;
	size_t rst_dev_clr_offset;
};

struct funit_cfg_data {
	const char *name;
	size_t clk_src_offset;
	uint8_t clk_data_index;
	uint32_t clk_enb_val;
};

enum {
	CLK_L_SET = 0,
	CLK_H_SET = 1,
	CLK_U_SET = 2,
	CLK_V_SET = 3,
	CLK_W_SET = 4,
	CLK_X_SET = 5,
};

#define CLK_SET_OFFSETS(x)						\
	{								\
	offsetof(struct clk_rst_ctlr, clk_enb_##x##_set),		\
	offsetof(struct clk_rst_ctlr, rst_dev_##x##_clr)		\
	}

static const struct clk_set_data clk_data_arr[] = {
	[CLK_L_SET] = CLK_SET_OFFSETS(l),
	[CLK_H_SET] = CLK_SET_OFFSETS(h),
	[CLK_U_SET] = CLK_SET_OFFSETS(u),
	[CLK_V_SET] = CLK_SET_OFFSETS(v),
	[CLK_W_SET] = CLK_SET_OFFSETS(w),
	[CLK_X_SET] = CLK_SET_OFFSETS(x),
};

static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;

static const struct funit_cfg_data funit_data[] =  {
	[FUNIT_SBC1] = {"sbc1", offsetof(struct clk_rst_ctlr, clk_src_sbc1),
			CLK_H_SET,
			CLK_H_SBC1},
	[FUNIT_SBC4] = {"sbc4", offsetof(struct clk_rst_ctlr, clk_src_sbc4),
			CLK_U_SET,
			CLK_U_SBC4},
	[FUNIT_I2C3] = {"i2c3", offsetof(struct clk_rst_ctlr, clk_src_i2c3),
			CLK_U_SET,
			CLK_U_I2C3},
	[FUNIT_I2C5] = {"i2c5", offsetof(struct clk_rst_ctlr, clk_src_i2c5),
			CLK_H_SET,
			CLK_H_I2C5},
	[FUNIT_SDMMC3] = {"sdmmc3", offsetof(struct clk_rst_ctlr, clk_src_sdmmc3),
			  CLK_U_SET,
			  CLK_U_SDMMC3},
	[FUNIT_SDMMC4] = {"sdmmc4", offsetof(struct clk_rst_ctlr, clk_src_sdmmc4),
			  CLK_L_SET,
			  CLK_L_SDMMC4},
};

static inline uint32_t get_clk_src_freq(uint32_t clk_src)
{
	uint32_t freq = 0;

	switch(clk_src) {
	case CLK_M:
		freq = TEGRA_CLK_M_KHZ;
		break;
	case PLLP:
		freq = TEGRA_PLLP_KHZ;
		break;
	default:
		printk(BIOS_SPEW, "%s ERROR: Unknown clk_src %d\n",
		       __func__,clk_src);
	}

	return freq;
}

void soc_configure_funits(const struct funit_cfg * const entries, size_t num)
{
	size_t i;
	const char *funit_i2c = "i2c";
	uint32_t clk_div;
	uint32_t clk_div_mask;


	for (i = 0; i < num; i++) {
		uint8_t *rst_base = (uint8_t*)clk_rst;
		const struct funit_cfg * const entry = &entries[i];
		const struct funit_cfg_data *funit;
		const struct clk_set_data *clk_data;
		uint32_t *clk_src_reg, *clk_enb_set_reg, *rst_dev_clr_reg;
		uint32_t clk_src_freq;

		if (entry->funit_index >= FUNIT_INDEX_MAX) {
			printk(BIOS_ERR, "Error: Index out of bounds\n");
			continue;
		}

		funit = &funit_data[entry->funit_index];
		clk_data = &clk_data_arr[funit->clk_data_index];

		clk_src_reg = (uint32_t*)(rst_base + funit->clk_src_offset);
		clk_enb_set_reg = (uint32_t*)(rst_base
					      + clk_data->clk_enb_set_offset);
		rst_dev_clr_reg = (uint32_t*)(rst_base
					      + clk_data->rst_dev_clr_offset);

		clk_src_freq = get_clk_src_freq(entry->clk_src_id);

		if (strncmp(funit->name,funit_i2c,strlen(funit_i2c)) == 0) {
			/* I2C funit */
			clk_div = get_i2c_clk_div(clk_src_freq,
						  entry->clk_dev_freq_khz);
			clk_div_mask = CLK_DIV_MASK_I2C;
		} else {
			/* Non I2C */
			clk_div = get_clk_div(clk_src_freq,entry->clk_dev_freq_khz);
			clk_div_mask = CLK_DIV_MASK;
		}

		_clock_set_div(clk_src_reg,funit->name,clk_div,
			       clk_div_mask,entry->clk_src_id);

		clock_grp_enable_clear_reset(funit->clk_enb_val,
					     clk_enb_set_reg,
					     rst_dev_clr_reg);

		soc_configure_pads(entry->pad_cfg,entry->pad_cfg_size);
	}
}
