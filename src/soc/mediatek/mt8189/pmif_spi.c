/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/pmif_spi.h>

/* PMIF, SPI_MODE_CTRL */
DEFINE_BIT(SPI_MODE_CTRL_PMIF_RDY, 9)
DEFINE_BIT(SPI_MODE_CTRL_SRCLK_EN, 10)
DEFINE_BIT(SPI_MODE_CTRL_SRVOL_EN, 11)

/* PMIF, SLEEP_PROTECTION_CTRL */
DEFINE_BITFIELD(SPM_SLEEP_REQ_SEL, 1, 0)
DEFINE_BITFIELD(SCP_SLEEP_REQ_SEL, 10, 9)

void pmif_spi_iocfg(void)
{
	/* Set SoC SPI IO driving strength to 6 mA */
	gpio_set_driving(GPIO(PWRAP_SPI_CK), GPIO_DRV_6_MA);
	gpio_set_driving(GPIO(PWRAP_SPI_CSN), GPIO_DRV_6_MA);
	gpio_set_driving(GPIO(PWRAP_SPI_MI), GPIO_DRV_6_MA);
	gpio_set_driving(GPIO(PWRAP_SPI_MO), GPIO_DRV_6_MA);
}

void pmif_spi_setting(struct pmif *arb)
{
	SET32_BITFIELDS(&arb->mtk_pmif->spi_mode_ctrl,
			SPI_MODE_CTRL_SRCLK_EN, 0,
			SPI_MODE_CTRL_SRVOL_EN, 0,
			SPI_MODE_CTRL_PMIF_RDY, 1);

	SET32_BITFIELDS(&arb->mtk_pmif->sleep_protection_ctrl,
			SPM_SLEEP_REQ_SEL, 1,
			SCP_SLEEP_REQ_SEL, 1);
}
