/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/mtcmos.h>
#include <soc/spm.h>

struct power_domain_data {
	void *pwr_con;
	u32 pwr_sta_mask;
	u32 sram_pdn_mask;
	u32 sram_ack_mask;
};

enum {
	SRAM_ISOINT_B	= 1U << 6,
	SRAM_CKISO	= 1U << 5,
	PWR_CLK_DIS	= 1U << 4,
	PWR_ON_2ND	= 1U << 3,
	PWR_ON		= 1U << 2,
	PWR_ISO		= 1U << 1,
	PWR_RST_B	= 1U << 0
};

static void mtcmos_power_on(const struct power_domain_data *pd)
{
	write32(&mtk_spm->poweron_config_set,
		(SPM_PROJECT_CODE << 16) | (1U << 0));

	setbits32(pd->pwr_con, PWR_ON);
	setbits32(pd->pwr_con, PWR_ON_2ND);

	while (!(read32(&mtk_spm->pwr_status) & pd->pwr_sta_mask) ||
	       !(read32(&mtk_spm->pwr_status_2nd) & pd->pwr_sta_mask))
		continue;

	clrbits32(pd->pwr_con, PWR_CLK_DIS);
	clrbits32(pd->pwr_con, PWR_ISO);
	setbits32(pd->pwr_con, PWR_RST_B);
	clrbits32(pd->pwr_con, pd->sram_pdn_mask);

	while (read32(pd->pwr_con) & pd->sram_ack_mask)
		continue;
}

void mtcmos_display_power_on(void)
{
	static const struct power_domain_data disp = {
		.pwr_con = &mtk_spm->dis_pwr_con,
		.pwr_sta_mask = DISP_PWR_STA_MASK,
		.sram_pdn_mask = DISP_SRAM_PDN_MASK,
		.sram_ack_mask = DISP_SRAM_ACK_MASK,
	};

	mtcmos_power_on(&disp);
}

void mtcmos_audio_power_on(void)
{
	static const struct power_domain_data audio = {
		.pwr_con = &mtk_spm->audio_pwr_con,
		.pwr_sta_mask = AUDIO_PWR_STA_MASK,
		.sram_pdn_mask = AUDIO_SRAM_PDN_MASK,
		.sram_ack_mask = AUDIO_SRAM_ACK_MASK,
	};

	mtcmos_power_on(&audio);
}
