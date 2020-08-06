/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Taken and adapted from U-Boot.
 */

#include "sdram.h"
#include <types.h>
#include <device/mmio.h>
#include <delay.h>
#include "clock.h"

static struct vtp_reg *vtpreg[2] = {(struct vtp_reg *)VTP0_CTRL_ADDR,
				    (struct vtp_reg *)VTP1_CTRL_ADDR};

/**
 * Base address for EMIF instances
 */
static struct emif_reg_struct *emif_reg[2] = {(struct emif_reg_struct *)EMIF4_0_CFG_BASE,
					      (struct emif_reg_struct *)EMIF4_1_CFG_BASE};

/**
 * Base addresses for DDR PHY cmd/data regs
 */
static struct ddr_cmd_regs *ddr_cmd_reg[2] = {(struct ddr_cmd_regs *)DDR_PHY_CMD_ADDR,
					      (struct ddr_cmd_regs *)DDR_PHY_CMD_ADDR2};

static struct ddr_data_regs *ddr_data_reg[2] = {(struct ddr_data_regs *)DDR_PHY_DATA_ADDR,
						(struct ddr_data_regs *)DDR_PHY_DATA_ADDR2};

/**
 * Base address for ddr io control instances
 */
static struct ddr_cmdtctrl *ioctrl_reg = {(struct ddr_cmdtctrl *)DDR_CONTROL_BASE_ADDR};

struct ctrl_stat *cstat = (struct ctrl_stat *)CTRL_BASE;

static struct ddr_ctrl *ddrctrl = (struct ddr_ctrl *)DDR_CTRL_ADDR;


static void config_vtp(int nr)
{
	write32(&vtpreg[nr]->vtp0ctrlreg, read32(&vtpreg[nr]->vtp0ctrlreg) | VTP_CTRL_ENABLE);
	write32(&vtpreg[nr]->vtp0ctrlreg,
		read32(&vtpreg[nr]->vtp0ctrlreg) & (~VTP_CTRL_START_EN));
	write32(&vtpreg[nr]->vtp0ctrlreg, read32(&vtpreg[nr]->vtp0ctrlreg) | VTP_CTRL_START_EN);

	/* Poll for READY */
	while ((read32(&vtpreg[nr]->vtp0ctrlreg) & VTP_CTRL_READY) != VTP_CTRL_READY)
		;
}

/**
 * Configure SDRAM
 */
static void config_sdram(const struct emif_regs *regs, int nr)
{
	if (regs->zq_config) {
		write32(&emif_reg[nr]->emif_zq_config, regs->zq_config);
		write32(&cstat->secure_emif_sdram_config, regs->sdram_config);
		write32(&emif_reg[nr]->emif_sdram_config, regs->sdram_config);

		/* Trigger initialization */
		write32(&emif_reg[nr]->emif_sdram_ref_ctrl, 0x00003100);
		/* Wait 1ms because of L3 timeout error */
		udelay(1000);

		/* Write proper sdram_ref_cref_ctrl value */
		write32(&emif_reg[nr]->emif_sdram_ref_ctrl, regs->ref_ctrl);
		write32(&emif_reg[nr]->emif_sdram_ref_ctrl_shdw, regs->ref_ctrl);
	}
	write32(&emif_reg[nr]->emif_sdram_ref_ctrl, regs->ref_ctrl);
	write32(&emif_reg[nr]->emif_sdram_ref_ctrl_shdw, regs->ref_ctrl);
	write32(&emif_reg[nr]->emif_sdram_config, regs->sdram_config);

	/* Write REG_COS_COUNT_1, REG_COS_COUNT_2, and REG_PR_OLD_COUNT. */
	if (regs->ocp_config)
		write32(&emif_reg[nr]->emif_l3_config, regs->ocp_config);
}

/**
 * Configure DDR DATA registers
 */
static void config_ddr_data(const struct ddr_data *data, int nr)
{
	int i;

	if (!data)
		return;

	for (i = 0; i < DDR_DATA_REGS_NR; i++) {
		write32(&(ddr_data_reg[nr] + i)->dt0rdsratio0, data->datardsratio0);
		write32(&(ddr_data_reg[nr] + i)->dt0wdsratio0, data->datawdsratio0);
		write32(&(ddr_data_reg[nr] + i)->dt0wiratio0, data->datawiratio0);
		write32(&(ddr_data_reg[nr] + i)->dt0giratio0, data->datagiratio0);
		write32(&(ddr_data_reg[nr] + i)->dt0fwsratio0, data->datafwsratio0);
		write32(&(ddr_data_reg[nr] + i)->dt0wrsratio0, data->datawrsratio0);
	}
}

static void config_io_ctrl(const struct ctrl_ioregs *ioregs)
{
	if (!ioregs)
		return;

	write32(&ioctrl_reg->cm0ioctl, ioregs->cm0ioctl);
	write32(&ioctrl_reg->cm1ioctl, ioregs->cm1ioctl);
	write32(&ioctrl_reg->cm2ioctl, ioregs->cm2ioctl);
	write32(&ioctrl_reg->dt0ioctl, ioregs->dt0ioctl);
	write32(&ioctrl_reg->dt1ioctl, ioregs->dt1ioctl);
}


/**
 * Configure DDR CMD control registers
 */
static void config_cmd_ctrl(const struct cmd_control *cmd, int nr)
{
	if (!cmd)
		return;

	write32(&ddr_cmd_reg[nr]->cm0csratio, cmd->cmd0csratio);
	write32(&ddr_cmd_reg[nr]->cm0iclkout, cmd->cmd0iclkout);

	write32(&ddr_cmd_reg[nr]->cm1csratio, cmd->cmd1csratio);
	write32(&ddr_cmd_reg[nr]->cm1iclkout, cmd->cmd1iclkout);

	write32(&ddr_cmd_reg[nr]->cm2csratio, cmd->cmd2csratio);
	write32(&ddr_cmd_reg[nr]->cm2iclkout, cmd->cmd2iclkout);
}

static inline uint32_t get_emif_rev(uint32_t base)
{
	struct emif_reg_struct *emif = (struct emif_reg_struct *)base;

	return (read32(&emif->emif_mod_id_rev) & EMIF_REG_MAJOR_REVISION_MASK)
	       >> EMIF_REG_MAJOR_REVISION_SHIFT;
}

/*
 * Get SDRAM type connected to EMIF.
 * Assuming similar SDRAM parts are connected to both EMIF's
 * which is typically the case. So it is sufficient to get
 * SDRAM type from EMIF1.
 */
static inline uint32_t emif_sdram_type(uint32_t sdram_config)
{
	return (sdram_config & EMIF_REG_SDRAM_TYPE_MASK) >> EMIF_REG_SDRAM_TYPE_SHIFT;
}

/*
 * Configure EXT PHY registers for software leveling
 */
static void ext_phy_settings_swlvl(const struct emif_regs *regs, int nr)
{
	uint32_t *ext_phy_ctrl_base = 0;
	uint32_t *emif_ext_phy_ctrl_base = 0;
	uint32_t i = 0;

	ext_phy_ctrl_base = (uint32_t *)&(regs->emif_ddr_ext_phy_ctrl_1);
	emif_ext_phy_ctrl_base = (uint32_t *)&(emif_reg[nr]->emif_ddr_ext_phy_ctrl_1);

	/* Configure external phy control timing registers */
	for (i = 0; i < EMIF_EXT_PHY_CTRL_TIMING_REG; i++) {
		write32(emif_ext_phy_ctrl_base++, *ext_phy_ctrl_base);
		/* Update shadow registers */
		write32(emif_ext_phy_ctrl_base++, *ext_phy_ctrl_base++);
	}
}

/*
 * Configure EXT PHY registers for hardware leveling
 */
static void ext_phy_settings_hwlvl(const struct emif_regs *regs, int nr)
{
	/*
	 * Enable hardware leveling on the EMIF.  For details about these
	 * magic values please see the EMIF registers section of the TRM.
	 */
	if (regs->emif_ddr_phy_ctlr_1 & 0x00040000) {
		/* PHY_INVERT_CLKOUT = 1 */
		write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_1, 0x00040100);
		write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_1_shdw, 0x00040100);
	} else {
		/* PHY_INVERT_CLKOUT = 0 */
		write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_1, 0x08020080);
		write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_1_shdw, 0x08020080);
	}

	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_22, 0x00000000);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_22_shdw, 0x00000000);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_23, 0x00600020);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_23_shdw, 0x00600020);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_24, 0x40010080);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_24_shdw, 0x40010080);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_25, 0x08102040);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_25_shdw, 0x08102040);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_26, 0x00200020);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_26_shdw, 0x00200020);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_27, 0x00200020);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_27_shdw, 0x00200020);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_28, 0x00200020);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_28_shdw, 0x00200020);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_29, 0x00200020);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_29_shdw, 0x00200020);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_30, 0x00200020);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_30_shdw, 0x00200020);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_31, 0x00000000);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_31_shdw, 0x00000000);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_32, 0x00000000);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_32_shdw, 0x00000000);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_33, 0x00000000);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_33_shdw, 0x00000000);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_34, 0x00000000);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_34_shdw, 0x00000000);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_35, 0x00000000);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_35_shdw, 0x00000000);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_36, 0x00000077);
	write32(&emif_reg[nr]->emif_ddr_ext_phy_ctrl_36_shdw, 0x00000077);

	/*
	 * Sequence to ensure that the PHY is again in a known state after
	 * hardware leveling.
	 */
	write32(&emif_reg[nr]->emif_iodft_tlgc, 0x2011);
	write32(&emif_reg[nr]->emif_iodft_tlgc, 0x2411);
	write32(&emif_reg[nr]->emif_iodft_tlgc, 0x2011);
}


/**
 * Configure DDR PHY
 */
static void config_ddr_phy(const struct emif_regs *regs, int nr)
{
	/*
	 * Disable initialization and refreshes for now until we finish
	 * programming EMIF regs and set time between rising edge of
	 * DDR_RESET to rising edge of DDR_CKE to > 500us per memory spec.
	 * We currently hardcode a value based on a max expected frequency
	 * of 400MHz.
	 */
	write32(&emif_reg[nr]->emif_sdram_ref_ctrl, EMIF_REG_INITREF_DIS_MASK | 0x3100);

	write32(&emif_reg[nr]->emif_ddr_phy_ctrl_1, regs->emif_ddr_phy_ctlr_1);
	write32(&emif_reg[nr]->emif_ddr_phy_ctrl_1_shdw, regs->emif_ddr_phy_ctlr_1);

	if (get_emif_rev((uint32_t)emif_reg[nr]) == EMIF_4D5) {
		if (emif_sdram_type(regs->sdram_config) == EMIF_SDRAM_TYPE_DDR3)
			ext_phy_settings_hwlvl(regs, nr);
		else
			ext_phy_settings_swlvl(regs, nr);
	}
}

/**
 * Set SDRAM timings
 */
static void set_sdram_timings(const struct emif_regs *regs, int nr)
{
	write32(&emif_reg[nr]->emif_sdram_tim_1, regs->sdram_tim1);
	write32(&emif_reg[nr]->emif_sdram_tim_1_shdw, regs->sdram_tim1);
	write32(&emif_reg[nr]->emif_sdram_tim_2, regs->sdram_tim2);
	write32(&emif_reg[nr]->emif_sdram_tim_2_shdw, regs->sdram_tim2);
	write32(&emif_reg[nr]->emif_sdram_tim_3, regs->sdram_tim3);
	write32(&emif_reg[nr]->emif_sdram_tim_3_shdw, regs->sdram_tim3);
}

static void ddr_pll_config(uint32_t ddrpll_m)
{
	uint32_t clkmode, clksel, div_m2;

	clkmode = read32(&am335x_cm_wkup->clkmode_dpll_ddr);
	clksel = read32(&am335x_cm_wkup->clksel_dpll_ddr);
	div_m2 = read32(&am335x_cm_wkup->div_m2_dpll_ddr);

	/* Set the PLL to bypass Mode */
	clkmode = (clkmode & CLK_MODE_MASK) | PLL_BYPASS_MODE;
	write32(&am335x_cm_wkup->clkmode_dpll_ddr, clkmode);

	/* Wait till bypass mode is enabled */
	while ((read32(&am335x_cm_wkup->idlest_dpll_ddr) & ST_MN_BYPASS) != ST_MN_BYPASS)
		;

	clksel = clksel & (~CLK_SEL_MASK);
	clksel = clksel | ((ddrpll_m << CLK_SEL_SHIFT) | DDRPLL_N);
	write32(&am335x_cm_wkup->clksel_dpll_ddr, clksel);

	div_m2 = div_m2 & CLK_DIV_SEL;
	div_m2 = div_m2 | DDRPLL_M2;
	write32(&am335x_cm_wkup->div_m2_dpll_ddr, div_m2);

	clkmode = (clkmode & CLK_MODE_MASK) | CLK_MODE_SEL;
	write32(&am335x_cm_wkup->clkmode_dpll_ddr, clkmode);

	/* Wait till dpll is locked */
	while ((read32(&am335x_cm_wkup->idlest_dpll_ddr) & ST_DPLL_CLK) != ST_DPLL_CLK)
		;
}


static void enable_emif_clocks(void)
{
	/* Enable EMIF0 Clock */
	write32(&am335x_cm_per->emif, CM_MODULEMODE_ENABLED);
	/* Poll if module is functional */
	while ((read32(&am335x_cm_per->emif)) != CM_MODULEMODE_ENABLED)
		;
}

void config_ddr(uint32_t pll, const struct ctrl_ioregs *ioregs, const struct ddr_data *data,
		const struct cmd_control *ctrl, const struct emif_regs *regs, int nr)
{
	enable_emif_clocks();
	ddr_pll_config(pll);
	config_vtp(nr);
	config_cmd_ctrl(ctrl, nr);
	config_ddr_data(data, nr);
	config_io_ctrl(ioregs);

	/* Set CKE to be controlled by EMIF/DDR PHY */
	write32(&ddrctrl->ddrckectrl, DDR_CKE_CTRL_NORMAL);

	/* Program EMIF instance */
	config_ddr_phy(regs, nr);

	set_sdram_timings(regs, nr);
	config_sdram(regs, nr);
}
