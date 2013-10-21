/*
 * drivers/video/tegra/dc/sor.c
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

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <lib.h>
#include <stdlib.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <device/device.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include <boot/tables.h>
#include <cbmem.h>
#include <soc/nvidia/tegra/dc.h>
#include "sor.h"
#include <soc/nvidia/tegra/displayport.h>
#include "clk_rst.h"
#include <soc/clock.h>
#include "chip.h"
#include <soc/display.h>

#define APBDEV_PMC_DPD_SAMPLE				(0x20)
#define APBDEV_PMC_DPD_SAMPLE_ON_DISABLE		(0)
#define APBDEV_PMC_DPD_SAMPLE_ON_ENABLE			(1)
#define APBDEV_PMC_SEL_DPD_TIM				(0x1c8)
#define APBDEV_PMC_SEL_DPD_TIM_SEL_DPD_TIM_DEFAULT	(0x7f)
#define APBDEV_PMC_IO_DPD2_REQ				(0x1c0)
#define APBDEV_PMC_IO_DPD2_REQ_LVDS_SHIFT		(25)
#define APBDEV_PMC_IO_DPD2_REQ_LVDS_OFF			(0 << 25)
#define APBDEV_PMC_IO_DPD2_REQ_LVDS_ON			(1 << 25)
#define APBDEV_PMC_IO_DPD2_REQ_CODE_SHIFT               (30)
#define APBDEV_PMC_IO_DPD2_REQ_CODE_DEFAULT_MASK        (0x3 << 30)
#define APBDEV_PMC_IO_DPD2_REQ_CODE_IDLE                (0 << 30)
#define APBDEV_PMC_IO_DPD2_REQ_CODE_DPD_OFF             (1 << 30)
#define APBDEV_PMC_IO_DPD2_REQ_CODE_DPD_ON              (2 << 30)
#define APBDEV_PMC_IO_DPD2_STATUS			(0x1c4)
#define APBDEV_PMC_IO_DPD2_STATUS_LVDS_SHIFT		(25)
#define APBDEV_PMC_IO_DPD2_STATUS_LVDS_OFF		(0 << 25)
#define APBDEV_PMC_IO_DPD2_STATUS_LVDS_ON		(1 << 25)


static inline u32 tegra_sor_readl(struct tegra_dc_sor_data *sor, u32 reg)
{
	u32 reg_val = readl((sor->base + reg * 4));
	return reg_val;
}

static inline void tegra_sor_writel(struct tegra_dc_sor_data *sor,
	u32 reg, u32 val)
{
	writel(val, (sor->base + reg * 4));
}

static inline void tegra_sor_write_field(struct tegra_dc_sor_data *sor,
	u32 reg, u32 mask, u32 val)
{
	u32 reg_val = tegra_sor_readl(sor, reg);
	reg_val &= ~mask;
	reg_val |= val;
	tegra_sor_writel(sor, reg, reg_val);
}

void tegra_dc_sor_set_dp_linkctl(struct tegra_dc_sor_data *sor, int ena,
	u8 training_pattern, const struct tegra_dc_dp_link_config *cfg)
{
	u32 reg_val;

	reg_val = tegra_sor_readl(sor, NV_SOR_DP_LINKCTL(sor->portnum));

	if (ena)
		reg_val |= NV_SOR_DP_LINKCTL_ENABLE_YES;
	else
		reg_val &= NV_SOR_DP_LINKCTL_ENABLE_NO;

	reg_val &= ~NV_SOR_DP_LINKCTL_TUSIZE_MASK;
	reg_val |= (cfg->tu_size << NV_SOR_DP_LINKCTL_TUSIZE_SHIFT);

	if (cfg->enhanced_framing)
		reg_val |= NV_SOR_DP_LINKCTL_ENHANCEDFRAME_ENABLE;

	tegra_sor_writel(sor, NV_SOR_DP_LINKCTL(sor->portnum), reg_val);

	switch (training_pattern) {
	case trainingPattern_1:
		tegra_sor_writel(sor, NV_SOR_DP_TPG, 0x41414141);
		break;
	case trainingPattern_2:
	case trainingPattern_3:
		reg_val = (cfg->link_bw == NV_SOR_LINK_SPEED_G5_4) ?
			0x43434343 : 0x42424242;
		tegra_sor_writel(sor, NV_SOR_DP_TPG, reg_val);
		break;
	default:
		tegra_sor_writel(sor, NV_SOR_DP_TPG, 0x50505050);
		break;
	}
}

void tegra_dc_sor_set_dp_lanedata(struct tegra_dc_sor_data *sor,
	u32 lane, u32 pre_emphasis, u32 drive_current, u32 tx_pu)
{
	u32 d_cur;
	u32 p_emp;


	d_cur = tegra_sor_readl(sor, NV_SOR_DC(sor->portnum));
	p_emp = tegra_sor_readl(sor, NV_SOR_PR(sor->portnum));

	switch (lane) {
	case 0:
		p_emp &= ~NV_SOR_PR_LANE2_DP_LANE0_MASK;
		p_emp |= (pre_emphasis <<
			NV_SOR_PR_LANE2_DP_LANE0_SHIFT);
		d_cur &= ~NV_SOR_DC_LANE2_DP_LANE0_MASK;
		d_cur |= (drive_current <<
			NV_SOR_DC_LANE2_DP_LANE0_SHIFT);
		break;
	case 1:
		p_emp &= ~NV_SOR_PR_LANE1_DP_LANE1_MASK;
		p_emp |= (pre_emphasis <<
			NV_SOR_PR_LANE1_DP_LANE1_SHIFT);
		d_cur &= ~NV_SOR_DC_LANE1_DP_LANE1_MASK;
		d_cur |= (drive_current <<
			NV_SOR_DC_LANE1_DP_LANE1_SHIFT);
		break;
	case 2:
		p_emp &= ~NV_SOR_PR_LANE0_DP_LANE2_MASK;
		p_emp |= (pre_emphasis <<
			NV_SOR_PR_LANE0_DP_LANE2_SHIFT);
		d_cur &= ~NV_SOR_DC_LANE0_DP_LANE2_MASK;
		d_cur |= (drive_current <<
			NV_SOR_DC_LANE0_DP_LANE2_SHIFT);
		break;
	case 3:
		p_emp &= ~NV_SOR_PR_LANE3_DP_LANE3_MASK;
		p_emp |= (pre_emphasis <<
			NV_SOR_PR_LANE3_DP_LANE3_SHIFT);
		d_cur &= ~NV_SOR_DC_LANE3_DP_LANE3_MASK;
		d_cur |= (drive_current <<
			NV_SOR_DC_LANE3_DP_LANE3_SHIFT);
		break;
	default:
		printk(BIOS_SPEW, "dp: sor lane count %d is invalid\n", lane);
	}

	tegra_sor_write_field(sor, NV_SOR_DP_LINKCTL(sor->portnum),
		NV_SOR_DP_PADCTL_TX_PU_VALUE_DEFAULT_MASK,
		tx_pu << NV_SOR_DP_PADCTL_TX_PU_VALUE_SHIFT);

	tegra_sor_writel(sor, NV_SOR_DC(sor->portnum), d_cur);
	tegra_sor_writel(sor, NV_SOR_PR(sor->portnum), p_emp);
}

