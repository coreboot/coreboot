/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <soc/addressmap.h>
#include <soc/clkbuf_ctl.h>
#include <soc/mt6685.h>
#include <timer.h>

#define XO_RC_MODE			0x1
#define XO_DISABLE_MODE			0x0

#define XO_EN_MODE_MASK			0x7
#define XO_EN_MODE_SHIFT		0

/* Impedance mask/shift, may need update for PMIC which is not 6685 */
#define XO_IMPEDANCE_MASK		0x7
#define XO_IMPEDANCE_SHIFT		3

#define XO_DRIVING_MASK			0x3
#define XO_DRIVING_SHIFT		6

#define DCXO_DBG_AUXOUT_1		124
#define DCXO_DBG_AUXOUT_2		125
#define DCXO_DBG_AUXOUT_3		126
#define DCXO_DBG_AUXOUT_4		127

/* Voting table */
#define SOC_VOTER_L_MASK		0x01
#define SOC_VOTER_H_MASK		0x00

#define CONN_PCIE_VOTER_L_MASK		0x80
#define CONN_PCIE_VOTER_H_MASK		0x02

#define CONN_GPS_VOTER_L_MASK		0x40
#define CONN_GPS_VOTER_H_MASK		0x00

#define MMWAVE_VOTER_L_MASK		0x00
#define MMWAVE_VOTER_H_MASK		0x00

#define NFC_VOTER_L_MASK		0x00
#define NFC_VOTER_H_MASK		0x08

#define RFMD_2A_VOTER_L_MASK		0x10
#define RFMD_2A_VOTER_H_MASK		0x00

#define UFS_VOTER_L_MASK		0x00
#define UFS_VOTER_H_MASK		0x20

static const struct ox_cfg ox[] = {
	[XO_BBCK1] = {
		.reg = XO_BBCK1_MODE_ADDR,
		.mode = BBCK_BUF1_STATUS_PMIC,
		.impedance = BBCK_BUF1_OUTPUT_IMPEDANCE,
		.driving = BBCK_BUF1_DRIVING_STRENGTH,
		.voter_mask = { /* 0x0001 */
			.low_byte = SOC_VOTER_L_MASK,
			.high_byte = SOC_VOTER_H_MASK,
		},
	},
	[XO_BBCK2] = {
		.reg = XO_BBCK2_MODE_ADDR,
		.mode = BBCK_BUF2_STATUS_PMIC,
		.impedance = BBCK_BUF2_OUTPUT_IMPEDANCE,
		.driving = BBCK_BUF2_DRIVING_STRENGTH,
		.voter_mask = { /* 0x0280 */
			.low_byte = CONN_PCIE_VOTER_L_MASK,
			.high_byte = CONN_PCIE_VOTER_H_MASK,
		},
	},
	[XO_BBCK3] = {
		.reg = XO_BBCK3_MODE_ADDR,
		.mode = BBCK_BUF3_STATUS_PMIC,
		.impedance = BBCK_BUF3_OUTPUT_IMPEDANCE,
		.driving = BBCK_BUF3_DRIVING_STRENGTH,
		.voter_mask = { /* 0x0000 */
			.low_byte = 0x0,
			.high_byte = 0x0,
		},
	},
	[XO_BBCK4] = {
		.reg = XO_BBCK4_MODE_ADDR,
		.mode = BBCK_BUF4_STATUS_PMIC,
		.impedance = BBCK_BUF4_OUTPUT_IMPEDANCE,
		.driving = BBCK_BUF4_DRIVING_STRENGTH,
		.voter_mask = { /* 0x0800 */
			.low_byte = NFC_VOTER_L_MASK,
			.high_byte = NFC_VOTER_H_MASK,
		},
	},
	[XO_BBCK5] = {
		.reg = XO_BBCK5_MODE_ADDR,
		.mode = BBCK_BUF5_STATUS_PMIC,
		.impedance = BBCK_BUF5_OUTPUT_IMPEDANCE,
		.driving = BBCK_BUF5_DRIVING_STRENGTH,
		.voter_mask = { /* 0x0000 */
			.low_byte = 0x0,
			.high_byte = 0x0,
		},
	},
	[XO_RFCK1A] = {
		.reg = XO_RFCK1A_MODE_ADDR,
		.mode = RFCK_BUF1A_STATUS_PMIC,
		.impedance = RFCK_BUF1A_OUTPUT_IMPEDANCE,
		.driving = 0,
		.voter_mask = { /* 0x0000 */
			.low_byte = MMWAVE_VOTER_L_MASK,
			.high_byte = MMWAVE_VOTER_H_MASK,
		},
	},
	[XO_RFCK1B] = {
		.reg = XO_RFCK1B_MODE_ADDR,
		.mode = RFCK_BUF1B_STATUS_PMIC,
		.impedance = RFCK_BUF1B_OUTPUT_IMPEDANCE,
		.driving = 0,
		.voter_mask = { /* 0x2000 */
			.low_byte = UFS_VOTER_L_MASK,
			.high_byte = UFS_VOTER_H_MASK,
		},
	},
	[XO_RFCK1C] = {
		.reg = XO_RFCK1C_MODE_ADDR,
		.mode = CLOCK_BUFFER_DISABLE,
	},
	[XO_RFCK2A] = {
		.reg = XO_RFCK2A_MODE_ADDR,
		.mode = RFCK_BUF2A_STATUS_PMIC,
		.impedance = RFCK_BUF2A_OUTPUT_IMPEDANCE,
		.driving = 0,
		.voter_mask = { /* 0x0010 */
			.low_byte = RFMD_2A_VOTER_L_MASK,
			.high_byte = RFMD_2A_VOTER_H_MASK,
		},
	},
	[XO_RFCK2B] = {
		.reg = XO_RFCK2B_MODE_ADDR,
		.mode = RFCK_BUF2B_STATUS_PMIC,
		.impedance = RFCK_BUF2B_OUTPUT_IMPEDANCE,
		.driving = 0,
		.voter_mask = { /* 0x0040 */
			.low_byte = CONN_GPS_VOTER_L_MASK,
			.high_byte = CONN_GPS_VOTER_H_MASK,
		},
	},
	[XO_RFCK2C] = {
		.reg = XO_RFCK2C_MODE_ADDR,
		.mode = CLOCK_BUFFER_DISABLE,
	},
	[XO_CONCK1] = {
		.reg = XO_CONCK1_MODE_ADDR,
		.mode = CLOCK_BUFFER_DISABLE,
	},
	[XO_CONCK2] = {
		.reg = XO_CONCK2_MODE_ADDR,
		.mode = CLOCK_BUFFER_DISABLE,
	},
};

_Static_assert(ARRAY_SIZE(ox) == XO_NUMBER, "Wrong array size of ox");

static u16 mt6685_read_low_high(u32 low_reg, u32 high_reg)
{
	u16 data = mt6685_read8(low_reg);
	data |= mt6685_read8(high_reg) << 8;
	return data;
}

u16 clk_buf_get_pmrc_en0(void)
{
	return mt6685_read_low_high(MT6685_PMRC_CON0, MT6685_PMRC_CON1);
}

static void clk_buf_dump_dws_log(void)
{
	int i;

	printk(BIOS_INFO, "%s: DWS_CLKBUF_STA BBCK1/2/3/4/5=", __func__);
	for (i = XO_BBCK1; i <= XO_BBCK5; ++i)
		printk(BIOS_INFO, "%u/", ox[i].mode);

	printk(BIOS_INFO, "\nRFCK1A/1B/1C/2A/2B/2C=");
	for (i = XO_RFCK1A; i <= XO_RFCK2C; ++i)
		printk(BIOS_INFO, "%u/", ox[i].mode);

	printk(BIOS_INFO, "\nCONCK1/2=");
	for (i = XO_CONCK1; i <= XO_CONCK2; ++i)
		printk(BIOS_INFO, "%u/", ox[i].mode);

	printk(BIOS_INFO, "\n");

	printk(BIOS_INFO, "%s: DWS_CLK_BUF_IMPEDANCE BBCK1/2/3/4/5=", __func__);
	for (i = XO_BBCK1; i <= XO_BBCK5; ++i)
		printk(BIOS_INFO, "%u/", ox[i].impedance);

	printk(BIOS_INFO, "\nRFCK1A/1B/1C/2A/2B/2C=");
	for (i = XO_RFCK1A; i <= XO_RFCK2C; ++i)
		printk(BIOS_INFO, "%u/", ox[i].impedance);

	printk(BIOS_INFO, "\nCONCK1/2=");
	for (i = XO_CONCK1; i <= XO_CONCK2; ++i)
		printk(BIOS_INFO, "%u/", ox[i].impedance);

	printk(BIOS_INFO, "\n");

	printk(BIOS_INFO, "%s: DWS_CLK_BUF_DRIVING BBCK1/2/3/4/5=", __func__);
	for (i = XO_BBCK1; i <= XO_BBCK5; ++i)
		printk(BIOS_INFO, "%u/", ox[i].driving);

	printk(BIOS_INFO, "\n");
}

static void dump_clkbuf_log(void)
{
	u8 xo_buf_cw[XO_NUMBER];
	u16 xo_buf_vote[XO_NUMBER];
	u8 dig_manctrl_cw1;
	u8 bblpm_cw0;
	u32 dig26m_div2;
	int i;

	_Static_assert(MT6685_XO_BUF_CTL0_L + 2 * (XO_NUMBER - 1) == MT6685_XO_BUF_CTL12_L,
		       "Wrong reg for MT6685_XO_BUF_CTL12_L");
	_Static_assert(MT6685_XO_BUF_CTL0_H + 2 * (XO_NUMBER - 1) == MT6685_XO_BUF_CTL12_H,
		       "Wrong reg for MT6685_XO_BUF_CTL12_H");

	for (i = 0; i < XO_NUMBER; i++) {
		xo_buf_cw[i] = mt6685_read8(MT6685_DCXO_EXTBUF1_CW0 + i);
		xo_buf_vote[i] = mt6685_read_low_high(MT6685_XO_BUF_CTL0_L + (2 * i),
						      MT6685_XO_BUF_CTL0_H + (2 * i));
	}

	dig_manctrl_cw1 = mt6685_read8(MT6685_DCXO_DIG_MANCTRL_CW1);
	bblpm_cw0 = mt6685_read8(MT6685_DCXO_BBLPM_CW0);

	dig26m_div2 = mt6685_read_field(RG_XO_DIG26M_DIV2_ADDR,
					RG_XO_DIG26M_DIV2_MASK,
					RG_XO_DIG26M_DIV2_SHIFT);

	printk(BIOS_INFO, "[clkbuf] xo_buf_cw(%#X ~ %#X):\n",
	       MT6685_DCXO_EXTBUF1_CW0, MT6685_DCXO_EXTBUF13_CW0);
	for (i = 0; i < XO_NUMBER; ++i)
		printk(BIOS_INFO, "%#x/", xo_buf_cw[i]);
	printk(BIOS_INFO, "\n");

	printk(BIOS_INFO, "[clkbuf] xo_buf_vote(%#X ~ %#X):\n",
	       MT6685_XO_BUF_CTL0_L, MT6685_XO_BUF_CTL12_H);

	for (i = 0; i < XO_NUMBER; ++i)
		printk(BIOS_INFO, "%#x/", xo_buf_vote[i]);
	printk(BIOS_INFO, "\n");

	printk(BIOS_INFO, "[clkbuf] dig_manctrl_cw1(%#x): %#x\nbblpm_cw0(%#x): %#x\n",
	       MT6685_DCXO_DIG_MANCTRL_CW1,
	       dig_manctrl_cw1,
	       MT6685_DCXO_BBLPM_CW0,
	       bblpm_cw0);
	printk(BIOS_INFO, "[clkbuf] dig26m_div2(%#x): %#x\n",
	       RG_XO_DIG26M_DIV2_ADDR,
	       dig26m_div2);
}

static void clk_buf_init_pmic_clkbuf(void)
{
	u8 val;
	int i;

	/* 1.1 Write XO_BUF impedance & driving */
	for (i = 0; i < XO_NUMBER; ++i)
		if (ox[i].mode != CLOCK_BUFFER_DISABLE)
			mt6685_write_field(ox[i].reg, ox[i].impedance,
					   XO_IMPEDANCE_MASK, XO_IMPEDANCE_SHIFT);

	/* 1.2 Write XO_BUF driving */
	for (i = 0; i <= XO_BBCK5; ++i)
		if (ox[i].mode != CLOCK_BUFFER_DISABLE)
			mt6685_write_field(ox[i].reg, ox[i].driving, XO_DRIVING_MASK,
					   XO_DRIVING_SHIFT);

	/* 1.3 Write RC voting table */
	for (i = 0; i < XO_NUMBER; i++) {
		u8 low, high;
		if (ox[i].mode == CLOCK_BUFFER_DISABLE) {
			low = 0;
			high = 0;
		} else {
			low = ox[i].voter_mask.low_byte;
			high = ox[i].voter_mask.high_byte;
		}
		mt6685_write8(MT6685_XO_BUF_CTL0_L + (2 * i), low);
		mt6685_write8(MT6685_XO_BUF_CTL0_H + (2 * i), high);
	}

	/* 1.4 Turn on LDO HW control from XO_BUFs */
	mt6685_write_field(RG_LDO_VRFCK1_HW14_OP_EN_ADDR,
			   0x1,
			   RG_LDO_VRFCK1_HW14_OP_EN_MASK,
			   RG_LDO_VRFCK1_HW14_OP_EN_SHIFT);

	mt6685_write_field(RG_LDO_VRFCK2_HW14_OP_EN_ADDR,
			   0x1,
			   RG_LDO_VRFCK2_HW14_OP_EN_MASK,
			   RG_LDO_VRFCK2_HW14_OP_EN_SHIFT);

	mt6685_write_field(RG_LDO_VBBCK_HW14_OP_EN_ADDR,
			   0x1,
			   RG_LDO_VBBCK_HW14_OP_EN_MASK,
			   RG_LDO_VBBCK_HW14_OP_EN_SHIFT);

	/* 1.5 Turn off LDO SW control */
	mt6685_write_field(RG_LDO_VRFCK1_EN_ADDR,
			   0x0,
			   RG_LDO_VRFCK1_EN_MASK,
			   RG_LDO_VRFCK1_EN_SHIFT);

	mt6685_write_field(RG_LDO_VRFCK2_EN_ADDR,
			   0x0,
			   RG_LDO_VRFCK2_EN_MASK,
			   RG_LDO_VRFCK2_EN_SHIFT);

	mt6685_write_field(RG_LDO_VBBCK_EN_ADDR,
			   0x0,
			   RG_LDO_VBBCK_EN_MASK,
			   RG_LDO_VBBCK_EN_SHIFT);

	/* 1.6 Enable/Disable XO by dws */
	for (i = 0; i < XO_NUMBER; ++i) {
		if (ox[i].mode == CLOCK_BUFFER_DISABLE ||
		    ox[i].mode == CLOCK_BUFFER_SW_CONTROL)
			mt6685_write_field(ox[i].reg, XO_DISABLE_MODE,
					   XO_EN_MODE_MASK, XO_EN_MODE_SHIFT);
		else if (ox[i].mode == CLOCK_BUFFER_HW_CONTROL)
			mt6685_write_field(ox[i].reg, XO_RC_MODE,
					   XO_EN_MODE_MASK, XO_EN_MODE_SHIFT);
	}

	/* 1.7 RC mode setting, en_bb hw mode. And switch to srclken_rc mode */
	val = mt6685_read8(MT6685_DCXO_DIG_MANCTRL_CW1);
	val &= ~(XO_PMIC_TOP_DIG_SW_MASK << XO_PMIC_TOP_DIG_SW_SHIFT);
	val &= ~(XO_ENBB_MAN_MASK << XO_ENBB_MAN_SHIFT);
	val &= ~(XO_ENBB_EN_M_MASK << XO_ENBB_EN_M_SHIFT);
	val &= ~(XO_CLKSEL_EN_M_MASK << XO_CLKSEL_EN_M_SHIFT);
	mt6685_write8(MT6685_DCXO_DIG_MANCTRL_CW1, val);
}

static u16 dcxo_dbg_read_auxout(u32 sel)
{
	u16 val;

	mt6685_write_field(XO_STATIC_AUXOUT_SEL_ADDR, sel, XO_STATIC_AUXOUT_SEL_MASK,
			   XO_STATIC_AUXOUT_SEL_SHIFT);
	val = mt6685_read_low_high(XO_STATIC_AUXOUT_L_ADDR, XO_STATIC_AUXOUT_H_ADDR);

	printk(BIOS_INFO, "%s: sel=%d, rg_auxout=%#x\n", __func__, sel, val);

	return val;
}

int clk_buf_init(void)
{
	/* Check dws setting */
	clk_buf_dump_dws_log();

	/* Dump lpsd dbg0-3 for debug during pmic_long_press_reset */
	dcxo_dbg_read_auxout(DCXO_DBG_AUXOUT_1);
	dcxo_dbg_read_auxout(DCXO_DBG_AUXOUT_2);
	dcxo_dbg_read_auxout(DCXO_DBG_AUXOUT_3);
	dcxo_dbg_read_auxout(DCXO_DBG_AUXOUT_4);

	clk_buf_init_pmic_clkbuf();

	/* Check if the setting is ok */
	dump_clkbuf_log();

	return 0;
}
