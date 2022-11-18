/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * drivers/video/tegra/dc/dp.c
 */

#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c_simple.h>
#include <edid.h>
#include <soc/addressmap.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/nvidia/tegra/dc.h>
#include <soc/nvidia/tegra/displayport.h>
#include <soc/sor.h>
#include <string.h>
#include <types.h>

#include "chip.h"

enum {
	DP_LT_SUCCESS = 0,
	DP_LT_FAILED = -1,
};

struct tegra_dc_dp_data dp_data;

static inline u32 tegra_dpaux_readl(struct tegra_dc_dp_data *dp, u32 reg)
{
	void *addr = dp->aux_base + (u32)(reg << 2);
	u32 reg_val = READL(addr);
	return reg_val;
}

static inline void tegra_dpaux_writel(struct tegra_dc_dp_data *dp,
					  u32 reg, u32 val)
{
	void *addr = dp->aux_base + (u32)(reg << 2);
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

static int tegra_dc_dpaux_read(struct tegra_dc_dp_data *dp, u32 cmd, u32 addr,
			u8 *data, u32 *size, u32 *aux_stat)
{
	u32 finished = 0;
	u32 cur_size;
	int ret = 0;

	do {
		cur_size = *size - finished;
		if (cur_size > DP_AUX_MAX_BYTES)
			cur_size = DP_AUX_MAX_BYTES;

		ret = tegra_dc_dpaux_read_chunk(dp, cmd, addr,
						data, &cur_size, aux_stat);
		if (ret)
			break;

		/* cur_size should be the real size returned */
		addr += cur_size;
		data += cur_size;
		finished += cur_size;

	} while (*size > finished);

	*size = finished;
	return ret;
}

static int tegra_dc_dp_dpcd_read(struct tegra_dc_dp_data *dp, u32 cmd,
				 u8 *data_ptr)
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

static int tegra_dc_i2c_aux_read(struct tegra_dc_dp_data *dp, u32 i2c_addr,
				 u8 addr, u8 *data, u32 *size, u32 *aux_stat)
{
	u32 finished = 0;
	int ret = 0;

	do {
		u32 cur_size = MIN(DP_AUX_MAX_BYTES, *size - finished);

		u32 len = 1;
		ret = tegra_dc_dpaux_write_chunk(
				dp, DPAUX_DP_AUXCTL_CMD_MOTWR, i2c_addr,
				&addr, &len, aux_stat);
		if (ret) {
			printk(BIOS_ERR, "%s: error sending address to read.\n",
			       __func__);
			break;
		}

		ret = tegra_dc_dpaux_read_chunk(
				dp, DPAUX_DP_AUXCTL_CMD_I2CRD, i2c_addr,
				data, &cur_size, aux_stat);
		if (ret) {
			printk(BIOS_ERR, "%s: error reading data.\n", __func__);
			break;
		}

		/* cur_size should be the real size returned */
		addr += cur_size;
		data += cur_size;
		finished += cur_size;
	} while (*size > finished);

	*size = finished;
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
}

static int _tegra_dp_lower_link_config(struct tegra_dc_dp_data *dp,
	struct tegra_dc_dp_link_config *cfg)
{

	switch (cfg->link_bw){
	case SOR_LINK_SPEED_G1_62:
		if (cfg->max_link_bw > SOR_LINK_SPEED_G1_62)
			cfg->link_bw = SOR_LINK_SPEED_G2_7;
		cfg->lane_count /= 2;
		break;
	case SOR_LINK_SPEED_G2_7:
		cfg->link_bw = SOR_LINK_SPEED_G1_62;
		break;
	case SOR_LINK_SPEED_G5_4:
		if (cfg->lane_count == 1) {
			cfg->link_bw = SOR_LINK_SPEED_G2_7;
			cfg->lane_count = cfg->max_lane_count;
		} else
			cfg->lane_count /= 2;
		break;
	default:
		printk(BIOS_ERR,"dp: Error link rate %d\n", cfg->link_bw);
		return DP_LT_FAILED;
	}

	return (cfg->lane_count > 0) ? DP_LT_SUCCESS : DP_LT_FAILED;
}

/* Calculate if given cfg can meet the mode request. */
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

static int tegra_dc_dp_init_max_link_cfg(
			struct soc_nvidia_tegra124_config *config,
			struct tegra_dc_dp_data *dp,
			struct tegra_dc_dp_link_config *link_cfg)
{
	u8 dpcd_data;
	int ret;

	CHECK_RET(tegra_dc_dp_dpcd_read(dp, NV_DPCD_MAX_LANE_COUNT,
			&dpcd_data));
	link_cfg->max_lane_count = dpcd_data & NV_DPCD_MAX_LANE_COUNT_MASK;
	link_cfg->tps3_supported = (dpcd_data &
		NV_DPCD_MAX_LANE_COUNT_TPS3_SUPPORTED_YES) ? 1 : 0;

	link_cfg->support_enhanced_framing =
		(dpcd_data & NV_DPCD_MAX_LANE_COUNT_ENHANCED_FRAMING_YES) ?
		1 : 0;

	CHECK_RET(tegra_dc_dp_dpcd_read(dp, NV_DPCD_MAX_DOWNSPREAD,
			&dpcd_data));
	link_cfg->downspread = (dpcd_data & NV_DPCD_MAX_DOWNSPREAD_VAL_0_5_PCT)?
				1 : 0;

	CHECK_RET(tegra_dc_dp_dpcd_read(dp, NV_DPCD_TRAINING_AUX_RD_INTERVAL,
			&link_cfg->aux_rd_interval));

	CHECK_RET(tegra_dc_dp_dpcd_read(dp, NV_DPCD_MAX_LINK_BANDWIDTH,
			&link_cfg->max_link_bw));

	link_cfg->bits_per_pixel = config->panel_bits_per_pixel;

	/*
	 * Set to a high value for link training and attach.
	 * Will be re-programmed when dp is enabled.
	 */
	link_cfg->drive_current = config->drive_current;
	link_cfg->preemphasis = config->preemphasis;
	link_cfg->postcursor = config->postcursor;

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

static int tegra_dc_dp_set_assr(struct tegra_dc_dp_data *dp, int ena)
{
	int ret;

	u8 dpcd_data = ena ?
		NV_DPCD_EDP_CONFIG_SET_ASC_RESET_ENABLE :
		NV_DPCD_EDP_CONFIG_SET_ASC_RESET_DISABLE;

	CHECK_RET(tegra_dc_dp_dpcd_write(dp, NV_DPCD_EDP_CONFIG_SET,
			dpcd_data));

	/* Also reset the scrambler to 0xfffe */
	tegra_dc_sor_set_internal_panel(&dp->sor, ena);
	return 0;
}

static int tegra_dp_set_link_bandwidth(struct tegra_dc_dp_data *dp, u8 link_bw)
{
	tegra_dc_sor_set_link_bandwidth(&dp->sor, link_bw);

	/* Sink side */
	return tegra_dc_dp_dpcd_write(dp, NV_DPCD_LINK_BANDWIDTH_SET, link_bw);
}

static int tegra_dp_set_lane_count(struct tegra_dc_dp_data *dp,
	const struct tegra_dc_dp_link_config *link_cfg)
{
	u8	dpcd_data;
	int	ret;

	/* check if panel support enhanched_framing */
	dpcd_data = link_cfg->lane_count;
	if (link_cfg->enhanced_framing)
		dpcd_data |= NV_DPCD_LANE_COUNT_SET_ENHANCEDFRAMING_T;
	CHECK_RET(tegra_dc_dp_dpcd_write(dp, NV_DPCD_LANE_COUNT_SET,
			dpcd_data));

	tegra_dc_sor_set_lane_count(&dp->sor, link_cfg->lane_count);

	/* Also power down lanes that will not be used */
	return 0;
}

static int tegra_dc_dp_link_trained(struct tegra_dc_dp_data *dp,
	const struct tegra_dc_dp_link_config *cfg)
{
	u32 lane;
	u8 mask;
	u8 data;
	int ret;

	for (lane = 0; lane < cfg->lane_count; ++lane) {
		CHECK_RET(tegra_dc_dp_dpcd_read(dp, (lane/2) ?
				NV_DPCD_LANE2_3_STATUS : NV_DPCD_LANE0_1_STATUS,
				&data));
		mask = (lane & 1) ?
			NV_DPCD_STATUS_LANEXPLUS1_CR_DONE_YES |
			NV_DPCD_STATUS_LANEXPLUS1_CHN_EQ_DONE_YES |
			NV_DPCD_STATUS_LANEXPLUS1_SYMBOL_LOCKED_YES :
			NV_DPCD_STATUS_LANEX_CR_DONE_YES |
			NV_DPCD_STATUS_LANEX_CHN_EQ_DONE_YES |
			NV_DPCD_STATUS_LANEX_SYMBOL_LOCKED_YES;
		if ((data & mask) != mask)
			return -1;
	}
	return 0;
}

static int tegra_dp_channel_eq_status(struct tegra_dc_dp_data *dp)
{
	u32 cnt;
	u32 n_lanes = dp->link_cfg.lane_count;
	u8 data;
	u8 ce_done = 1;

	for (cnt = 0; cnt < n_lanes / 2; cnt++) {
		tegra_dc_dp_dpcd_read(dp, (NV_DPCD_LANE0_1_STATUS + cnt), &data);

		if (n_lanes == 1) {
			ce_done = (data &
					(0x1 << NV_DPCD_STATUS_LANEX_CHN_EQ_DONE_SHIFT)) &&
			(data &	(0x1 << NV_DPCD_STATUS_LANEX_SYMBOL_LOCKED_SHFIT));
			break;
		} else if (!(data & (0x1 << NV_DPCD_STATUS_LANEX_CHN_EQ_DONE_SHIFT)) ||
			!(data & (0x1 << NV_DPCD_STATUS_LANEX_SYMBOL_LOCKED_SHFIT)) ||
			!(data & (0x1 << NV_DPCD_STATUS_LANEXPLUS1_CHN_EQ_DONE_SHIFT)) ||
			!(data & (0x1 << NV_DPCD_STATUS_LANEXPLUS1_SYMBOL_LOCKED_SHIFT)))
			return 0;
	}

	if (ce_done) {
		tegra_dc_dp_dpcd_read(dp, NV_DPCD_LANE_ALIGN_STATUS_UPDATED, &data);
		if (!(data & NV_DPCD_LANE_ALIGN_STATUS_UPDATED_DONE_YES))
				ce_done = 0;
	}

	return ce_done;
}

static u8 tegra_dp_clock_recovery_status(struct tegra_dc_dp_data *dp)
{
	u32 cnt;
	u32 n_lanes = dp->link_cfg.lane_count;
	u8 data_ptr;

	for (cnt = 0; cnt < n_lanes / 2; cnt++) {
		tegra_dc_dp_dpcd_read(dp,
			(NV_DPCD_LANE0_1_STATUS + cnt), &data_ptr);

		if (n_lanes == 1)
			return (data_ptr & NV_DPCD_STATUS_LANEX_CR_DONE_YES) ? 1 : 0;
		else if (!(data_ptr & NV_DPCD_STATUS_LANEX_CR_DONE_YES) ||
			!(data_ptr &
			(NV_DPCD_STATUS_LANEXPLUS1_CR_DONE_YES)))
			return 0;
	}

	return 1;
}

static void tegra_dp_lt_adjust(struct tegra_dc_dp_data *dp,
				u32 pe[4], u32 vs[4], u32 pc[4],
				u8 pc_supported)
{
	size_t cnt;
	u8 data_ptr;
	u32 n_lanes = dp->link_cfg.lane_count;

	for (cnt = 0; cnt < n_lanes / 2; cnt++) {
		tegra_dc_dp_dpcd_read(dp,
			(NV_DPCD_LANE0_1_ADJUST_REQ + cnt), &data_ptr);
		pe[2 * cnt] = (data_ptr & NV_DPCD_ADJUST_REQ_LANEX_PE_MASK) >>
					NV_DPCD_ADJUST_REQ_LANEX_PE_SHIFT;
		vs[2 * cnt] = (data_ptr & NV_DPCD_ADJUST_REQ_LANEX_DC_MASK) >>
					NV_DPCD_ADJUST_REQ_LANEX_DC_SHIFT;
		pe[1 + 2 * cnt] =
			(data_ptr & NV_DPCD_ADJUST_REQ_LANEXPLUS1_PE_MASK) >>
					NV_DPCD_ADJUST_REQ_LANEXPLUS1_PE_SHIFT;
		vs[1 + 2 * cnt] =
			(data_ptr & NV_DPCD_ADJUST_REQ_LANEXPLUS1_DC_MASK) >>
					NV_DPCD_ADJUST_REQ_LANEXPLUS1_DC_SHIFT;
	}
	if (pc_supported) {
		tegra_dc_dp_dpcd_read(dp,
				NV_DPCD_ADJUST_REQ_POST_CURSOR2, &data_ptr);
		for (cnt = 0; cnt < n_lanes; cnt++) {
			pc[cnt] = (data_ptr >>
			NV_DPCD_ADJUST_REQ_POST_CURSOR2_LANE_SHIFT(cnt)) &
			NV_DPCD_ADJUST_REQ_POST_CURSOR2_LANE_MASK;
		}
	}
}

static inline u32 tegra_dp_wait_aux_training(struct tegra_dc_dp_data *dp,
							u8 is_clk_recovery)
{
	if (!dp->link_cfg.aux_rd_interval)
		is_clk_recovery ? udelay(200) :
					udelay(500);
	else
		mdelay(dp->link_cfg.aux_rd_interval * 4);

	return dp->link_cfg.aux_rd_interval;
}

static void tegra_dp_tpg(struct tegra_dc_dp_data *dp, u32 tp, u32 n_lanes)
{
	u8 data = (tp == training_pattern_disabled)
		? (tp | NV_DPCD_TRAINING_PATTERN_SET_SC_DISABLED_F)
		: (tp | NV_DPCD_TRAINING_PATTERN_SET_SC_DISABLED_T);

	tegra_dc_sor_set_dp_linkctl(&dp->sor, 1, tp, &dp->link_cfg);
	tegra_dc_dp_dpcd_write(dp, NV_DPCD_TRAINING_PATTERN_SET, data);
}

static int tegra_dp_link_config(struct tegra_dc_dp_data *dp,
	const struct tegra_dc_dp_link_config *link_cfg)
{
	u8	dpcd_data;
	u32	retry;

	if (link_cfg->lane_count == 0) {
		printk(BIOS_ERR, "dp: error: lane count is 0. "
				"Can not set link config.\n");
		return DP_LT_FAILED;
	}

	/* Set power state if it is not in normal level */
	if (tegra_dc_dp_dpcd_read(dp, NV_DPCD_SET_POWER, &dpcd_data))
		return DP_LT_FAILED;

	if (dpcd_data == NV_DPCD_SET_POWER_VAL_D3_PWRDWN) {
		dpcd_data = NV_DPCD_SET_POWER_VAL_D0_NORMAL;

		/* DP spec requires 3 retries */
		for (retry = 3; retry > 0; --retry){
			if (tegra_dc_dp_dpcd_write(dp, NV_DPCD_SET_POWER, dpcd_data))
				break;
			if (retry == 1){
				printk(BIOS_ERR, "dp: Failed to set DP panel power\n");
				return DP_LT_FAILED;
			}
		}
	}

	/* Enable ASSR if possible */
	if (link_cfg->alt_scramber_reset_cap)
		if (tegra_dc_dp_set_assr(dp, 1))
			return DP_LT_FAILED;

	if (tegra_dp_set_link_bandwidth(dp, link_cfg->link_bw)) {
		printk(BIOS_ERR, "dp: Failed to set link bandwidth\n");
		return DP_LT_FAILED;
	}
	if (tegra_dp_set_lane_count(dp, link_cfg)) {
		printk(BIOS_ERR, "dp: Failed to set lane count\n");
		return DP_LT_FAILED;
	}
	tegra_dc_sor_set_dp_linkctl(&dp->sor, 1, training_pattern_none,
					link_cfg);
	return DP_LT_SUCCESS;
}

static int tegra_dp_lower_link_config(struct tegra_dc_dp_data *dp,
	struct tegra_dc_dp_link_config *cfg)
{
	struct tegra_dc_dp_link_config tmp_cfg;

	tmp_cfg = dp->link_cfg;
	cfg->is_valid = 0;

	if (_tegra_dp_lower_link_config(dp, cfg))
		goto fail;

	if (tegra_dc_dp_calc_config(dp, dp->dc->config, cfg))
		goto fail;
	tegra_dp_link_config(dp, cfg);

	return DP_LT_SUCCESS;
fail:
	dp->link_cfg = tmp_cfg;
	tegra_dp_link_config(dp, &tmp_cfg);
	return DP_LT_FAILED;
}

static void tegra_dp_lt_config(struct tegra_dc_dp_data *dp,
				u32 pe[4], u32 vs[4], u32 pc[4])
{
	struct tegra_dc_sor_data *sor = &dp->sor;
	u32 n_lanes = dp->link_cfg.lane_count;
	u8 pc_supported = dp->link_cfg.tps3_supported;
	u32 cnt;
	u32 val;

	for (cnt = 0; cnt < n_lanes; cnt++) {
		u32 mask = 0;
		u32 pe_reg, vs_reg, pc_reg;
		u32 shift = 0;

		switch (cnt) {
		case 0:
			mask = NV_SOR_PR_LANE2_DP_LANE0_MASK;
			shift = NV_SOR_PR_LANE2_DP_LANE0_SHIFT;
			break;
		case 1:
			mask = NV_SOR_PR_LANE1_DP_LANE1_MASK;
			shift = NV_SOR_PR_LANE1_DP_LANE1_SHIFT;
			break;
		case 2:
			mask = NV_SOR_PR_LANE0_DP_LANE2_MASK;
			shift = NV_SOR_PR_LANE0_DP_LANE2_SHIFT;
			break;
		case 3:
			mask = NV_SOR_PR_LANE3_DP_LANE3_MASK;
			shift = NV_SOR_PR_LANE3_DP_LANE3_SHIFT;
			break;
		default:
			printk(BIOS_ERR,
				"dp: incorrect lane cnt\n");
		}

		pe_reg = tegra_dp_pe_regs[pc[cnt]][vs[cnt]][pe[cnt]];
		vs_reg = tegra_dp_vs_regs[pc[cnt]][vs[cnt]][pe[cnt]];
		pc_reg = tegra_dp_pc_regs[pc[cnt]][vs[cnt]][pe[cnt]];

		tegra_dp_set_pe_vs_pc(sor, mask, pe_reg << shift,
				vs_reg << shift, pc_reg << shift, pc_supported);
	}

	tegra_dp_disable_tx_pu(&dp->sor);
	udelay(20);

	for (cnt = 0; cnt < n_lanes; cnt++) {
		u32 max_vs_flag = tegra_dp_is_max_vs(pe[cnt], vs[cnt]);
		u32 max_pe_flag = tegra_dp_is_max_pe(pe[cnt], vs[cnt]);

		val = (vs[cnt] << NV_DPCD_TRAINING_LANEX_SET_DC_SHIFT) |
			(max_vs_flag ?
			NV_DPCD_TRAINING_LANEX_SET_DC_MAX_REACHED_T :
			NV_DPCD_TRAINING_LANEX_SET_DC_MAX_REACHED_F) |
			(pe[cnt] << NV_DPCD_TRAINING_LANEX_SET_PE_SHIFT) |
			(max_pe_flag ?
			NV_DPCD_TRAINING_LANEX_SET_PE_MAX_REACHED_T :
			NV_DPCD_TRAINING_LANEX_SET_PE_MAX_REACHED_F);
		tegra_dc_dp_dpcd_write(dp,
			(NV_DPCD_TRAINING_LANE0_SET + cnt), val);
	}

	if (pc_supported) {
		for (cnt = 0; cnt < n_lanes / 2; cnt++) {
			u32 max_pc_flag0 = tegra_dp_is_max_pc(pc[cnt]);
			u32 max_pc_flag1 = tegra_dp_is_max_pc(pc[cnt + 1]);
			val = (pc[cnt] << NV_DPCD_LANEX_SET2_PC2_SHIFT) |
				(max_pc_flag0 ?
				NV_DPCD_LANEX_SET2_PC2_MAX_REACHED_T :
				NV_DPCD_LANEX_SET2_PC2_MAX_REACHED_F) |
				(pc[cnt + 1] <<
				NV_DPCD_LANEXPLUS1_SET2_PC2_SHIFT) |
				(max_pc_flag1 ?
				NV_DPCD_LANEXPLUS1_SET2_PC2_MAX_REACHED_T :
				NV_DPCD_LANEXPLUS1_SET2_PC2_MAX_REACHED_F);
			tegra_dc_dp_dpcd_write(dp,
				(NV_DPCD_TRAINING_LANE0_1_SET2 + cnt), val);
		}
	}
}

static int _tegra_dp_channel_eq(struct tegra_dc_dp_data *dp, u32 pe[4],
				u32 vs[4], u32 pc[4], u8 pc_supported,
				u32 n_lanes)
{
	u32 retry_cnt;

	for (retry_cnt = 0; retry_cnt < 4; retry_cnt++) {
		if (retry_cnt){
			tegra_dp_lt_adjust(dp, pe, vs, pc, pc_supported);
			tegra_dp_lt_config(dp, pe, vs, pc);
		}

		tegra_dp_wait_aux_training(dp, 0);

		if (!tegra_dp_clock_recovery_status(dp)) {
			printk(BIOS_ERR,"dp: CR failed in channel EQ sequence!\n");
			break;
		}

		if (tegra_dp_channel_eq_status(dp))
			return DP_LT_SUCCESS;
	}

	return DP_LT_FAILED;
}

static int tegra_dp_channel_eq(struct tegra_dc_dp_data *dp,
					u32 pe[4], u32 vs[4], u32 pc[4])
{
	u32 n_lanes = dp->link_cfg.lane_count;
	u8 pc_supported = dp->link_cfg.tps3_supported;
	int err;
	u32 tp_src = training_pattern_2;

	if (pc_supported)
		tp_src = training_pattern_3;

	tegra_dp_tpg(dp, tp_src, n_lanes);

	err = _tegra_dp_channel_eq(dp, pe, vs, pc, pc_supported, n_lanes);

	tegra_dp_tpg(dp, training_pattern_disabled, n_lanes);

	return err;
}

static int _tegra_dp_clk_recovery(struct tegra_dc_dp_data *dp, u32 pe[4],
					u32 vs[4], u32 pc[4], u8 pc_supported,
					u32 n_lanes)
{
	u32 vs_temp[4];
	u32 retry_cnt = 0;

	do {
		tegra_dp_lt_config(dp, pe, vs, pc);
		tegra_dp_wait_aux_training(dp, 1);

		if (tegra_dp_clock_recovery_status(dp))
			return DP_LT_SUCCESS;

		memcpy(vs_temp, vs, sizeof(vs_temp));
		tegra_dp_lt_adjust(dp, pe, vs, pc, pc_supported);

		if (memcmp(vs_temp, vs, sizeof(vs_temp)))
			retry_cnt = 0;
		else
			++retry_cnt;
	} while (retry_cnt < 5);

	return DP_LT_FAILED;
}

static int tegra_dp_clk_recovery(struct tegra_dc_dp_data *dp,
					u32 pe[4], u32 vs[4], u32 pc[4])
{
	u32 n_lanes = dp->link_cfg.lane_count;
	u8 pc_supported = dp->link_cfg.tps3_supported;
	int err;

	tegra_dp_tpg(dp, training_pattern_1, n_lanes);

	err = _tegra_dp_clk_recovery(dp, pe, vs, pc, pc_supported, n_lanes);
	if (err < 0)
		tegra_dp_tpg(dp, training_pattern_disabled, n_lanes);

	return err;
}

static int tegra_dc_dp_full_link_training(struct tegra_dc_dp_data *dp)
{
	struct tegra_dc_sor_data *sor = &dp->sor;
	int err;
	u32 pe[4], vs[4], pc[4];

	tegra_sor_precharge_lanes(sor);

retry_cr:
	memset(pe, preEmphasis_Disabled, sizeof(pe));
	memset(vs, driveCurrent_Level0, sizeof(vs));
	memset(pc, postCursor2_Level0, sizeof(pc));

	err = tegra_dp_clk_recovery(dp, pe, vs, pc);
	if (err != DP_LT_SUCCESS) {
		if (!tegra_dp_lower_link_config(dp, &dp->link_cfg))
			goto retry_cr;

		printk(BIOS_ERR, "dp: clk recovery failed\n");
		goto fail;
	}

	err = tegra_dp_channel_eq(dp, pe, vs, pc);
	if (err != DP_LT_SUCCESS) {
		if (!tegra_dp_lower_link_config(dp, &dp->link_cfg))
			goto retry_cr;

		printk(BIOS_ERR,
			"dp: channel equalization failed\n");
		goto fail;
	}

	tegra_dc_dp_dump_link_cfg(dp, &dp->link_cfg);

	return 0;

fail:
	return err;
}
/*
 * All link training functions are ported from kernel dc driver.
 * See more details at drivers/video/tegra/dc/dp.c
 */
static int tegra_dc_dp_fast_link_training(struct tegra_dc_dp_data *dp,
	const struct tegra_dc_dp_link_config *link_cfg)
{
	struct tegra_dc_sor_data *sor = &dp->sor;
	u8	link_bw;
	u8	lane_count;
	u16	data16;
	u32	data32;
	u32	size;
	u32	status;
	int	j;
	u32	mask = 0xffff >> ((4 - link_cfg->lane_count) * 4);

	tegra_dc_sor_set_lane_parm(sor, link_cfg);
	tegra_dc_dp_dpcd_write(dp, NV_DPCD_MAIN_LINK_CHANNEL_CODING_SET,
		NV_DPCD_MAIN_LINK_CHANNEL_CODING_SET_ANSI_8B10B);

	/* Send TP1 */
	tegra_dc_sor_set_dp_linkctl(sor, 1, training_pattern_1, link_cfg);
	tegra_dc_dp_dpcd_write(dp, NV_DPCD_TRAINING_PATTERN_SET,
		NV_DPCD_TRAINING_PATTERN_SET_TPS_TP1);

	for (j = 0; j < link_cfg->lane_count; ++j)
		tegra_dc_dp_dpcd_write(dp, NV_DPCD_TRAINING_LANE0_SET + j,
			0x24);
	udelay(520);

	size = sizeof(data16);
	tegra_dc_dpaux_read(dp, DPAUX_DP_AUXCTL_CMD_AUXRD,
		NV_DPCD_LANE0_1_STATUS, (u8 *)&data16, &size, &status);
	status = mask & 0x1111;
	if ((data16 & status) != status) {
		printk(BIOS_ERR,
			"dp: Link training error for TP1 (%#x)\n", data16);
		return -EFAULT;
	}

	/* enable ASSR */
	tegra_dc_dp_set_assr(dp, link_cfg->scramble_ena);
	tegra_dc_sor_set_dp_linkctl(sor, 1, training_pattern_3, link_cfg);

	tegra_dc_dp_dpcd_write(dp, NV_DPCD_TRAINING_PATTERN_SET,
		link_cfg->link_bw == 20 ? 0x23 : 0x22);
	for (j = 0; j < link_cfg->lane_count; ++j)
		tegra_dc_dp_dpcd_write(dp, NV_DPCD_TRAINING_LANE0_SET + j,
			0x24);
	udelay(520);

	size = sizeof(data32);
	tegra_dc_dpaux_read(dp, DPAUX_DP_AUXCTL_CMD_AUXRD,
		NV_DPCD_LANE0_1_STATUS, (u8 *)&data32, &size, &status);
	if ((data32 & mask) != (0x7777 & mask)) {
		printk(BIOS_ERR,
			"dp: Link training error for TP2/3 (0x%x)\n", data32);
		return -EFAULT;
	}

	tegra_dc_sor_set_dp_linkctl(sor, 1, training_pattern_disabled,
				link_cfg);
	tegra_dc_dp_dpcd_write(dp, NV_DPCD_TRAINING_PATTERN_SET, 0);

	if (tegra_dc_dp_link_trained(dp, link_cfg)) {
		tegra_dc_sor_read_link_config(&dp->sor, &link_bw,
			&lane_count);
		printk(BIOS_ERR,
			"Fast link trainging failed, link bw %d, lane # %d\n",
			link_bw, lane_count);
		return -EFAULT;
	}

	printk(BIOS_INFO,
		"Fast link trainging succeeded, link bw %d, lane %d\n",
		link_cfg->link_bw, link_cfg->lane_count);

	return 0;
}

static int tegra_dp_do_link_training(struct tegra_dc_dp_data *dp,
	const struct tegra_dc_dp_link_config *link_cfg)
{
	u8	link_bw;
	u8	lane_count;
	int	ret;

	/* Now do the fast link training for eDP */
	ret = tegra_dc_dp_fast_link_training(dp, link_cfg);
	if (ret) {
		printk(BIOS_ERR, "dp: fast link training failed\n");

		/* Try full link training then */
			if (tegra_dc_dp_full_link_training(dp)){
				printk(BIOS_ERR, "dp: full link training failed\n");
				return ret;
			}
	} else {
		/* set to a known-good drive setting if fast link succeeded */
		tegra_dc_sor_set_voltage_swing(&dp->sor);
	}

	/* Everything goes well, double check the link config */
	/* TODO: record edc/c2 data for debugging */
	tegra_dc_sor_read_link_config(&dp->sor, &link_bw, &lane_count);

	if ((link_cfg->link_bw == link_bw) &&
		(link_cfg->lane_count == lane_count))
		return 0;
	else
		return -EFAULT;
}

static int tegra_dc_dp_explore_link_cfg(struct tegra_dc_dp_data *dp,
	struct tegra_dc_dp_link_config *link_cfg,
	const struct soc_nvidia_tegra124_config *config)
{
	struct tegra_dc_dp_link_config temp_cfg;

	if (!config->pixel_clock || !config->xres || !config->yres) {
		printk(BIOS_ERR,
			"dp: error mode configuration");
		return -EINVAL;
	}
	if (!link_cfg->max_link_bw || !link_cfg->max_lane_count) {
		printk(BIOS_ERR,
			"dp: error link configuration");
		return -EINVAL;
	}

	link_cfg->is_valid = 0;

	memcpy(&temp_cfg, link_cfg, sizeof(temp_cfg));

	temp_cfg.link_bw = temp_cfg.max_link_bw;
	temp_cfg.lane_count = temp_cfg.max_lane_count;

	/*
	 * set to max link config
	 */
	if ((!tegra_dc_dp_calc_config(dp, config, &temp_cfg)) &&
		(!tegra_dp_link_config(dp, &temp_cfg)) &&
		(!tegra_dp_do_link_training(dp, &temp_cfg)))
		/* the max link cfg is doable */
		memcpy(link_cfg, &temp_cfg, sizeof(temp_cfg));

	return link_cfg->is_valid ? 0 : -EFAULT;
}

static void tegra_dp_update_config(struct tegra_dc_dp_data *dp,
				   struct soc_nvidia_tegra124_config *config)
{
	struct edid edid;
	u8 buf[128] = {0};
	u32 size = sizeof(buf), aux_stat = 0;

	tegra_dc_dpaux_enable(dp);
	if (tegra_dc_i2c_aux_read(dp, TEGRA_EDID_I2C_ADDRESS, 0, buf, &size,
				  &aux_stat)) {
		printk(BIOS_ERR, "%s: Failed to read EDID. Use defaults.\n",
		       __func__);
		return;
	}

	if (decode_edid(buf, sizeof(buf), &edid) != EDID_CONFORMANT) {
		printk(BIOS_ERR, "%s: Failed to decode EDID. Use defaults.\n",
		       __func__);
		return;
	}

	config->xres = edid.mode.ha;
	config->yres = edid.mode.va;
	config->pixel_clock = edid.mode.pixel_clock * 1000;

	config->hfront_porch = edid.mode.hso;
	config->hsync_width = edid.mode.hspw;
	config->hback_porch = edid.mode.hbl - edid.mode.hso - edid.mode.hspw;

	config->vfront_porch = edid.mode.vso;
	config->vsync_width = edid.mode.vspw;
	config->vback_porch = edid.mode.vbl - edid.mode.vso - edid.mode.vspw;

	/**
	 * Note edid->framebuffer_bits_per_pixel is currently hard-coded as 32,
	 * so we should keep the default value in device config.
	 *
	 * EDID v1.3 panels may not have color depth info, so we need to check
	 * if these values are zero before updating config.
	 */
	if (edid.panel_bits_per_pixel)
		config->panel_bits_per_pixel = edid.panel_bits_per_pixel;
	if (edid.panel_bits_per_color)
		config->color_depth = edid.panel_bits_per_color;
	printk(BIOS_SPEW, "%s: configuration updated by EDID.\n", __func__);
}

void dp_init(void *_config)
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

	tegra_dp_update_config(dp, config);
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

void dp_enable(void *_dp)
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

	if (tegra_dc_dp_init_max_link_cfg(config, dp, &dp->link_cfg)) {
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

	if (tegra_dc_dp_explore_link_cfg(dp, &dp->link_cfg, config)) {
		printk(BIOS_ERR, "dp: error to configure link\n");
		goto error_enable;
	}

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
