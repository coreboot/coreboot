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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* DDR3 mem setup file for SMDK5250 board based on EXYNOS5 */

#include <console/console.h>
#include <delay.h>
#include <arch/io.h>
#include "clk.h"
#include "cpu.h"
#include "dmc.h"
#include "setup.h"

#define RDLVL_COMPLETE_TIMEOUT	10000

static void reset_phy_ctrl(void)
{
	struct exynos5_clock *clk = (struct exynos5_clock *)EXYNOS5_CLOCK_BASE;

	writel(LPDDR3PHY_CTRL_PHY_RESET_OFF, &clk->lpddr3phy_ctrl);
	writel(LPDDR3PHY_CTRL_PHY_RESET, &clk->lpddr3phy_ctrl);

#if 0
	/*
	 * For proper memory initialization there should be a minimum delay of
	 * 500us after the LPDDR3PHY_CTRL_PHY_RESET signal.
	 * The below value is an approximate value whose calculation in done
	 * considering that sdelay takes 2 instruction for every 1 delay cycle.
	 * And assuming each instruction takes 1 clock cycle i.e 1/(1.7 Ghz)sec
	 * So for 500 usec, the number of delay cycle should be
	 * (500 * 10^-6) * (1.7 * 10^9) / 2 = 425000
	 *
	 * TODO(hatim.rv@samsung.com): Implement the delay using timer/counter
	 */
	sdelay(425000);
#endif
	udelay(500);
}

int ddr3_mem_ctrl_init(struct mem_timings *mem, unsigned long mem_iv_size,
		       int mem_reset)
{
	unsigned int val;
	struct exynos5_phy_control *phy0_ctrl, *phy1_ctrl;
	struct exynos5_dmc *dmc;
	int i;

	phy0_ctrl = (struct exynos5_phy_control *)EXYNOS5_DMC_PHY0_BASE;
	phy1_ctrl = (struct exynos5_phy_control *)EXYNOS5_DMC_PHY1_BASE;
	dmc = (struct exynos5_dmc *)EXYNOS5_DMC_CTRL_BASE;

	if (mem_reset) {
		printk(BIOS_SPEW, "%s: reset phy: ", __func__);
		reset_phy_ctrl();
		printk(BIOS_SPEW, "done\n");
	} else {
		printk(BIOS_SPEW, "%s: skip mem_reset.\n", __func__);
	}

	/* Set Impedance Output Driver */
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: Set Impedance Output Driver\n");
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: mem->impedance 0x%x\n",
	       mem->impedance);
	val = (mem->impedance << CA_CK_DRVR_DS_OFFSET) |
		(mem->impedance << CA_CKE_DRVR_DS_OFFSET) |
		(mem->impedance << CA_CS_DRVR_DS_OFFSET) |
		(mem->impedance << CA_ADR_DRVR_DS_OFFSET);
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: val 0x%x\n", val);
	writel(val, &phy0_ctrl->phy_con39);
	writel(val, &phy1_ctrl->phy_con39);

	/* Set Read Latency and Burst Length for PHY0 and PHY1 */
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: "
	       "Set Read Latency and Burst Length for PHY0 and PHY1\n");
	val = (mem->ctrl_bstlen << PHY_CON42_CTRL_BSTLEN_SHIFT) |
		(mem->ctrl_rdlat << PHY_CON42_CTRL_RDLAT_SHIFT);
	writel(val, &phy0_ctrl->phy_con42);
	writel(val, &phy1_ctrl->phy_con42);

	/* ZQ Calibration */
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: ZQ Calibration\n");
	if (dmc_config_zq(mem, phy0_ctrl, phy1_ctrl))
		return SETUP_ERR_ZQ_CALIBRATION_FAILURE;

	/* DQ Signal */
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: DQ Signal\n");
	writel(mem->phy0_pulld_dqs, &phy0_ctrl->phy_con14);
	writel(mem->phy1_pulld_dqs, &phy1_ctrl->phy_con14);

	writel(mem->concontrol | (mem->rd_fetch << CONCONTROL_RD_FETCH_SHIFT)
		| (mem->dfi_init_start << CONCONTROL_DFI_INIT_START_SHIFT),
		&dmc->concontrol);

	update_reset_dll(dmc, DDR_MODE_DDR3);

	/* DQS Signal */
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: DQS Signal\n");
	writel(mem->phy0_dqs, &phy0_ctrl->phy_con4);
	writel(mem->phy1_dqs, &phy1_ctrl->phy_con4);

	writel(mem->phy0_dq, &phy0_ctrl->phy_con6);
	writel(mem->phy1_dq, &phy1_ctrl->phy_con6);

	writel(mem->phy0_tFS, &phy0_ctrl->phy_con10);
	writel(mem->phy1_tFS, &phy1_ctrl->phy_con10);

	val = (mem->ctrl_start_point << PHY_CON12_CTRL_START_POINT_SHIFT) |
		(mem->ctrl_inc << PHY_CON12_CTRL_INC_SHIFT) |
		(mem->ctrl_dll_on << PHY_CON12_CTRL_DLL_ON_SHIFT) |
		(mem->ctrl_ref << PHY_CON12_CTRL_REF_SHIFT);
	writel(val, &phy0_ctrl->phy_con12);
	writel(val, &phy1_ctrl->phy_con12);

	/* Start DLL locking */
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: Start DLL Locking\n");
	writel(val | (mem->ctrl_start << PHY_CON12_CTRL_START_SHIFT),
		&phy0_ctrl->phy_con12);
	writel(val | (mem->ctrl_start << PHY_CON12_CTRL_START_SHIFT),
		&phy1_ctrl->phy_con12);

	update_reset_dll(dmc, DDR_MODE_DDR3);

	writel(mem->concontrol | (mem->rd_fetch << CONCONTROL_RD_FETCH_SHIFT),
		&dmc->concontrol);

	/* Memory Channel Interleaving Size */
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: "
	       "Memory Channel Interleaving Size\n");
	writel(mem->iv_size, &dmc->ivcontrol);

	/* Set DMC MEMCONTROL register */
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: Set DMC MEMCONTROL register\n");
	val = mem->memcontrol & ~DMC_MEMCONTROL_DSREF_ENABLE;
	writel(val, &dmc->memcontrol);

	writel(mem->memconfig, &dmc->memconfig0);
	writel(mem->memconfig, &dmc->memconfig1);
	writel(mem->membaseconfig0, &dmc->membaseconfig0);
	writel(mem->membaseconfig1, &dmc->membaseconfig1);

	/* Precharge Configuration */
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: Precharge Configuration\n");
	writel(mem->prechconfig_tp_cnt << PRECHCONFIG_TP_CNT_SHIFT,
		&dmc->prechconfig);

	/* Power Down mode Configuration */
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: "
	       "Power Down mode Configuration\n");
	writel(mem->dpwrdn_cyc << PWRDNCONFIG_DPWRDN_CYC_SHIFT |
		mem->dsref_cyc << PWRDNCONFIG_DSREF_CYC_SHIFT,
		&dmc->pwrdnconfig);

	/* TimingRow, TimingData, TimingPower and Timingaref
	 * values as per Memory AC parameters
	 */
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: "
	       "TimingRow, TimingData, TimingPower and Timingaref\n");
	writel(mem->timing_ref, &dmc->timingref);
	writel(mem->timing_row, &dmc->timingrow);
	writel(mem->timing_data, &dmc->timingdata);
	writel(mem->timing_power, &dmc->timingpower);

	/* Send PALL command */
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: Send PALL Command\n");
	dmc_config_prech(mem, dmc);

	/* Send NOP, MRS and ZQINIT commands */
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: Send NOP, MRS, and ZQINIT\n");
	dmc_config_mrs(mem, dmc);

	if (mem->gate_leveling_enable) {
		val = PHY_CON0_RESET_VAL;
		val |= P0_CMD_EN;
		writel(val, &phy0_ctrl->phy_con0);
		writel(val, &phy1_ctrl->phy_con0);

		val = PHY_CON2_RESET_VAL;
		val |= INIT_DESKEW_EN;
		writel(val, &phy0_ctrl->phy_con2);
		writel(val, &phy1_ctrl->phy_con2);

		val = PHY_CON0_RESET_VAL;
		val |= P0_CMD_EN;
		val |= BYTE_RDLVL_EN;
		writel(val, &phy0_ctrl->phy_con0);
		writel(val, &phy1_ctrl->phy_con0);

		val = (mem->ctrl_start_point <<
				PHY_CON12_CTRL_START_POINT_SHIFT) |
			(mem->ctrl_inc << PHY_CON12_CTRL_INC_SHIFT) |
			(mem->ctrl_force << PHY_CON12_CTRL_FORCE_SHIFT) |
			(mem->ctrl_start << PHY_CON12_CTRL_START_SHIFT) |
			(mem->ctrl_ref << PHY_CON12_CTRL_REF_SHIFT);
		writel(val, &phy0_ctrl->phy_con12);
		writel(val, &phy1_ctrl->phy_con12);

		val = PHY_CON2_RESET_VAL;
		val |= INIT_DESKEW_EN;
		val |= RDLVL_GATE_EN;
		writel(val, &phy0_ctrl->phy_con2);
		writel(val, &phy1_ctrl->phy_con2);

		val = PHY_CON0_RESET_VAL;
		val |= P0_CMD_EN;
		val |= BYTE_RDLVL_EN;
		val |= CTRL_SHGATE;
		writel(val, &phy0_ctrl->phy_con0);
		writel(val, &phy1_ctrl->phy_con0);

		val = PHY_CON1_RESET_VAL;
		val &= ~(CTRL_GATEDURADJ_MASK);
		writel(val, &phy0_ctrl->phy_con1);
		writel(val, &phy1_ctrl->phy_con1);

		writel(CTRL_RDLVL_GATE_ENABLE, &dmc->rdlvl_config);
		i = RDLVL_COMPLETE_TIMEOUT;
		while ((readl(&dmc->phystatus) &
			(RDLVL_COMPLETE_CHO | RDLVL_COMPLETE_CH1)) !=
			(RDLVL_COMPLETE_CHO | RDLVL_COMPLETE_CH1) && i > 0) {
			/*
			 * TODO(waihong): Comment on how long this take to
			 * timeout
			 */
			udelay(1);
			i--;
		}
		if (!i){
			printk(BIOS_SPEW, "Timeout on RDLVL. No DRAM.\n");
			return SETUP_ERR_RDLV_COMPLETE_TIMEOUT;
		}
		writel(CTRL_RDLVL_GATE_DISABLE, &dmc->rdlvl_config);

		writel(0, &phy0_ctrl->phy_con14);
		writel(0, &phy1_ctrl->phy_con14);

		val = (mem->ctrl_start_point <<
				PHY_CON12_CTRL_START_POINT_SHIFT) |
			(mem->ctrl_inc << PHY_CON12_CTRL_INC_SHIFT) |
			(mem->ctrl_force << PHY_CON12_CTRL_FORCE_SHIFT) |
			(mem->ctrl_start << PHY_CON12_CTRL_START_SHIFT) |
			(mem->ctrl_dll_on << PHY_CON12_CTRL_DLL_ON_SHIFT) |
			(mem->ctrl_ref << PHY_CON12_CTRL_REF_SHIFT);
		writel(val, &phy0_ctrl->phy_con12);
		writel(val, &phy1_ctrl->phy_con12);

		update_reset_dll(dmc, DDR_MODE_DDR3);
	}

	/* Send PALL command */
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: Send PALL Command\n");
	dmc_config_prech(mem, dmc);

	writel(mem->memcontrol, &dmc->memcontrol);

	/* Set DMC Concontrol and enable auto-refresh counter */
	printk(BIOS_SPEW, "ddr3_mem_ctrl_init: "
	       "Set DMC Concontrol and enable auto-refresh counter\n");
	writel(mem->concontrol | (mem->rd_fetch << CONCONTROL_RD_FETCH_SHIFT)
		| (mem->aref_en << CONCONTROL_AREF_EN_SHIFT), &dmc->concontrol);
	return 0;
}
