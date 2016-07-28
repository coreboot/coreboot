/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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
#include <soc/mtcmos.h>
#include <soc/spm.h>

enum {
	SRAM_ISOINT_B	= 1U << 6,
	SRAM_CKISO	= 1U << 5,
	PWR_CLK_DIS	= 1U << 4,
	PWR_ON_2ND	= 1U << 3,
	PWR_ON		= 1U << 2,
	PWR_ISO		= 1U << 1,
	PWR_RST_B	= 1U << 0
};

enum {
	SRAM_PDN           = 0xf << 8,
	DIS_SRAM_ACK       = 0x1 << 12,
	AUD_SRAM_ACK       = 0xf << 12,
};

enum {
	DIS_PWR_STA_MASK   = 0x1 << 3,
	AUD_PWR_STA_MASK   = 0x1 << 24,
};

static void mtcmos_power_on(u32 *pwr_con, u32 pwr_sta_mask)
{
	write32(&mt8173_spm->poweron_config_set,
		(SPM_PROJECT_CODE << 16) | (1U << 0));

	setbits_le32(pwr_con, PWR_ON);
	setbits_le32(pwr_con, PWR_ON_2ND);

	while (!(read32(&mt8173_spm->pwr_status) & pwr_sta_mask) ||
	       !(read32(&mt8173_spm->pwr_status_2nd) & pwr_sta_mask))
		continue;

	clrbits_le32(pwr_con, PWR_CLK_DIS);
	clrbits_le32(pwr_con, PWR_ISO);
	setbits_le32(pwr_con, PWR_RST_B);
	clrbits_le32(pwr_con, SRAM_PDN);
}

void mtcmos_audio_power_on(void)
{
	mtcmos_power_on(&mt8173_spm->audio_pwr_con, AUD_PWR_STA_MASK);
	while (read32(&mt8173_spm->audio_pwr_con) & AUD_SRAM_ACK)
		continue;
}

void mtcmos_display_power_on(void)
{
	mtcmos_power_on(&mt8173_spm->dis_pwr_con, DIS_PWR_STA_MASK);
	while (read32(&mt8173_spm->dis_pwr_con) & DIS_SRAM_ACK)
		continue;
}
