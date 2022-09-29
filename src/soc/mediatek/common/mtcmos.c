/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <device/mmio.h>
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

__weak void mtcmos_set_scpd_ext_buck_iso(const struct power_domain_data *pd)
{
	/* do nothing */
}

void mtcmos_power_on(const struct power_domain_data *pd)
{
	write32(&mtk_spm->poweron_config_set,
		(SPM_PROJECT_CODE << 16) | (1U << 0));

	if (pd->caps & SCPD_EXT_BUCK_ISO)
		mtcmos_set_scpd_ext_buck_iso(pd);

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

	if (pd->caps & SCPD_SRAM_ISO) {
		setbits32(pd->pwr_con, SRAM_ISOINT_B);
		udelay(1);
		clrbits32(pd->pwr_con, SRAM_CKISO);
	}
}

void mtcmos_display_power_on(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(disp); i++)
		mtcmos_power_on(&disp[i]);
}

void mtcmos_audio_power_on(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(audio); i++)
		mtcmos_power_on(&audio[i]);
}
