/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/pmif.h>
#include <soc/pmif_spmi.h>

/* PMIF, SPI_MODE_CTRL */
DEFINE_BIT(VLD_SRCLK_EN_CTRL, 5)
DEFINE_BIT(SPI_MODE_CTRL_PMIF_RDY, 9)
DEFINE_BIT(SPI_MODE_CTRL_SRCLK_EN, 10)
DEFINE_BIT(SPI_MODE_CTRL_SRVOL_EN, 11)

/* PMIF, SLEEP_PROTECTION_CTRL */
DEFINE_BIT(SPM_SLEEP_REQ_SEL, 0)
DEFINE_BIT(SCP_SLEEP_REQ_SEL, 9)

void pmif_spmi_force_normal_mode(struct pmif *arb)
{
	/* listen srclken_0 only for entering normal or sleep mode */
	SET32_BITFIELDS(&arb->mtk_pmif->spi_mode_ctrl,
			VLD_SRCLK_EN_CTRL, 0,
			SPI_MODE_CTRL_PMIF_RDY, 1,
			SPI_MODE_CTRL_SRCLK_EN, 0,
			SPI_MODE_CTRL_SRVOL_EN, 0);

	/* disable spm/scp sleep request */
	SET32_BITFIELDS(&arb->mtk_pmif->sleep_protection_ctrl, SPM_SLEEP_REQ_SEL, 1,
			SCP_SLEEP_REQ_SEL, 1);
	printk(BIOS_INFO, "%s done\n", __func__);
}
