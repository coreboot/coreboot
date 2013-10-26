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
#include "sor.h"
#include <soc/nvidia/tegra/displayport.h>

extern int dump;
unsigned long READL(void *p);
void WRITEL(unsigned long value, void *p);

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
											   u32 timeout_ms)
{
	u32 reg_val = 0;

	printk(BIOS_SPEW, "JZ: %s: enter, poll_reg: %#x: timeout: 0x%x\n",
		   __func__, reg * 4, timeout_ms);
	do {
		udelay(1);
		reg_val = tegra_dpaux_readl(dp, reg);
	} while (((reg_val & mask) != exp_val) && (--timeout_ms > 0));

	if ((reg_val & mask) == exp_val)
		return 0;	/* success */
	printk(BIOS_SPEW,
		   "dpaux_poll_register 0x%x: timeout: "
		   "(reg_val)0x%08x & (mask)0x%08x != (exp_val)0x%08x\n",
		   reg, reg_val, mask, exp_val);
	return timeout_ms;
}

static inline int tegra_dpaux_wait_transaction(struct tegra_dc_dp_data *dp)
{
	/* According to DP spec, each aux transaction needs to finish
	   within 40ms. */
	if (tegra_dc_dpaux_poll_register(dp, DPAUX_DP_AUXCTL,
									 DPAUX_DP_AUXCTL_TRANSACTREQ_MASK,
									 DPAUX_DP_AUXCTL_TRANSACTREQ_DONE,
									 100, DP_AUX_TIMEOUT_MS * 1000) != 0) {
		printk(BIOS_SPEW, "dp: DPAUX transaction timeout\n");
		return -1;
	}
	return 0;
}

static int tegra_dc_dpaux_write_chunk(struct tegra_dc_dp_data *dp, u32 cmd,
									  u32 addr, u8 * data, u32 * size,
									  u32 * aux_stat)
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
			printk(BIOS_SPEW, "dp: aux write cmd 0x%x is invalid\n", cmd);
			return -1;
	}

#if 0
/* interesting. */
	if (tegra_platform_is_silicon()) {
		*aux_stat = tegra_dpaux_readl(dp, DPAUX_DP_AUXSTAT);
		if (!(*aux_stat & DPAUX_DP_AUXSTAT_HPD_STATUS_PLUGGED)) {
			printk(BIOS_SPEW, "dp: HPD is not detected\n");
			return -EFAULT;
		}
	}
#endif

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
			printk(BIOS_SPEW, "dp: aux write transaction timeout\n");

		*aux_stat = tegra_dpaux_readl(dp, DPAUX_DP_AUXSTAT);

		if ((*aux_stat & DPAUX_DP_AUXSTAT_TIMEOUT_ERROR_PENDING) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_RX_ERROR_PENDING) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_SINKSTAT_ERROR_PENDING) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_NO_STOP_ERROR_PENDING)) {
			if (timeout_retries-- > 0) {
				printk(BIOS_SPEW, "dp: aux write retry (0x%x) -- %d\n",
					   *aux_stat, timeout_retries);
				/* clear the error bits */
				tegra_dpaux_writel(dp, DPAUX_DP_AUXSTAT, *aux_stat);
				continue;
			} else {
				printk(BIOS_SPEW, "dp: aux write got error (0x%x)\n",
					   *aux_stat);
				return -1;
			}
		}

		if ((*aux_stat & DPAUX_DP_AUXSTAT_REPLYTYPE_I2CDEFER) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_REPLYTYPE_DEFER)) {
			if (defer_retries-- > 0) {
				printk(BIOS_SPEW, "dp: aux write defer (0x%x) -- %d\n",
					   *aux_stat, defer_retries);
				/* clear the error bits */
				tegra_dpaux_writel(dp, DPAUX_DP_AUXSTAT, *aux_stat);
				continue;
			} else {
				printk(BIOS_SPEW, "dp: aux write defer exceeds max retries "
					   "(0x%x)\n", *aux_stat);
				return -1;
			}
		}

		if ((*aux_stat & DPAUX_DP_AUXSTAT_REPLYTYPE_MASK) ==
			DPAUX_DP_AUXSTAT_REPLYTYPE_ACK) {
			*size = ((*aux_stat) & DPAUX_DP_AUXSTAT_REPLY_M_MASK);
			return 0;
		} else {
			printk(BIOS_SPEW, "dp: aux write failed (0x%x)\n", *aux_stat);
			return -1;
		}
	}
	/* Should never come to here */
	return -1;
}

static int tegra_dc_dpaux_write(struct tegra_dc_dp_data *dp, u32 cmd, u32 addr,
								u8 * data, u32 * size, u32 * aux_stat)
{
	u32 cur_size = 0;
	u32 finished = 0;
	u32 cur_left;
	int ret = 0;

	do {
		cur_size = *size - finished;
		if (cur_size > DP_AUX_MAX_BYTES)
			cur_size = DP_AUX_MAX_BYTES;
		cur_left = cur_size;
		ret = tegra_dc_dpaux_write_chunk(dp, cmd, addr,
										 data, &cur_left, aux_stat);

		cur_size -= cur_left;
		finished += cur_size;
		addr += cur_size;
		data += cur_size;

		if (ret)
			break;
	} while (*size > finished);

	*size = finished;
	return ret;
}

static int tegra_dc_dpaux_read_chunk(struct tegra_dc_dp_data *dp, u32 cmd,
									 u32 addr, u8 * data, u32 * size,
									 u32 * aux_stat)
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
			printk(BIOS_SPEW, "dp: aux read cmd 0x%x is invalid\n", cmd);
			return -1;
	}

	if (0) {
		*aux_stat = tegra_dpaux_readl(dp, DPAUX_DP_AUXSTAT);
		if (!(*aux_stat & DPAUX_DP_AUXSTAT_HPD_STATUS_PLUGGED)) {
			printk(BIOS_SPEW, "dp: HPD is not detected\n");
			//return EFAULT;
		}
	}

	tegra_dpaux_writel(dp, DPAUX_DP_AUXADDR, addr);

	reg_val = tegra_dpaux_readl(dp, DPAUX_DP_AUXCTL);
	reg_val &= ~DPAUX_DP_AUXCTL_CMD_MASK;
	reg_val |= cmd;
	printk(BIOS_SPEW, "cmd = %08x\n", reg_val);
	reg_val &= ~DPAUX_DP_AUXCTL_CMDLEN_FIELD;
	reg_val |= ((*size - 1) << DPAUX_DP_AUXCTL_CMDLEN_SHIFT);
	printk(BIOS_SPEW, "cmd = %08x\n", reg_val);
	while ((timeout_retries > 0) && (defer_retries > 0)) {
		if ((timeout_retries != DP_AUX_TIMEOUT_MAX_TRIES) ||
			(defer_retries != DP_AUX_DEFER_MAX_TRIES))
			udelay(DP_DPCP_RETRY_SLEEP_NS * 2);

		reg_val |= DPAUX_DP_AUXCTL_TRANSACTREQ_PENDING;
		printk(BIOS_SPEW, "cmd = %08x\n", reg_val);
		tegra_dpaux_writel(dp, DPAUX_DP_AUXCTL, reg_val);

		if (tegra_dpaux_wait_transaction(dp))
			printk(BIOS_SPEW, "dp: aux read transaction timeout\n");

		*aux_stat = tegra_dpaux_readl(dp, DPAUX_DP_AUXSTAT);
		printk(BIOS_SPEW, "dp: %s: aux stat: 0x%08x\n", __func__, *aux_stat);

		if ((*aux_stat & DPAUX_DP_AUXSTAT_TIMEOUT_ERROR_PENDING) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_RX_ERROR_PENDING) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_SINKSTAT_ERROR_PENDING) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_NO_STOP_ERROR_PENDING)) {
			if (timeout_retries-- > 0) {
				printk(BIOS_SPEW, "dp: aux read retry (0x%x) -- %d\n",
					   *aux_stat, timeout_retries);
				/* clear the error bits */
				tegra_dpaux_writel(dp, DPAUX_DP_AUXSTAT, *aux_stat);
				continue;	/* retry */
			} else {
				printk(BIOS_SPEW, "dp: aux read got error (0x%x)\n", *aux_stat);
				return -1;
			}
		}

		if ((*aux_stat & DPAUX_DP_AUXSTAT_REPLYTYPE_I2CDEFER) ||
			(*aux_stat & DPAUX_DP_AUXSTAT_REPLYTYPE_DEFER)) {
			if (defer_retries-- > 0) {
				printk(BIOS_SPEW, "dp: aux read defer (0x%x) -- %d\n",
					   *aux_stat, defer_retries);
				/* clear the error bits */
				tegra_dpaux_writel(dp, DPAUX_DP_AUXSTAT, *aux_stat);
				continue;
			} else {
				printk(BIOS_SPEW, "dp: aux read defer exceeds max retries "
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
			printk(BIOS_SPEW, "dp: aux read data %d bytes\n", *size);
			memcpy(data, temp_data, *size);

			return 0;
		} else {
			printk(BIOS_SPEW, "dp: aux read failed (0x%x\n", *aux_stat);
			return -1;
		}
	}
	/* Should never come to here */
	printk(BIOS_SPEW, "%s: can't\n", __func__);
	return -1;
}

int tegra_dc_dpaux_read(struct tegra_dc_dp_data *dp, u32 cmd, u32 addr,
						u8 * data, u32 * size, u32 * aux_stat)
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

		/* cur_size should be the real size returned */
		addr += cur_size;
		data += cur_size;
		finished += cur_size;

		if (ret)
			break;

#if 0
		if (cur_size == 0) {
			printk(BIOS_SPEW, "JZ: no data found, ret\n");
			break;
		}
#endif
	} while (*size > finished);

	*size = finished;
	return ret;
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
		printk(BIOS_SPEW,
			   "dp: Failed to read DPCD data. CMD 0x%x, Status 0x%x\n", cmd,
			   status);

	return ret;
}

static int tegra_dc_dp_init_max_link_cfg(struct tegra_dc_dp_data *dp,
										 struct tegra_dc_dp_link_config *cfg)
{
	u8 dpcd_data;
	int ret;

	ret = tegra_dc_dp_dpcd_read(dp, NV_DPCD_MAX_LANE_COUNT, &dpcd_data);
	if (ret)
		return ret;

	cfg->max_lane_count = dpcd_data & NV_DPCD_MAX_LANE_COUNT_MASK;
	printk(BIOS_SPEW, "JZ: %s: max_lane_count: %d\n", __func__,
		   cfg->max_lane_count);

	cfg->support_enhanced_framing =
		(dpcd_data & NV_DPCD_MAX_LANE_COUNT_ENHANCED_FRAMING_YES) ? 1 : 0;
	printk(BIOS_SPEW, "JZ: %s: enh-framing: %d\n", __func__,
		   cfg->support_enhanced_framing);

	ret = tegra_dc_dp_dpcd_read(dp, NV_DPCD_MAX_DOWNSPREAD, &dpcd_data);
	if (ret)
		return ret;
	cfg->downspread = (dpcd_data & NV_DPCD_MAX_DOWNSPREAD_VAL_0_5_PCT) ? 1 : 0;
	printk(BIOS_SPEW, "JZ: %s: downspread: %d\n", __func__, cfg->downspread);

	ret = tegra_dc_dp_dpcd_read(dp, NV_DPCD_MAX_LINK_BANDWIDTH,
								&cfg->max_link_bw);
	if (ret)
		return ret;
	printk(BIOS_SPEW, "JZ: %s: max_link_bw: %d\n", __func__, cfg->max_link_bw);

	// jz, changed
	// cfg->bits_per_pixel = dp->dc->pdata->default_out->depth;
	cfg->bits_per_pixel = 18;

	/* TODO: need to come from the board file */
	/* Venice2 settings */
	cfg->drive_current = 0x20202020;
	cfg->preemphasis = 0;
	cfg->postcursor = 0;

	ret = tegra_dc_dp_dpcd_read(dp, NV_DPCD_EDP_CONFIG_CAP, &dpcd_data);
	if (ret)
		return ret;
	cfg->alt_scramber_reset_cap =
		(dpcd_data & NV_DPCD_EDP_CONFIG_CAP_ASC_RESET_YES) ? 1 : 0;
	cfg->only_enhanced_framing =
		(dpcd_data & NV_DPCD_EDP_CONFIG_CAP_FRAMING_CHANGE_YES) ? 1 : 0;
	printk(BIOS_SPEW, "JZ: %s: alt_reset_cap: %d, only_enh_framing: %d\n",
		   __func__, cfg->alt_scramber_reset_cap, cfg->only_enhanced_framing);

	cfg->lane_count = cfg->max_lane_count;
	cfg->link_bw = NV_SOR_LINK_SPEED_G1_62;
	cfg->enhanced_framing = cfg->support_enhanced_framing;
	return 0;
}

struct tegra_dc_dp_data dp_data;

static int tegra_dc_dpcd_read_rev(struct tegra_dc_dp_data *dp, u8 * rev)
{
	u32 size;
	int ret;
	u32 status = 0;

	size = 3;
	ret = tegra_dc_dpaux_read(dp, DPAUX_DP_AUXCTL_CMD_AUXRD,
							  NV_DPCD_REV, rev, &size, &status);
	if (ret) {
		printk(BIOS_SPEW, "dp: Failed to read NV_DPCD_REV\n");
		return ret;
	}
	return 0;
}

u32 dp_setup_timing(u32 panel_id, u32 width, u32 height);
void dp_bringup(u32 winb_addr)
{
	struct tegra_dc_dp_data *dp = &dp_data;

	u32 dpcd_rev;
	u32 pclk_freq;

	u32 xres = 1366;	/* norrin display */
	u32 yres = 768;

	printk(BIOS_SPEW, "JZ: %s: entry\n", __func__);

	dp->sor.base = (void *)TEGRA_ARM_SOR;
	dp->sor.portnum = 0;

	dp->aux_base = (void *)TEGRA_ARM_DPAUX;

	/* read panel info */
	if (!tegra_dc_dpcd_read_rev(dp, (u8 *) & dpcd_rev)) {
		printk(BIOS_SPEW, "PANEL info: \n");
		printk(BIOS_SPEW, "--DPCP version(%#x): %d.%d\n",
			   dpcd_rev, (dpcd_rev >> 4) & 0x0f, (dpcd_rev & 0x0f));
	}

	if (tegra_dc_dp_init_max_link_cfg(dp, &dp->link_cfg))
		printk(BIOS_SPEW, "dp: failed to init link configuration\n");

	dp_link_training((u32) (dp->link_cfg.lane_count),
					 (u32) (dp->link_cfg.link_bw));

	pclk_freq = dp_setup_timing(5, xres, yres);
	printk(BIOS_SPEW, "JZ: %s: pclk_freq: %d\n", __func__, pclk_freq);

	void dp_misc_setting(u32 panel_bpp, u32 width, u32 height, u32 winb_addr,
						 u32 lane_count, u32 enhanced_framing, u32 panel_edp,
						 u32 pclkfreq, u32 linkfreq);

	dp_misc_setting(dp->link_cfg.bits_per_pixel,
					xres, yres, winb_addr,
					(u32) dp->link_cfg.lane_count,
					(u32) dp->link_cfg.enhanced_framing,
					(u32) dp->link_cfg.alt_scramber_reset_cap,
					pclk_freq, dp->link_cfg.link_bw * 27);

}

void debug_dpaux_print(u32 addr, u32 size)
{
	struct tegra_dc_dp_data *dp = &dp_data;
	u32 status = 0;
	u8 buf[16];
	int i;

	if ((size == 0) || (size > 16)) {
		printk(BIOS_SPEW, "dp: %s: invalid size %d\n", __func__, size);
		return;
	}

	if (tegra_dc_dpaux_read(dp, DPAUX_DP_AUXCTL_CMD_AUXRD,
							addr, buf, &size, &status)) {
		printk(BIOS_SPEW, "******AuxRead Error: 0x%04x: status 0x%08x\n", addr,
			   status);
		return;
	}
	printk(BIOS_SPEW, "%s: addr: 0x%04x, size: %d\n", __func__, addr, size);
	for (i = 0; i < size; ++i)
		printk(BIOS_SPEW, " %02x", buf[i]);

	printk(BIOS_SPEW, "\n");
}

int dpaux_read(u32 addr, u32 size, u8 * data)
{

	struct tegra_dc_dp_data *dp = &dp_data;
	u32 status = 0;

	if ((size == 0) || (size > 16)) {
		printk(BIOS_SPEW, "dp: %s: invalid size %d\n", __func__, size);
		return -1;
	}

	if (tegra_dc_dpaux_read(dp, DPAUX_DP_AUXCTL_CMD_AUXRD,
							addr, data, &size, &status)) {
		printk(BIOS_SPEW, "dp: Failed to read reg %#x, status: %#x\n", addr,
			   status);
		return -1;
	}

	return 0;
}

int dpaux_write(u32 addr, u32 size, u32 data)
{
	struct tegra_dc_dp_data *dp = &dp_data;
	u32 status = 0;
	int ret;

	printk(BIOS_SPEW, "JZ: %s: entry, addr: 0x%08x, size: 0x%08x, data: %#x\n",
		   __func__, addr, size, data);

	ret = tegra_dc_dpaux_write(dp, DPAUX_DP_AUXCTL_CMD_AUXWR,
							   addr, (u8 *) & data, &size, &status);
	if (ret)
		printk(BIOS_SPEW, "dp: Failed to write to reg %#x, status: 0x%x\n",
			   addr, status);
	return ret;
}
