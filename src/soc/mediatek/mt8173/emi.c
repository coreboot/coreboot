/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/dramc_common.h>
#include <soc/dramc_register.h>
#include <soc/dramc_pi_api.h>
#include <soc/dramc_soc.h>
#include <soc/mt6391.h>
#include <soc/pmic_wrap.h>
#include <soc/pll.h>
#include <soc/infracfg.h>
#include <soc/pericfg.h>

struct emi_regs *emi_regs = (void *)EMI_BASE;

static void dram_vcore_adjust(void)
{
	/* options: Vcore_HV_LPPDR3/Vcore_NV_LPPDR3/Vcore_LV_LPPDR3 */
	pwrap_write_field(PMIC_RG_VCORE_CON9, Vcore_NV_LPPDR3, 0x7F, 0);
	pwrap_write_field(PMIC_RG_VCORE_CON10, Vcore_NV_LPPDR3, 0x7F, 0);
}

static void dram_vmem_adjust(void)
{
	/* options: Vmem_HV_LPPDR3/Vmem_NV_LPPDR3/Vmem_LV_LPPDR3 */
	pwrap_write_field(PMIC_RG_VDRM_CON9, Vmem_NV_LPDDR3, 0x7F, 0);
	pwrap_write_field(PMIC_RG_VDRM_CON10, Vmem_NV_LPDDR3, 0x7F, 0);
}

static void emi_init(const struct mt8173_sdram_params *sdram_params)
{
	/* EMI setting initialization */
	write32(&emi_regs->emi_conf, sdram_params->emi_set.conf);
	write32(&emi_regs->emi_conm, sdram_params->emi_set.conm_1);
	write32(&emi_regs->emi_arbi, sdram_params->emi_set.arbi);
	write32(&emi_regs->emi_arba, sdram_params->emi_set.arba);
	write32(&emi_regs->emi_arbc, sdram_params->emi_set.arbc);
	write32(&emi_regs->emi_arbd, sdram_params->emi_set.arbd);
	write32(&emi_regs->emi_arbe, sdram_params->emi_set.arbe);
	write32(&emi_regs->emi_arbf, sdram_params->emi_set.arbf);
	write32(&emi_regs->emi_arbg, sdram_params->emi_set.arbg);
	write32(&emi_regs->emi_arbj, sdram_params->emi_set.arbj);
	write32(&emi_regs->emi_cona, sdram_params->emi_set.cona);
	write32(&emi_regs->emi_testd, sdram_params->emi_set.testd);
	write32(&emi_regs->emi_bmen, sdram_params->emi_set.bmen);
	write32(&emi_regs->emi_conb, sdram_params->emi_set.conb);
	write32(&emi_regs->emi_conc, sdram_params->emi_set.conc);
	write32(&emi_regs->emi_cond, sdram_params->emi_set.cond);
	write32(&emi_regs->emi_cone, sdram_params->emi_set.cone);
	write32(&emi_regs->emi_cong, sdram_params->emi_set.cong);
	write32(&emi_regs->emi_conh, sdram_params->emi_set.conh);
	write32(&emi_regs->emi_slct, sdram_params->emi_set.slct_1);
	write32(&emi_regs->emi_mdct, sdram_params->emi_set.mdct_1);
	write32(&emi_regs->emi_arbk, sdram_params->emi_set.arbk);
	write32(&emi_regs->emi_testc, sdram_params->emi_set.testc);
	write32(&emi_regs->emi_mdct, sdram_params->emi_set.mdct_2);
	write32(&emi_regs->emi_testb, sdram_params->emi_set.testb);
	write32(&emi_regs->emi_slct, sdram_params->emi_set.slct_2);
	write32(&emi_regs->emi_conm, sdram_params->emi_set.conm_2);
	write32(&emi_regs->emi_test0, sdram_params->emi_set.test0);
	write32(&emi_regs->emi_test1, sdram_params->emi_set.test1);
}

static void do_calib(const struct mt8173_sdram_params *sdram_params)
{
	u32 channel;

	sw_impedance_cal(CHANNEL_A, sdram_params);
	sw_impedance_cal(CHANNEL_B, sdram_params);

	/* SPM_CONTROL_AFTERK */
	transfer_to_reg_control();

	/* do dram calibration for channel A and B */
	for (channel = 0; channel < CHANNEL_NUM; channel++) {
		ca_training(channel, sdram_params);
		write_leveling(channel, sdram_params);

		/* rx gating and datlat for single or dual rank */
		if (is_dual_rank(channel, sdram_params)) {
			dual_rank_rx_dqs_gating_cal(channel, sdram_params);
			dual_rank_rx_datlat_cal(channel, sdram_params);
		} else {
			rx_dqs_gating_cal(channel, 0, sdram_params);
			rx_datlat_cal(channel, 0, sdram_params);
		}

		clk_duty_cal(channel);
		/* rx window perbit calibration */
		perbit_window_cal(channel, RX_WIN);
		/* tx window perbit calibration */
		perbit_window_cal(channel, TX_WIN);

		dramc_rankinctl_config(channel, sdram_params);
		dramc_runtime_config(channel, sdram_params);
	}

	/* SPM_CONTROL_AFTERK */
	transfer_to_spm_control();
}

static void init_dram(const struct mt8173_sdram_params *sdram_params)
{
	emi_init(sdram_params);

	dramc_pre_init(CHANNEL_A, sdram_params);
	dramc_pre_init(CHANNEL_B, sdram_params);

	div2_phase_sync();

	dramc_init(CHANNEL_A, sdram_params);
	dramc_init(CHANNEL_B, sdram_params);
}

size_t sdram_size(void)
{
	u32 value = read32(&emi_regs->emi_cona);
	u32 bit_counter = 0;

	/* check if dual channel */
	if (value & CONA_DUAL_CH_EN)
		bit_counter++;

	/* check if 32bit, 32 = 2^5*/
	if (value & CONA_32BIT_EN)
		bit_counter += 5;
	else
		bit_counter += 4;

	/* check column address */
	/* 00 is 9 bits, 01 is 10 bits, 10 is 11 bits */
	bit_counter += ((value & COL_ADDR_BITS_MASK) >> COL_ADDR_BITS_SHIFT) +
		       9;

	/* check if row address */
	/* 00 is 13 bits, 01 is 14 bits, 10 is 15bits, 11 is 16 bits */
	bit_counter += ((value & ROW_ADDR_BITS_MASK) >> ROW_ADDR_BITS_SHIFT) +
		       13;

	/* check if dual rank */
	if (value & CONA_DUAL_RANK_EN)
		bit_counter++;

	/* add bank address bit, LPDDR3 is 8 banks =2^3 */
	bit_counter += 3;

	/* transform bits to bytes */
	return ((size_t)1 << (bit_counter - 3));
}

static void init_4GB_mode(void)
{
	if (sdram_size() == (size_t)4 * GiB) {
		setbits32(&mt8173_pericfg->axi_bus_ctl3, PERISYS_4G_SUPPORT);
		setbits32(&mt8173_infracfg->infra_misc, DDR_4GB_SUPPORT_EN);
	} else {
		clrbits32(&mt8173_pericfg->axi_bus_ctl3, PERISYS_4G_SUPPORT);
		clrbits32(&mt8173_infracfg->infra_misc, DDR_4GB_SUPPORT_EN);
	}
}

void mt_set_emi(const struct mt8173_sdram_params *sdram_params)
{
	/* voltage info */
	dram_vcore_adjust();
	dram_vmem_adjust();

	if (sdram_params->type != TYPE_LPDDR3) {
		die("The DRAM type is not supported");
	}

	init_dram(sdram_params);
	do_calib(sdram_params);
	init_4GB_mode();
}
