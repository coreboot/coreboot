/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <delay.h>
#include <soc/emi.h>
#include <soc/dramc_register.h>
#include <soc/dramc_pi_api.h>

static void sw_imp_cal_vref_sel(u8 term_option, u8 impcal_stage)
{
	u8 vref_sel = 0;

	if (term_option == 1)
		vref_sel = IMP_LP4X_TERM_VREF_SEL;
	else {
		switch (impcal_stage) {
		case IMPCAL_STAGE_DRVP:
			vref_sel = IMP_DRVP_LP4X_UNTERM_VREF_SEL;
			break;
		case IMPCAL_STAGE_DRVN:
			vref_sel = IMP_DRVN_LP4X_UNTERM_VREF_SEL;
			break;
		default:
			vref_sel = IMP_TRACK_LP4X_UNTERM_VREF_SEL;
			break;
		}
	}

	clrsetbits_le32(&ch[0].phy.shu[0].ca_cmd[11], 0x3f << 8, vref_sel << 8);
}

void dramc_sw_impedance(const struct sdram_params *params)
{
	u8 term = 0, ca_term = ODT_OFF, dq_term = ODT_ON;
	u32 sw_impedance[2][4] = {0};

	for (term = 0; term < 2; term++)
		for (u8 i = 0; i < 4; i++)
			sw_impedance[term][i] = params->impedance[term][i];

	sw_impedance[ODT_OFF][2] = sw_impedance[ODT_ON][2];
	sw_impedance[ODT_OFF][3] = sw_impedance[ODT_ON][3];

	clrsetbits_le32(&ch[0].phy.shu[0].ca_cmd[11], 0xff, 0x3);
	sw_imp_cal_vref_sel(dq_term, IMPCAL_STAGE_DRVP);

	/* DQ */
	clrsetbits_le32(&ch[0].ao.shu[0].drving[0], (0x1f << 5) | (0x1f << 0),
		(sw_impedance[dq_term][0] << 5) |
		(sw_impedance[dq_term][1] << 0));
	clrsetbits_le32(&ch[0].ao.shu[0].drving[1],
		(0x1f << 25)|(0x1f << 20) | (1 << 31),
		(sw_impedance[dq_term][0] << 25) |
		(sw_impedance[dq_term][1] << 20) | (!dq_term << 31));
	clrsetbits_le32(&ch[0].ao.shu[0].drving[2], (0x1f << 5) | (0x1f << 0),
		(sw_impedance[dq_term][2] << 5) |
		(sw_impedance[dq_term][3] << 0));
	clrsetbits_le32(&ch[0].ao.shu[0].drving[3], (0x1f << 25) | (0x1f << 20),
		(sw_impedance[dq_term][2] << 25) |
		(sw_impedance[dq_term][3] << 20));

	/* DQS */
	for (u8 i = 0; i <= 2; i += 2) {
		clrsetbits_le32(&ch[0].ao.shu[0].drving[i],
			(0x1f << 25) | (0x1f << 20),
			(sw_impedance[dq_term][i] << 25) |
			(sw_impedance[dq_term][i + 1] << 20));
		clrsetbits_le32(&ch[0].ao.shu[0].drving[i],
			(0x1f << 15) | (0x1f << 10),
			(sw_impedance[dq_term][i] << 15) |
			(sw_impedance[dq_term][i + 1] << 10));
	}

	/* CMD & CLK */
	for (u8 i = 1; i <= 3; i += 2) {
		clrsetbits_le32(&ch[0].ao.shu[0].drving[i],
			(0x1f << 15) | (0x1f << 10),
			(sw_impedance[ca_term][i - 1] << 15) |
			(sw_impedance[ca_term][i] << 10));
		clrsetbits_le32(&ch[0].ao.shu[0].drving[i],
			(0x1f << 5) | (0x1f << 0),
			(sw_impedance[ca_term][i - 1] << 5) |
			(sw_impedance[ca_term][i] << 0));
	}

	clrsetbits_le32(&ch[0].phy.shu[0].ca_cmd[11], 0x1f << 17,
		sw_impedance[ca_term][0] << 17);
	clrsetbits_le32(&ch[0].phy.shu[0].ca_cmd[11], 0x1f << 22,
		sw_impedance[ca_term][1] << 22);

	clrsetbits_le32(&ch[0].phy.shu[0].ca_cmd[3],
		SHU1_CA_CMD3_RG_TX_ARCMD_PU_PRE_MASK,
		1 << SHU1_CA_CMD3_RG_TX_ARCMD_PU_PRE_SHIFT);
	clrbits_le32(&ch[0].phy.shu[0].ca_cmd[0],
		SHU1_CA_CMD0_RG_TX_ARCLK_DRVN_PRE_MASK);

	clrsetbits_le32(&ch[0].phy.shu[0].ca_dll[1], 0x1f << 16, 0x9 << 16);
}
