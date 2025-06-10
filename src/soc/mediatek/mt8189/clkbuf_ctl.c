/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <delay.h>
#include <soc/clkbuf_ctl.h>
#include <soc/mt6359p.h>
#include <soc/srclken_rc.h>

#define PMIC_REG_MASK			0xFFFF
#define PMIC_REG_SHIFT			0

#define DCXO_DBG_AUXOUT_1		54
#define DCXO_DBG_AUXOUT_2		55
#define DCXO_DBG_AUXOUT_3		56

#define PMIC_CW00_INIT_VAL		0x4F1D
#define PMIC_CW09_INIT_VAL		0x31F0
#define XO_SOC_VOTER_MASK		0x001
#define XO_CONN_VOTER_MASK		0x7C0
#define XO_NFC_VOTER_MASK		0x001
#define XO_EXT_VOTER_MASK		0x001
#define XO_CDAC_FPM_VAL			0xA8
#define XO_CDAC_LPM_VAL			0xFF

static void clk_buf_dump_clkbuf_log(void)
{
	u32 pmic_cw00, pmic_cw09, pmic_cw12, pmic_cw13,
	    pmic_cw15, pmic_cw19, top_spi_con1,
	    ldo_vrfck_op_en, ldo_vbbck_op_en, ldo_vrfck_en,
	    ldo_vbbck_en, vrfck_hv_en;

	pmic_cw00 = mt6359p_read_field(PMIC_XO_EXTBUF1_MODE_ADDR,
				       PMIC_REG_MASK, PMIC_REG_SHIFT);
	pmic_cw09 = mt6359p_read_field(PMIC_XO_EXTBUF7_MODE_ADDR,
				       PMIC_REG_MASK, PMIC_REG_SHIFT);
	pmic_cw12 = mt6359p_read_field(PMIC_XO_EXTBUF2_CLKSEL_MAN_ADDR,
				       PMIC_REG_MASK, PMIC_REG_SHIFT);
	pmic_cw13 = mt6359p_read_field(PMIC_RG_XO_EXTBUF2_SRSEL_ADDR,
				       PMIC_REG_MASK, PMIC_REG_SHIFT);
	pmic_cw15 = mt6359p_read_field(PMIC_RG_XO_RESERVED1_ADDR,
				       PMIC_REG_MASK, PMIC_REG_SHIFT);
	pmic_cw19 = mt6359p_read_field(PMIC_RG_XO_EXTBUF2_RSEL_ADDR,
				       PMIC_REG_MASK, PMIC_REG_SHIFT);
	top_spi_con1 = mt6359p_read_field(PMIC_RG_SRCLKEN_IN3_EN_ADDR,
					  PMIC_RG_SRCLKEN_IN3_EN_MASK,
					  PMIC_RG_SRCLKEN_IN3_EN_SHIFT);
	ldo_vrfck_op_en = mt6359p_read_field(PMIC_RG_LDO_VRFCK_HW14_OP_EN_ADDR,
					     PMIC_RG_LDO_VRFCK_HW14_OP_EN_MASK,
					     PMIC_RG_LDO_VRFCK_HW14_OP_EN_SHIFT);
	ldo_vbbck_op_en = mt6359p_read_field(PMIC_RG_LDO_VBBCK_HW14_OP_EN_ADDR,
					     PMIC_RG_LDO_VBBCK_HW14_OP_EN_MASK,
					     PMIC_RG_LDO_VBBCK_HW14_OP_EN_SHIFT);
	ldo_vrfck_en = mt6359p_read_field(PMIC_RG_LDO_VRFCK_EN_ADDR,
					  PMIC_RG_LDO_VRFCK_EN_MASK,
					  PMIC_RG_LDO_VRFCK_EN_SHIFT);
	ldo_vbbck_en = mt6359p_read_field(PMIC_RG_LDO_VBBCK_EN_ADDR,
					  PMIC_RG_LDO_VBBCK_EN_MASK,
					  PMIC_RG_LDO_VBBCK_EN_SHIFT);
	printk(BIOS_INFO, "%s DCXO_CW00/09/12/13/15/19=0x%x %x %x %x %x %x\n",
	       __func__, pmic_cw00, pmic_cw09, pmic_cw12, pmic_cw13, pmic_cw15, pmic_cw19);
	printk(BIOS_INFO, "%s spi_con1/ldo_rf_op/ldo_bb_op/ldo_rf_en/"
	       "ldo_bb_en=0x%x %x %x %x %x\n", __func__, top_spi_con1,
	       ldo_vrfck_op_en, ldo_vbbck_op_en, ldo_vrfck_en, ldo_vbbck_en);
	vrfck_hv_en = mt6359p_read_field(PMIC_RG_VRFCK_HV_EN_ADDR,
					 PMIC_RG_VRFCK_HV_EN_MASK,
					 PMIC_RG_VRFCK_HV_EN_SHIFT);
	printk(BIOS_INFO, "%s clk buf vrfck_hv_en=0x%x\n", __func__, vrfck_hv_en);
}

u32 clk_buf_get_pmrc_en(void)
{
	return mt6359p_read_field(PMIC_PMRC_EN_ADDR, PMIC_PMRC_EN_MASK, PMIC_PMRC_EN_SHIFT);
}

static void clk_buf_init_pmic_clkbuf(void)
{
	int pmic_cw04_l, pmic_cw04_h;

	/* Dump registers before setting */
	clk_buf_dump_clkbuf_log();

	/* unlock pmic key */
	mt6359p_write_field(PMIC_TMA_KEY_ADDR, 0x9CA6,
			    PMIC_TMA_KEY_MASK, PMIC_TMA_KEY_SHIFT);

	/* 1.1 set VRFCK input supply */
	mt6359p_write_field(PMIC_RG_VRFCK_HV_EN_ADDR, 0x0,
			    PMIC_RG_VRFCK_HV_EN_MASK, PMIC_RG_VRFCK_HV_EN_SHIFT);
	/* 1.2.1 set VRFCK En = 0 */
	mt6359p_write_field(PMIC_RG_LDO_VRFCK_EN_ADDR, 0x0,
			    PMIC_RG_LDO_VRFCK_EN_MASK, PMIC_RG_LDO_VRFCK_EN_SHIFT);
	/* 1.2.1 set VRFCK1 as power src */
	mt6359p_write_field(PMIC_RG_LDO_VRFCK_ANA_SEL_ADDR, 0x1,
			    PMIC_RG_LDO_VRFCK_ANA_SEL_MASK, PMIC_RG_LDO_VRFCK_ANA_SEL_SHIFT);
	/* 1.2.2 switch LDO-RFCK to LDO-RFCK1 */
	mt6359p_write_field(PMIC_RG_VRFCK_NDIS_EN_ADDR, 0x0,
			    PMIC_RG_VRFCK_NDIS_EN_MASK, PMIC_RG_VRFCK_NDIS_EN_SHIFT);
	mt6359p_write_field(PMIC_RG_VRFCK_1_NDIS_EN_ADDR, 0x1,
			    PMIC_RG_VRFCK_1_NDIS_EN_MASK, PMIC_RG_VRFCK_1_NDIS_EN_SHIFT);
	/* 1.2.3 Set VRFCK En = 1 */
	mt6359p_write_field(PMIC_RG_LDO_VRFCK_EN_ADDR, 0x1,
			    PMIC_RG_LDO_VRFCK_EN_MASK, PMIC_RG_LDO_VRFCK_EN_SHIFT);

	/* 1.3 turn on LDO HW control from EXTBUFs */
	mt6359p_write_field(PMIC_RG_LDO_VRFCK_OP_EN_SET_ADDR, 0x1,
			    PMIC_RG_LDO_VRFCK_HW14_OP_EN_MASK,
			    PMIC_RG_LDO_VRFCK_HW14_OP_EN_SHIFT);
	mt6359p_write_field(PMIC_RG_LDO_VBBCK_OP_EN_SET_ADDR, 0x1,
			    PMIC_RG_LDO_VBBCK_HW14_OP_EN_MASK,
			    PMIC_RG_LDO_VBBCK_HW14_OP_EN_SHIFT);

	/* 1.4 turn on LDO SW control */
	mt6359p_write_field(PMIC_RG_LDO_VRFCK_EN_ADDR, 0x0,
			    PMIC_RG_LDO_VRFCK_EN_MASK, PMIC_RG_LDO_VRFCK_EN_SHIFT);
	mt6359p_write_field(PMIC_RG_LDO_VBBCK_EN_ADDR, 0x0,
			    PMIC_RG_LDO_VBBCK_EN_MASK, PMIC_RG_LDO_VBBCK_EN_SHIFT);

	/* lock pmic key */
	mt6359p_write_field(PMIC_TMA_KEY_ADDR, 0,
			    PMIC_TMA_KEY_MASK, PMIC_TMA_KEY_SHIFT);

	/* Setup initial PMIC clock buffer setting */
	/* 1.5 Buffer de-sense setting */
	mt6359p_write_field(PMIC_RG_XO_EXTBUF2_SRSEL_ADDR, PMIC_CLK_BUF2_CONTROLS_FOR_DESENSE,
			    PMIC_RG_XO_EXTBUF2_SRSEL_MASK, PMIC_RG_XO_EXTBUF2_SRSEL_SHIFT);
	mt6359p_write_field(PMIC_RG_XO_EXTBUF3_HD_ADDR, PMIC_CLK_BUF3_CONTROLS_FOR_DESENSE,
			    PMIC_RG_XO_EXTBUF3_HD_MASK, PMIC_RG_XO_EXTBUF3_HD_SHIFT);
	mt6359p_write_field(PMIC_RG_XO_EXTBUF4_SRSEL_ADDR, PMIC_CLK_BUF4_CONTROLS_FOR_DESENSE,
			    PMIC_RG_XO_EXTBUF4_SRSEL_MASK, PMIC_RG_XO_EXTBUF4_SRSEL_SHIFT);

	/* 1.6 Buffer setting for trace impedance */
	mt6359p_write_field(PMIC_RG_XO_EXTBUF1_RSEL_ADDR, PMIC_CLK_BUF1_OUTPUT_IMPEDANCE,
			    PMIC_RG_XO_EXTBUF1_RSEL_MASK, PMIC_RG_XO_EXTBUF1_RSEL_SHIFT);
	mt6359p_write_field(PMIC_RG_XO_EXTBUF2_RSEL_ADDR, PMIC_CLK_BUF2_OUTPUT_IMPEDANCE,
			    PMIC_RG_XO_EXTBUF2_RSEL_MASK, PMIC_RG_XO_EXTBUF2_RSEL_SHIFT);
	mt6359p_write_field(PMIC_RG_XO_EXTBUF3_RSEL_ADDR, PMIC_CLK_BUF3_OUTPUT_IMPEDANCE,
			    PMIC_RG_XO_EXTBUF3_RSEL_MASK, PMIC_RG_XO_EXTBUF3_RSEL_SHIFT);
	mt6359p_write_field(PMIC_RG_XO_EXTBUF4_RSEL_ADDR, PMIC_CLK_BUF4_OUTPUT_IMPEDANCE,
			    PMIC_RG_XO_EXTBUF4_RSEL_MASK, PMIC_RG_XO_EXTBUF4_RSEL_SHIFT);
	mt6359p_write_field(PMIC_RG_XO_EXTBUF7_RSEL_ADDR, PMIC_CLK_BUF7_OUTPUT_IMPEDANCE,
			    PMIC_RG_XO_EXTBUF7_RSEL_MASK, PMIC_RG_XO_EXTBUF7_RSEL_SHIFT);

	/* 1.7 26M enable control */
	/* fully new co-clock mode */
	/* All XO mode should set to 2'b01 */
	mt6359p_write_field(PMIC_XO_EXTBUF1_MODE_ADDR, PMIC_CW00_INIT_VAL,
			    PMIC_REG_MASK, PMIC_REG_SHIFT);
	mt6359p_write_field(PMIC_XO_EXTBUF7_MODE_ADDR, PMIC_CW09_INIT_VAL,
			    PMIC_REG_MASK, PMIC_REG_SHIFT);
	/* update control mapping table */
	mt6359p_write_field(PMIC_XO_SOC_VOTE_ADDR, XO_SOC_VOTER_MASK,
			    PMIC_XO_SOC_VOTE_MASK, PMIC_XO_SOC_VOTE_SHIFT);
	mt6359p_write_field(PMIC_XO_WCN_VOTE_ADDR, XO_CONN_VOTER_MASK,
			    PMIC_XO_WCN_VOTE_MASK, PMIC_XO_WCN_VOTE_SHIFT);
	mt6359p_write_field(PMIC_XO_NFC_VOTE_ADDR, XO_NFC_VOTER_MASK,
			    PMIC_XO_NFC_VOTE_MASK, PMIC_XO_NFC_VOTE_SHIFT);
	mt6359p_write_field(PMIC_XO_CEL_VOTE_ADDR, XO_CONN_VOTER_MASK,
			    PMIC_XO_CEL_VOTE_MASK, PMIC_XO_CEL_VOTE_SHIFT);
	mt6359p_write_field(PMIC_XO_EXT_VOTE_ADDR, XO_EXT_VOTER_MASK,
			    PMIC_XO_EXT_VOTE_MASK, PMIC_XO_EXT_VOTE_SHIFT);
	mdelay(1);

	/* switch to new control mode */
	/*
	 * XO_PMIC_TOP_DIG_SW is set to 0
	 * XO_MODE_CONN_BT_MASK is set to 0 (BT only : 1)
	 * XO_BUF_CONN_BT_MASK is set to 0 (BT only : 1 )
	 */
	mt6359p_write_field(PMIC_XO_PMIC_TOP_DIG_SW_ADDR, 0x0,
			    PMIC_XO_PMIC_TOP_DIG_SW_MASK, PMIC_XO_PMIC_TOP_DIG_SW_SHIFT);
	mt6359p_write_field(PMIC_XO_MODE_CONN_BT_MASK_ADDR, 0x0,
			    PMIC_XO_MODE_CONN_BT_MASK_MASK, PMIC_XO_MODE_CONN_BT_MASK_SHIFT);
	mt6359p_write_field(PMIC_XO_BUF_CONN_BT_MASK_ADDR, 0x0,
			    PMIC_XO_BUF_CONN_BT_MASK_MASK, PMIC_XO_BUF_CONN_BT_MASK_SHIFT);

	/* set capid */
	mt6359p_write_field(PMIC_XO_CDAC_FPM_ADDR, XO_CDAC_FPM_VAL,
			    PMIC_XO_CDAC_FPM_MASK, PMIC_XO_CDAC_FPM_SHIFT);
	mt6359p_write_field(PMIC_XO_CDAC_LPM_ADDR, XO_CDAC_LPM_VAL,
			    PMIC_XO_CDAC_LPM_MASK, PMIC_XO_CDAC_LPM_SHIFT);

	pmic_cw04_l = mt6359p_read_field(PMIC_XO_CDAC_FPM_ADDR,
				       PMIC_XO_CDAC_FPM_MASK, PMIC_XO_CDAC_FPM_SHIFT);
	pmic_cw04_h = mt6359p_read_field(PMIC_XO_CDAC_LPM_ADDR,
				       PMIC_XO_CDAC_LPM_MASK, PMIC_XO_CDAC_LPM_SHIFT);
	printk(BIOS_INFO,
	       "%s clk buf pmic_cw04_l=0x%x, pmic_cw04_h=0x%x\n",
	       __func__, pmic_cw04_l, pmic_cw04_h);

	/* Check if the setting is ok */
	clk_buf_dump_clkbuf_log();
}

/* For CONN DBG */
static void dcxo_dump_auxout(u16 sel)
{
	u32 rg_auxout;

	mt6359p_write_field(PMIC_XO_STATIC_AUXOUT_SEL_ADDR, sel,
			    PMIC_XO_STATIC_AUXOUT_SEL_MASK,
			    PMIC_XO_STATIC_AUXOUT_SEL_SHIFT);
	rg_auxout = mt6359p_read_field(PMIC_XO_STATIC_AUXOUT_ADDR,
				       PMIC_XO_STATIC_AUXOUT_MASK,
				       PMIC_XO_STATIC_AUXOUT_SHIFT);
	printk(BIOS_INFO, "%s: sel=%d, rg_auxout=0x%x\n", __func__, sel, rg_auxout);
}

int clk_buf_init(void)
{
	/* dump lpsd dbg1-3 for debug */
	dcxo_dump_auxout(DCXO_DBG_AUXOUT_1);
	dcxo_dump_auxout(DCXO_DBG_AUXOUT_2);
	dcxo_dump_auxout(DCXO_DBG_AUXOUT_3);

	clk_buf_init_pmic_clkbuf();

	return 0;
}
