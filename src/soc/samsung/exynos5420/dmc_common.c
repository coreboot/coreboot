/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Samsung Electronics
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
 * Foundation, Inc.
 */

/* Mem setup common file for different types of DDR present on SMDK5420 boards.
 */

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <soc/clk.h>
#include <soc/dmc.h>
#include <soc/setup.h>

#define ZQ_INIT_TIMEOUT	10000

int dmc_config_zq(struct mem_timings *mem,
		  struct exynos5_phy_control *phy0_ctrl,
		  struct exynos5_phy_control *phy1_ctrl)
{
	unsigned long val = 0;
	int i;

	/*
	 * ZQ Calibration:
	 * Select Driver Strength,
	 * long calibration for manual calibration
	 */
	val = PHY_CON16_RESET_VAL;
	val |= mem->zq_mode_dds << PHY_CON16_ZQ_MODE_DDS_SHIFT;
	val |= mem->zq_mode_term << PHY_CON16_ZQ_MODE_TERM_SHIFT;
	val |= ZQ_CLK_DIV_EN;
	write32(&phy0_ctrl->phy_con16, val);
	write32(&phy1_ctrl->phy_con16, val);

	/* Disable termination */
	if (mem->zq_mode_noterm)
		val |= PHY_CON16_ZQ_MODE_NOTERM_MASK;
	write32(&phy0_ctrl->phy_con16, val);
	write32(&phy1_ctrl->phy_con16, val);

	/* ZQ_MANUAL_START: Enable */
	val |= ZQ_MANUAL_STR;
	write32(&phy0_ctrl->phy_con16, val);
	write32(&phy1_ctrl->phy_con16, val);

	/* ZQ_MANUAL_START: Disable */
	val &= ~ZQ_MANUAL_STR;

	/*
	 * Since we are manually calibrating the ZQ values,
	 * we are looping for the ZQ_init to complete.
	 */
	i = ZQ_INIT_TIMEOUT;
	while ((read32(&phy0_ctrl->phy_con17) & ZQ_DONE) != ZQ_DONE && i > 0) {
		udelay(1);
		i--;
	}
	if (!i)
		return -1;
	write32(&phy0_ctrl->phy_con16, val);

	i = ZQ_INIT_TIMEOUT;
	while ((read32(&phy1_ctrl->phy_con17) & ZQ_DONE) != ZQ_DONE && i > 0) {
		udelay(1);
		i--;
	}
	if (!i)
		return -1;
	write32(&phy1_ctrl->phy_con16, val);

	return 0;
}

void update_reset_dll(struct exynos5_dmc *dmc, enum ddr_mode mode)
{
	unsigned long val;

	if (mode == DDR_MODE_DDR3) {
		val = MEM_TERM_EN | PHY_TERM_EN | DMC_CTRL_SHGATE;
		write32(&dmc->phycontrol0, val);
	}

	/* Update DLL Information: Force DLL Resynchronization */
	val = read32(&dmc->phycontrol0);
	val |= FP_RSYNC;
	write32(&dmc->phycontrol0, val);

	/* Reset Force DLL Resynchronization */
	val = read32(&dmc->phycontrol0);
	val &= ~FP_RSYNC;
	write32(&dmc->phycontrol0, val);
}

void dmc_config_mrs(struct mem_timings *mem, struct exynos5_dmc *dmc)
{
	int channel, chip;

	for (channel = 0; channel < mem->dmc_channels; channel++) {
		unsigned long mask;

		mask = channel << DIRECT_CMD_CHANNEL_SHIFT;
		for (chip = 0; chip < mem->chips_to_configure; chip++) {
			int i;

			mask |= chip << DIRECT_CMD_CHIP_SHIFT;

			/* Sending NOP command */
			write32(&dmc->directcmd, DIRECT_CMD_NOP | mask);

			/*
			 * TODO(alim.akhtar@samsung.com): Do we need these
			 * delays? This one and the next were not there for
			 * DDR3.
			 */
			udelay(100);

			/* Sending EMRS/MRS commands */
			for (i = 0; i < MEM_TIMINGS_MSR_COUNT; i++) {
				write32(&dmc->directcmd,
					mem->direct_cmd_msr[i] | mask);
				udelay(100);
			}

			if (mem->send_zq_init) {
				/* Sending ZQINIT command */
				write32(&dmc->directcmd,
					DIRECT_CMD_ZQINIT | mask);
				/*
				 * FIXME: This was originally sdelay(10000)
				 * in the imported u-boot code. That may have
				 * been meant to be sdelay(0x10000) since that
				 * was used elsewhere in this function. Either
				 * way seems to work, though.
				 */
				udelay(12);
			}
		}
	}
}

void dmc_config_prech(struct mem_timings *mem, struct exynos5_dmc *dmc)
{
	int channel, chip;

	for (channel = 0; channel < mem->dmc_channels; channel++) {
		unsigned long mask;

		mask = channel << DIRECT_CMD_CHANNEL_SHIFT;
		for (chip = 0; chip < mem->chips_per_channel; chip++) {
			mask |= chip << DIRECT_CMD_CHIP_SHIFT;

			/* PALL (all banks precharge) CMD */
			write32(&dmc->directcmd, DIRECT_CMD_PALL | mask);
			udelay(100);
		}
	}
}
