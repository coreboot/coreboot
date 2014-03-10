/*
 * drivers/video/tegra/dc/dp.c
 *
 * Copyright (c) 2011-2013, NVIDIA Corporation.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/i2c.h>
#include <stdlib.h>
#include <string.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/nvidia/tegra/dc.h>
#include "chip.h"
#include "sor.h"
#include <soc/nvidia/tegra/displayport.h>

struct tegra_dc_dp_data dp_data;

static inline u32 tegra_dpaux_readl(struct tegra_dc_dp_data *dp, u32 reg)
{
	void *addr = dp->aux_base + (u32) (reg << 2);
	u32 reg_val = READL(addr);
	return reg_val;
}

static inline void tegra_dpaux_writel(struct tegra_dc_dp_data *dp,
					  u32 reg, u32 val)
{
	void *addr = dp->aux_base + (u32) (reg << 2);
	WRITEL(val, addr);
}

static inline u32 tegra_dc_dpaux_poll_register(struct tegra_dc_dp_data *dp,
					   u32 reg, u32 mask, u32 exp_val,
					   u32 poll_interval_us,
					   u32 timeout_us)
{
	u32 reg_val = 0;
	u32 temp = timeout_us;

	do {
		udelay(poll_interval_us);
		reg_val = tegra_dpaux_readl(dp, reg);
		if (timeout_us > poll_interval_us)
			timeout_us -= poll_interval_us;
		else
			break;
	} while ((reg_val & mask) != exp_val);

	if ((reg_val & mask) == exp_val)
		return 0;	/* success */
	printk(BIOS_ERR,
		   "dpaux_poll_register 0x%x: timeout: "
		   "(reg_val)0x%08x & (mask)0x%08x != (exp_val)0x%08x\n",
		   reg, reg_val, mask, exp_val);
	return temp;
}

static inline int tegra_dpaux_wait_transaction(struct tegra_dc_dp_data *dp)
{
	/* According to DP spec, each aux transaction needs to finish
	   within 40ms. */
	if (tegra_dc_dpaux_poll_register(dp, DPAUX_DP_AUXCTL,
					 DPAUX_DP_AUXCTL_TRANSACTREQ_MASK,
					 DPAUX_DP_AUXCTL_TRANSACTREQ_DONE,
					 100, DP_AUX_TIMEOUT_MS * 1000) != 0) {
		printk(BIOS_INFO, "dp: DPAUX transaction timeout\n");
		return -1;
	}
	return 0;
}

static int tegra_dc_dpaux_write_chunk(struct tegra_dc_dp_data *dp, u32 cmd,
					  u32 addr, u8 *data, u32 *size,
					  u32 *aux_stat)
{
	int i;
	u32 reg_val;
	u32 timeout_retries = DP_AUX_TIMEOUT_MAX_TRIES;
	u32 defer_retries = DP_AUX_DEFER_MAX_TRIES;
	u32 temp_data;

	if (*size > DP_AUX_MAX_BYTES)
		return -1;	/* only write one chunk of data */

	/* Make sure the command is write command */
	switch (cmd) {
		case DPAUX_DP_AUXCTL_CMD_I2CWR:
		case DPAUX_DP_AUXCTL_CMD_MOTWR:
		case DPAUX_DP_AUXCTL_CMD_AUXWR:
			break;
		default:
			printk(BIOS_ERR, "dp: aux write cmd 0x%x is invalid\n",
				cmd);
			return -1;
	}

	tegra_dpaux_writel(dp, DPAUX_DP_AUXADDR, addr);
	for (i = 0; i < DP_AUX_MAX_BYTES / 4; ++i) {
		memcpy(&temp_data, data, 4);
		tegra_dpaux_writel(dp, DPAUX_DP_AUXDATA_WRITE_W(i), temp_data);
		data += 4;
	}

	reg_val = tegra_dpaux_readl(dp, DPAUX_DP_AUXCTL);
	reg_val &= ~DPAUX_DP_AUXCTL_CMD_MASK;
	reg_val |= cmd;
	reg_val &= ~DPAUX_DP_AUXCTL_CMDLEN_FIELD;
	reg_val |= ((*size - 1) << DPAUX_DP_AUXCTL_CMDLEN_SHIFT);

	while ((timeout_retries > 0) && (defer_retries > 0)) {
		if ((timeout_retries != DP_AUX_TIMEOUT_MAX_TRIES) ||
			(defer_retries != DP_AUX_DEFER_MAX_TRIES))
			udelay(1);

		reg_val |= DPAUX_DP_AUXCTL_TRANSACTREQ_PENDING;
		tegra_dpaux_writel(dp, DPAUX_DP_AUXCTL, reg_val);

		if (tegra_dpaux_wait_transaction(dp))
			printk(BIOS_ERR, "dp: aux write transaction timeout\n");

		*aux_stat = tegra_dpaux_readl(dp, DPAUX_DP_AUXSTAT);

		if ((*aux_stat & DPAUX_DP_AUXSTAT_TIMEOUT_ERROR_PENDING) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_RX_ERROR_PENDING) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_SINKSTAT_ERROR_PENDING) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_NO_STOP_ERROR_PENDING)) {
			if (timeout_retries-- > 0) {
				printk(BIOS_INFO, "dp: aux write retry (0x%x) -- %d\n",
					   *aux_stat, timeout_retries);
				/* clear the error bits */
				tegra_dpaux_writel(dp, DPAUX_DP_AUXSTAT, *aux_stat);
				continue;
			} else {
				printk(BIOS_ERR, "dp: aux write got error (0x%x)\n",
					   *aux_stat);
				return -1;
			}
		}

		if ((*aux_stat & DPAUX_DP_AUXSTAT_REPLYTYPE_I2CDEFER) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_REPLYTYPE_DEFER)) {
			if (defer_retries-- > 0) {
				printk(BIOS_INFO, "dp: aux write defer (0x%x) -- %d\n",
					   *aux_stat, defer_retries);
				/* clear the error bits */
				tegra_dpaux_writel(dp, DPAUX_DP_AUXSTAT, *aux_stat);
				continue;
			} else {
				printk(BIOS_ERR, "dp: aux write defer exceeds max retries "
					   "(0x%x)\n", *aux_stat);
				return -1;
			}
		}

		if ((*aux_stat & DPAUX_DP_AUXSTAT_REPLYTYPE_MASK) ==
			DPAUX_DP_AUXSTAT_REPLYTYPE_ACK) {
			*size = ((*aux_stat) & DPAUX_DP_AUXSTAT_REPLY_M_MASK);
			return 0;
		} else {
			printk(BIOS_ERR, "dp: aux write failed (0x%x)\n",
				*aux_stat);
			return -1;
		}
	}
	/* Should never come to here */
	return -1;
}

static int tegra_dc_dpaux_read_chunk(struct tegra_dc_dp_data *dp, u32 cmd,
					 u32 addr, u8 *data, u32 *size,
					 u32 *aux_stat)
{
	u32 reg_val;
	u32 timeout_retries = DP_AUX_TIMEOUT_MAX_TRIES;
	u32 defer_retries = DP_AUX_DEFER_MAX_TRIES;

	if (*size > DP_AUX_MAX_BYTES)
		return -1;	/* only read one chunk */

	/* Check to make sure the command is read command */
	switch (cmd) {
		case DPAUX_DP_AUXCTL_CMD_I2CRD:
		case DPAUX_DP_AUXCTL_CMD_I2CREQWSTAT:
		case DPAUX_DP_AUXCTL_CMD_MOTRD:
		case DPAUX_DP_AUXCTL_CMD_AUXRD:
			break;
		default:
			printk(BIOS_ERR, "dp: aux read cmd 0x%x is invalid\n",
				cmd);
			return -1;
	}

	*aux_stat = tegra_dpaux_readl(dp, DPAUX_DP_AUXSTAT);
	if (!(*aux_stat & DPAUX_DP_AUXSTAT_HPD_STATUS_PLUGGED)) {
		printk(BIOS_SPEW, "dp: HPD is not detected\n");
		return -1;
	}

	tegra_dpaux_writel(dp, DPAUX_DP_AUXADDR, addr);

	reg_val = tegra_dpaux_readl(dp, DPAUX_DP_AUXCTL);
	reg_val &= ~DPAUX_DP_AUXCTL_CMD_MASK;
	reg_val |= cmd;
	reg_val &= ~DPAUX_DP_AUXCTL_CMDLEN_FIELD;
	reg_val |= ((*size - 1) << DPAUX_DP_AUXCTL_CMDLEN_SHIFT);
	while ((timeout_retries > 0) && (defer_retries > 0)) {
		if ((timeout_retries != DP_AUX_TIMEOUT_MAX_TRIES) ||
			(defer_retries != DP_AUX_DEFER_MAX_TRIES))
			udelay(DP_DPCP_RETRY_SLEEP_NS * 2);

		reg_val |= DPAUX_DP_AUXCTL_TRANSACTREQ_PENDING;
		tegra_dpaux_writel(dp, DPAUX_DP_AUXCTL, reg_val);

		if (tegra_dpaux_wait_transaction(dp))
			printk(BIOS_INFO, "dp: aux read transaction timeout\n");

		*aux_stat = tegra_dpaux_readl(dp, DPAUX_DP_AUXSTAT);

		if ((*aux_stat & DPAUX_DP_AUXSTAT_TIMEOUT_ERROR_PENDING) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_RX_ERROR_PENDING) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_SINKSTAT_ERROR_PENDING) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_NO_STOP_ERROR_PENDING)) {
			if (timeout_retries-- > 0) {
				printk(BIOS_INFO, "dp: aux read retry (0x%x)"
						" -- %d\n", *aux_stat,
						timeout_retries);
				/* clear the error bits */
				tegra_dpaux_writel(dp, DPAUX_DP_AUXSTAT,
						*aux_stat);
				continue;	/* retry */
			} else {
				printk(BIOS_ERR, "dp: aux read got error"
						" (0x%x)\n", *aux_stat);
				return -1;
			}
		}

		if ((*aux_stat & DPAUX_DP_AUXSTAT_REPLYTYPE_I2CDEFER) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_REPLYTYPE_DEFER)) {
			if (defer_retries-- > 0) {
				printk(BIOS_INFO, "dp: aux read defer (0x%x) -- %d\n",
					   *aux_stat, defer_retries);
				/* clear the error bits */
				tegra_dpaux_writel(dp, DPAUX_DP_AUXSTAT, *aux_stat);
				continue;
			} else {
				printk(BIOS_INFO, "dp: aux read defer exceeds max retries "
					   "(0x%x)\n", *aux_stat);
				return -1;
			}
		}

		if ((*aux_stat & DPAUX_DP_AUXSTAT_REPLYTYPE_MASK) ==
			DPAUX_DP_AUXSTAT_REPLYTYPE_ACK) {
			int i;
			u32 temp_data[4];

			for (i = 0; i < DP_AUX_MAX_BYTES / 4; ++i)
				temp_data[i] = tegra_dpaux_readl(dp,
						DPAUX_DP_AUXDATA_READ_W(i));

			*size = ((*aux_stat) & DPAUX_DP_AUXSTAT_REPLY_M_MASK);
			memcpy(data, temp_data, *size);

			return 0;
		} else {
			printk(BIOS_ERR, "dp: aux read failed (0x%x\n",
					*aux_stat);
			return -1;
		}
	}
	/* Should never come to here */
	printk(BIOS_ERR, "%s: can't\n", __func__);
	return -1;
}

static int tegra_dc_dp_dpcd_read(struct tegra_dc_dp_data *dp, u32 cmd,
				 u8 * data_ptr)
{
	u32 size = 1;
	u32 status = 0;
	int ret;

	ret = tegra_dc_dpaux_read_chunk(dp, DPAUX_DP_AUXCTL_CMD_AUXRD,
					cmd, data_ptr, &size, &status);
	if (ret)
		printk(BIOS_ERR,
			"dp: Failed to read DPCD data. CMD 0x%x, Status 0x%x\n",
			cmd, status);

	return ret;
}

static int tegra_dc_dp_dpcd_write(struct tegra_dc_dp_data *dp, u32 cmd,
				u8 data)
{
	u32 size = 1;
	u32 status = 0;
	int ret;

	ret = tegra_dc_dpaux_write_chunk(dp, DPAUX_DP_AUXCTL_CMD_AUXWR,
					cmd, &data, &size, &status);
	if (ret)
		printk(BIOS_ERR,
		       "dp: Failed to write DPCD data. CMD 0x%x, Status 0x%x\n",
		       cmd, status);
	return ret;
}

static void tegra_dc_dpaux_enable(struct tegra_dc_dp_data *dp)
{
	/* clear interrupt */
	tegra_dpaux_writel(dp, DPAUX_INTR_AUX, 0xffffffff);
	/* do not enable interrupt for now. Enable them when Isr in place */
	tegra_dpaux_writel(dp, DPAUX_INTR_EN_AUX, 0x0);

	tegra_dpaux_writel(dp, DPAUX_HYBRID_PADCTL,
		DPAUX_HYBRID_PADCTL_AUX_DRVZ_OHM_50 |
		DPAUX_HYBRID_PADCTL_AUX_CMH_V0_70 |
		0x18 << DPAUX_HYBRID_PADCTL_AUX_DRVI_SHIFT |
		DPAUX_HYBRID_PADCTL_AUX_INPUT_RCV_ENABLE);

	tegra_dpaux_writel(dp, DPAUX_HYBRID_SPARE,
			DPAUX_HYBRID_SPARE_PAD_PWR_POWERUP);
}

static void tegra_dc_dp_dump_link_cfg(struct tegra_dc_dp_data *dp,
	const struct tegra_dc_dp_link_config *link_cfg)
{
	printk(BIOS_INFO, "DP config: cfg_name               "
		"cfg_value\n");
	printk(BIOS_INFO, "           Lane Count             %d\n",
		link_cfg->max_lane_count);
	printk(BIOS_INFO, "           SupportEnhancedFraming %s\n",
		link_cfg->support_enhanced_framing ? "Y" : "N");
	printk(BIOS_INFO, "           Bandwidth              %d\n",
		link_cfg->max_link_bw);
	printk(BIOS_INFO, "           bpp                    %d\n",
		link_cfg->bits_per_pixel);
	printk(BIOS_INFO, "           EnhancedFraming        %s\n",
		link_cfg->enhanced_framing ? "Y" : "N");
	printk(BIOS_INFO, "           Scramble_enabled       %s\n",
		link_cfg->scramble_ena ? "Y" : "N");
	printk(BIOS_INFO, "           LinkBW                 %d\n",
		link_cfg->link_bw);
	printk(BIOS_INFO, "           lane_count             %d\n",
		link_cfg->lane_count);
	printk(BIOS_INFO, "           activespolarity        %d\n",
		link_cfg->activepolarity);
	printk(BIOS_INFO, "           active_count           %d\n",
		link_cfg->active_count);
	printk(BIOS_INFO, "           tu_size                %d\n",
		link_cfg->tu_size);
	printk(BIOS_INFO, "           active_frac            %d\n",
		link_cfg->active_frac);
	printk(BIOS_INFO, "           watermark              %d\n",
		link_cfg->watermark);
	printk(BIOS_INFO, "           hblank_sym             %d\n",
		link_cfg->hblank_sym);
	printk(BIOS_INFO, "           vblank_sym             %d\n",
		link_cfg->vblank_sym);
};

/* Calcuate if given cfg can meet the mode request. */
/* Return true if mode is possible, false otherwise. */
static int tegra_dc_dp_calc_config(struct tegra_dc_dp_data *dp,
	const struct soc_nvidia_tegra124_config *config,
	struct tegra_dc_dp_link_config *link_cfg)
{
	const u32	link_rate = 27 * link_cfg->link_bw * 1000 * 1000;
	const u64	f	  = 100000;	/* precision factor */

	u32	num_linkclk_line; /* Number of link clocks per line */
	u64	ratio_f; /* Ratio of incoming to outgoing data rate */

	u64	frac_f;
	u64	activesym_f;	/* Activesym per TU */
	u64	activecount_f;
	u32	activecount;
	u32	activepolarity;
	u64	approx_value_f;
	u32	activefrac		  = 0;
	u64	accumulated_error_f	  = 0;
	u32	lowest_neg_activecount	  = 0;
	u32	lowest_neg_activepolarity = 0;
	u32	lowest_neg_tusize	  = 64;
	u32	num_symbols_per_line;
	u64	lowest_neg_activefrac	  = 0;
	u64	lowest_neg_error_f	  = 64 * f;
	u64	watermark_f;

	int	i;
	int	neg;

	if (!link_rate || !link_cfg->lane_count || !config->pixel_clock ||
		!link_cfg->bits_per_pixel)
		return -1;

	if ((u64)config->pixel_clock * link_cfg->bits_per_pixel >=
		(u64)link_rate * 8 * link_cfg->lane_count)
		return -1;

	num_linkclk_line = (u32)((u64)link_rate * (u64)config->xres / config->pixel_clock);

	ratio_f = (u64)config->pixel_clock * link_cfg->bits_per_pixel * f;
	ratio_f /= 8;
	ratio_f = (u64)(ratio_f / (link_rate * link_cfg->lane_count));

	for (i = 64; i >= 32; --i) {
		activesym_f	= ratio_f * i;
		activecount_f	= (u64)(activesym_f / (u32)f) * f;
		frac_f		= activesym_f - activecount_f;
		activecount	= (u32)((u64)(activecount_f / (u32)f));

		if (frac_f < (f / 2)) /* fraction < 0.5 */
			activepolarity = 0;
		else {
			activepolarity = 1;
			frac_f = f - frac_f;
		}

		if (frac_f != 0) {
			frac_f = (u64)((f * f) / frac_f); /* 1/fraction */
			if (frac_f > (15 * f))
				activefrac = activepolarity ? 1 : 15;
			else
				activefrac = activepolarity ?
					(u32)((u64)(frac_f / (u32)f)) + 1 :
					(u32)((u64)(frac_f / (u32)f));
		}

		if (activefrac == 1)
			activepolarity = 0;

		if (activepolarity == 1)
			approx_value_f = activefrac ? (u64)(
				(activecount_f + (activefrac * f - f) * f) /
				(activefrac * f)) :
				activecount_f + f;
		else
			approx_value_f = activefrac ?
				activecount_f + (u64)(f / activefrac) :
				activecount_f;

		if (activesym_f < approx_value_f) {
			accumulated_error_f = num_linkclk_line *
				(u64)((approx_value_f - activesym_f) / i);
			neg = 1;
		} else {
			accumulated_error_f = num_linkclk_line *
				(u64)((activesym_f - approx_value_f) / i);
			neg = 0;
		}

		if ((neg && (lowest_neg_error_f > accumulated_error_f)) ||
			(accumulated_error_f == 0)) {
			lowest_neg_error_f = accumulated_error_f;
			lowest_neg_tusize = i;
			lowest_neg_activecount = activecount;
			lowest_neg_activepolarity = activepolarity;
			lowest_neg_activefrac = activefrac;

			if (accumulated_error_f == 0)
				break;
		}
	}

	if (lowest_neg_activefrac == 0) {
		link_cfg->activepolarity = 0;
		link_cfg->active_count   = lowest_neg_activepolarity ?
			lowest_neg_activecount : lowest_neg_activecount - 1;
		link_cfg->tu_size	      = lowest_neg_tusize;
		link_cfg->active_frac    = 1;
	} else {
		link_cfg->activepolarity = lowest_neg_activepolarity;
		link_cfg->active_count   = (u32)lowest_neg_activecount;
		link_cfg->tu_size	      = lowest_neg_tusize;
		link_cfg->active_frac    = (u32)lowest_neg_activefrac;
	}

	watermark_f = (u64)((ratio_f * link_cfg->tu_size * (f - ratio_f)) / f);
	link_cfg->watermark = (u32)((u64)((watermark_f + lowest_neg_error_f) /
		f)) + link_cfg->bits_per_pixel / 4 - 1;
	num_symbols_per_line = (config->xres * link_cfg->bits_per_pixel) /
		(8 * link_cfg->lane_count);

	if (link_cfg->watermark > 30) {
		printk(BIOS_INFO,
			"dp: sor setting: unable to get a good tusize, "
			"force watermark to 30.\n");
		link_cfg->watermark = 30;
		return -1;
	} else if (link_cfg->watermark > num_symbols_per_line) {
		printk(BIOS_INFO,
			"dp: sor setting: force watermark to the number "
			"of symbols in the line.\n");
		link_cfg->watermark = num_symbols_per_line;
		return -1;
	}

	/* Refer to dev_disp.ref for more information. */
	/* # symbols/hblank = ((SetRasterBlankEnd.X + SetRasterSize.Width - */
	/*                      SetRasterBlankStart.X - 7) * link_clk / pclk) */
	/*                      - 3 * enhanced_framing - Y */
	/* where Y = (# lanes == 4) 3 : (# lanes == 2) ? 6 : 12 */
	link_cfg->hblank_sym = (int)((u64)(((u64)(config->hback_porch +
			config->hfront_porch + config->hsync_width - 7) *
			link_rate) / config->pixel_clock)) -
			3 * link_cfg->enhanced_framing -
			(12 / link_cfg->lane_count);

	if (link_cfg->hblank_sym < 0)
		link_cfg->hblank_sym = 0;


	/* Refer to dev_disp.ref for more information. */
	/* # symbols/vblank = ((SetRasterBlankStart.X - */
	/*                      SetRasterBlankEen.X - 25) * link_clk / pclk) */
	/*                      - Y - 1; */
	/* where Y = (# lanes == 4) 12 : (# lanes == 2) ? 21 : 39 */
	link_cfg->vblank_sym = (int)((u64)((u64)(config->xres - 25)
			* link_rate / config->pixel_clock)) - (36 /
			link_cfg->lane_count) - 4;

	if (link_cfg->vblank_sym < 0)
		link_cfg->vblank_sym = 0;

	link_cfg->is_valid = 1;
	tegra_dc_dp_dump_link_cfg(dp, link_cfg);

	return 0;
}

static int tegra_dc_dp_init_link_cfg(
			struct soc_nvidia_tegra124_config *config,
			struct tegra_dc_dp_data *dp,
			struct tegra_dc_dp_link_config *link_cfg)
{
	u8 dpcd_data;
	int ret;

	link_cfg->max_lane_count = config->lane_count;
	link_cfg->support_enhanced_framing = config->enhanced_framing;
	link_cfg->max_link_bw = config->link_bw;
	link_cfg->drive_current = config->drive_current;
	link_cfg->preemphasis = config->preemphasis;
	link_cfg->postcursor = config->postcursor;
	link_cfg->bits_per_pixel = config->panel_bits_per_pixel;

	CHECK_RET(tegra_dc_dp_dpcd_read(dp, NV_DPCD_EDP_CONFIG_CAP,
			&dpcd_data));
	link_cfg->alt_scramber_reset_cap =
		(dpcd_data & NV_DPCD_EDP_CONFIG_CAP_ASC_RESET_YES) ?
		1 : 0;
	link_cfg->only_enhanced_framing =
		(dpcd_data & NV_DPCD_EDP_CONFIG_CAP_FRAMING_CHANGE_YES) ?
		1 : 0;

	link_cfg->lane_count = link_cfg->max_lane_count;
	link_cfg->link_bw = link_cfg->max_link_bw;
	link_cfg->enhanced_framing = link_cfg->support_enhanced_framing;

	tegra_dc_dp_calc_config(dp, config, link_cfg);
	return 0;
}

void dp_init(void * _config)
{
	struct soc_nvidia_tegra124_config *config = (void *)_config;
	struct tegra_dc *dc = config->dc_data;
	struct tegra_dc_dp_data *dp = &dp_data;

        // set up links among config, dc, dp and sor
        dp->dc = dc;
        dc->out = dp;
        dp->sor.dc = dc;

	dp->sor.power_is_up = 0;
	dp->sor.base = (void *)TEGRA_ARM_SOR;
	dp->sor.pmc_base = (void *)TEGRA_PMC_BASE;
	dp->sor.portnum = 0;
	dp->sor.link_cfg = &dp->link_cfg;
	dp->aux_base = (void *)TEGRA_ARM_DPAUX;
	dp->link_cfg.is_valid = 0;
	dp->enabled = 0;
}

static void tegra_dp_hpd_config(struct tegra_dc_dp_data *dp,
				struct soc_nvidia_tegra124_config *config)
{
	u32 val;

	val = config->hpd_plug_min_us |
		(config->hpd_unplug_min_us <<
		DPAUX_HPD_CONFIG_UNPLUG_MIN_TIME_SHIFT);
	tegra_dpaux_writel(dp, DPAUX_HPD_CONFIG, val);

	tegra_dpaux_writel(dp, DPAUX_HPD_IRQ_CONFIG, config->hpd_irq_min_us);
}

static int tegra_dp_hpd_plug(struct tegra_dc_dp_data *dp, int timeout_ms)
{
	u32 val;
	u32 timeout = timeout_ms * 1000;
	do {
		val = tegra_dpaux_readl(dp, DPAUX_DP_AUXSTAT);
		if (val & DPAUX_DP_AUXSTAT_HPD_STATUS_PLUGGED)
			return 0;
		udelay(100);
		timeout -= 100;
	} while (timeout > 0);
	return -1;
}

void dp_enable(void * _dp)
{
	struct tegra_dc_dp_data *dp = _dp;
	struct tegra_dc *dc = dp->dc;
	struct soc_nvidia_tegra124_config *config = dc->config;

	u8      data;
	u32     retry;
	int     ret;

	tegra_dc_dpaux_enable(dp);

	tegra_dp_hpd_config(dp, config);
	if (tegra_dp_hpd_plug(dp, config->vdd_to_hpd_delay_ms) < 0) {
		printk(BIOS_ERR, "dp: hpd plug failed\n");
		goto error_enable;
	}

	if (tegra_dc_dp_init_link_cfg(config, dp, &dp->link_cfg)) {
		printk(BIOS_ERR, "dp: failed to init link configuration\n");
		goto error_enable;
        }

	tegra_dc_sor_enable_dp(&dp->sor);

	tegra_dc_sor_set_panel_power(&dp->sor, 1);

	/* Write power on to DPCD */
	data = NV_DPCD_SET_POWER_VAL_D0_NORMAL;
	retry = 0;
	do {
		ret = tegra_dc_dp_dpcd_write(dp,
			NV_DPCD_SET_POWER, data);
	} while ((retry++ < DP_POWER_ON_MAX_TRIES) && ret);

	if (ret || retry >= DP_POWER_ON_MAX_TRIES) {
		printk(BIOS_ERR,
			"dp: failed to power on panel (0x%x)\n", ret);
		goto error_enable;
	}

	/* Confirm DP is plugging status */
	if (!(tegra_dpaux_readl(dp, DPAUX_DP_AUXSTAT) &
			DPAUX_DP_AUXSTAT_HPD_STATUS_PLUGGED)) {
		printk(BIOS_ERR, "dp: could not detect HPD\n");
		goto error_enable;
	}

	/* Check DP version */
	if (tegra_dc_dp_dpcd_read(dp, NV_DPCD_REV, &dp->revision))
		printk(BIOS_ERR,
			"dp: failed to read the revision number from sink\n");

	tegra_dc_sor_set_power_state(&dp->sor, 1);
	tegra_dc_sor_attach(&dp->sor);

	/*
	 * Power down the unused lanes to save power
	 * (about hundreds milli-watts, varies from boards).
	 */
	tegra_dc_sor_power_down_unused_lanes(&dp->sor);

	dp->enabled = 1;
error_enable:
	return;
}
