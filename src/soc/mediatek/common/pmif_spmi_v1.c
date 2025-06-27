/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <soc/pmif.h>
#include <soc/pmif_spmi.h>

/* PMIF, SPI_MODE_CTRL */
DEFINE_BIT(SPI_MODE_CTRL, 7)
DEFINE_BIT(SRVOL_EN, 11)
DEFINE_BIT(SPI_MODE_EXT_CMD, 12)
DEFINE_BIT(SPI_EINT_MODE_GATING_EN, 13)

/* PMIF, SLEEP_PROTECTION_CTRL */
DEFINE_BITFIELD(SPM_SLEEP_REQ_SEL, 1, 0)
DEFINE_BITFIELD(SCP_SLEEP_REQ_SEL, 10, 9)

void pmif_spmi_force_normal_mode(struct pmif *arb)
{
	/* listen srclken_0 only for entering normal or sleep mode */
	SET32_BITFIELDS(&arb->mtk_pmif->spi_mode_ctrl,
			SPI_MODE_CTRL, 0,
			SRVOL_EN, 0,
			SPI_MODE_EXT_CMD, 1,
			SPI_EINT_MODE_GATING_EN, 1);

	/* enable spm/scp sleep request */
	SET32_BITFIELDS(&arb->mtk_pmif->sleep_protection_ctrl, SPM_SLEEP_REQ_SEL, 0,
			SCP_SLEEP_REQ_SEL, 0);
}
