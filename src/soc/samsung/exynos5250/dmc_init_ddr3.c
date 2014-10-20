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

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <soc/clk.h>
#include <soc/cpu.h>
#include <soc/dmc.h>
#include <soc/setup.h>

#define RDLVL_COMPLETE_TIMEOUT	10000

static void reset_phy_ctrl(void)
{
	writel(LPDDR3PHY_CTRL_PHY_RESET_ENABLE, &exynos_clock->lpddr3phy_ctrl);
	writel(LPDDR3PHY_CTRL_PHY_RESET_DISABLE, &exynos_clock->lpddr3phy_ctrl);

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
	int i;

	if (mem_reset)
		reset_phy_ctrl();

	/* Set Impedance Output Driver */
	val = (mem->impedance << CA_CK_DRVR_DS_OFFSET) |
		(mem->impedance << CA_CKE_DRVR_DS_OFFSET) |
		(mem->impedance << CA_CS_DRVR_DS_OFFSET) |
		(mem->impedance << CA_ADR_DRVR_DS_OFFSET);
	writel(val, &exynos_phy0_control->phy_con39);
	writel(val, &exynos_phy1_control->phy_con39);

	/* Set Read Latency and Burst Length for PHY0 and PHY1 */
	val = (mem->ctrl_bstlen << PHY_CON42_CTRL_BSTLEN_SHIFT) |
		(mem->ctrl_rdlat << PHY_CON42_CTRL_RDLAT_SHIFT);
	writel(val, &exynos_phy0_control->phy_con42);
	writel(val, &exynos_phy1_control->phy_con42);

	/* ZQ Calibration */
	if (dmc_config_zq(mem, exynos_phy0_control, exynos_phy1_control)){
		printk(BIOS_EMERG, "DRAM ZQ CALIBRATION FAILURE\n");
		return SETUP_ERR_ZQ_CALIBRATION_FAILURE;
	}

	/* DQ Signal */
	writel(mem->phy0_pulld_dqs, &exynos_phy0_control->phy_con14);
	writel(mem->phy1_pulld_dqs, &exynos_phy1_control->phy_con14);

	writel(mem->concontrol | (mem->rd_fetch << CONCONTROL_RD_FETCH_SHIFT)
		| (mem->dfi_init_start << CONCONTROL_DFI_INIT_START_SHIFT),
		&exynos_dmc->concontrol);

	update_reset_dll(exynos_dmc, DDR_MODE_DDR3);

	/* DQS Signal */
	writel(mem->phy0_dqs, &exynos_phy0_control->phy_con4);
	writel(mem->phy1_dqs, &exynos_phy1_control->phy_con4);

	writel(mem->phy0_dq, &exynos_phy0_control->phy_con6);
	writel(mem->phy1_dq, &exynos_phy1_control->phy_con6);

	writel(mem->phy0_tFS, &exynos_phy0_control->phy_con10);
	writel(mem->phy1_tFS, &exynos_phy1_control->phy_con10);

	val = (mem->ctrl_start_point << PHY_CON12_CTRL_START_POINT_SHIFT) |
		(mem->ctrl_inc << PHY_CON12_CTRL_INC_SHIFT) |
		(mem->ctrl_dll_on << PHY_CON12_CTRL_DLL_ON_SHIFT) |
		(mem->ctrl_ref << PHY_CON12_CTRL_REF_SHIFT);
	writel(val, &exynos_phy0_control->phy_con12);
	writel(val, &exynos_phy1_control->phy_con12);

	/* Start DLL locking */
	writel(val | (mem->ctrl_start << PHY_CON12_CTRL_START_SHIFT),
		&exynos_phy0_control->phy_con12);
	writel(val | (mem->ctrl_start << PHY_CON12_CTRL_START_SHIFT),
		&exynos_phy1_control->phy_con12);

	update_reset_dll(exynos_dmc, DDR_MODE_DDR3);

	writel(mem->concontrol | (mem->rd_fetch << CONCONTROL_RD_FETCH_SHIFT),
		&exynos_dmc->concontrol);

	/* Memory Channel Inteleaving Size */
	writel(mem->iv_size, &exynos_dmc->ivcontrol);

	/* Set DMC MEMCONTROL register */
	val = mem->memcontrol & ~DMC_MEMCONTROL_DSREF_ENABLE;
	writel(val, &exynos_dmc->memcontrol);

	writel(mem->memconfig, &exynos_dmc->memconfig0);
	writel(mem->memconfig, &exynos_dmc->memconfig1);
	writel(mem->membaseconfig0, &exynos_dmc->membaseconfig0);
	writel(mem->membaseconfig1, &exynos_dmc->membaseconfig1);

	/* Precharge Configuration */
	writel(mem->prechconfig_tp_cnt << PRECHCONFIG_TP_CNT_SHIFT,
		&exynos_dmc->prechconfig);

	/* Power Down mode Configuration */
	writel(mem->dpwrdn_cyc << PWRDNCONFIG_DPWRDN_CYC_SHIFT |
		mem->dsref_cyc << PWRDNCONFIG_DSREF_CYC_SHIFT,
		&exynos_dmc->pwrdnconfig);

	/* TimingRow, TimingData, TimingPower and Timingaref
	 * values as per Memory AC parameters
	 */
	writel(mem->timing_ref, &exynos_dmc->timingref);
	writel(mem->timing_row, &exynos_dmc->timingrow);
	writel(mem->timing_data, &exynos_dmc->timingdata);
	writel(mem->timing_power, &exynos_dmc->timingpower);

	/* Send PALL command */
	dmc_config_prech(mem, exynos_dmc);

	if (mem_reset) {
		/* Send NOP, MRS and ZQINIT commands.
		 * Sending MRS command will reset the DRAM. We should not be
		 * reseting the DRAM after resume, this will lead to memory
		 * corruption as DRAM content is lost after DRAM reset
		 */
		dmc_config_mrs(mem, exynos_dmc);
	}

	if (mem->gate_leveling_enable) {
		val = PHY_CON0_RESET_VAL;
		val |= P0_CMD_EN;
		writel(val, &exynos_phy0_control->phy_con0);
		writel(val, &exynos_phy1_control->phy_con0);

		val = PHY_CON2_RESET_VAL;
		val |= INIT_DESKEW_EN;
		writel(val, &exynos_phy0_control->phy_con2);
		writel(val, &exynos_phy1_control->phy_con2);

		val = PHY_CON0_RESET_VAL;
		val |= P0_CMD_EN;
		val |= BYTE_RDLVL_EN;
		writel(val, &exynos_phy0_control->phy_con0);
		writel(val, &exynos_phy1_control->phy_con0);

		val = (mem->ctrl_start_point <<
				PHY_CON12_CTRL_START_POINT_SHIFT) |
			(mem->ctrl_inc << PHY_CON12_CTRL_INC_SHIFT) |
			(mem->ctrl_force << PHY_CON12_CTRL_FORCE_SHIFT) |
			(mem->ctrl_start << PHY_CON12_CTRL_START_SHIFT) |
			(mem->ctrl_ref << PHY_CON12_CTRL_REF_SHIFT);
		writel(val, &exynos_phy0_control->phy_con12);
		writel(val, &exynos_phy1_control->phy_con12);

		val = PHY_CON2_RESET_VAL;
		val |= INIT_DESKEW_EN;
		val |= RDLVL_GATE_EN;
		writel(val, &exynos_phy0_control->phy_con2);
		writel(val, &exynos_phy1_control->phy_con2);

		val = PHY_CON0_RESET_VAL;
		val |= P0_CMD_EN;
		val |= BYTE_RDLVL_EN;
		val |= CTRL_SHGATE;
		writel(val, &exynos_phy0_control->phy_con0);
		writel(val, &exynos_phy1_control->phy_con0);

		val = PHY_CON1_RESET_VAL;
		val &= ~(CTRL_GATEDURADJ_MASK);
		writel(val, &exynos_phy0_control->phy_con1);
		writel(val, &exynos_phy1_control->phy_con1);

		writel(CTRL_RDLVL_GATE_ENABLE, &exynos_dmc->rdlvl_config);
		i = RDLVL_COMPLETE_TIMEOUT;
		while ((readl(&exynos_dmc->phystatus) &
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
			printk(BIOS_EMERG, "Timeout on RDLVL. No DRAM.\n");
			return SETUP_ERR_RDLV_COMPLETE_TIMEOUT;
		}
		writel(CTRL_RDLVL_GATE_DISABLE, &exynos_dmc->rdlvl_config);

		writel(0, &exynos_phy0_control->phy_con14);
		writel(0, &exynos_phy1_control->phy_con14);

		val = (mem->ctrl_start_point <<
				PHY_CON12_CTRL_START_POINT_SHIFT) |
			(mem->ctrl_inc << PHY_CON12_CTRL_INC_SHIFT) |
			(mem->ctrl_force << PHY_CON12_CTRL_FORCE_SHIFT) |
			(mem->ctrl_start << PHY_CON12_CTRL_START_SHIFT) |
			(mem->ctrl_dll_on << PHY_CON12_CTRL_DLL_ON_SHIFT) |
			(mem->ctrl_ref << PHY_CON12_CTRL_REF_SHIFT);
		writel(val, &exynos_phy0_control->phy_con12);
		writel(val, &exynos_phy1_control->phy_con12);

		update_reset_dll(exynos_dmc, DDR_MODE_DDR3);
	}

	/* Send PALL command */
	dmc_config_prech(mem, exynos_dmc);

	writel(mem->memcontrol, &exynos_dmc->memcontrol);

	/* Set DMC Concontrol and enable auto-refresh counter */
	writel(mem->concontrol | (mem->rd_fetch << CONCONTROL_RD_FETCH_SHIFT)
		| (mem->aref_en << CONCONTROL_AREF_EN_SHIFT),
		&exynos_dmc->concontrol);
	return 0;
}
