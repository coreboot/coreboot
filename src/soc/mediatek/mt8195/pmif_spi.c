/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <soc/iocfg.h>
#include <soc/pmif_spi.h>

/* IOCFG_BM, PWRAP_SPI0_DRIVING */
DEFINE_BITFIELD(PWRAP_SPI0_DRIVING, 29, 24)
DEFINE_BITFIELD(PWRAP_SPI1_DRIVING, 5, 0)

void pmif_spi_iocfg(void)
{
	/* Set SoC SPI IO driving strength to 6 mA */
	SET32_BITFIELDS(&mtk_iocfg_bm->drv_cfg1, PWRAP_SPI0_DRIVING, IO_6_MA);
	SET32_BITFIELDS(&mtk_iocfg_bm->drv_cfg2, PWRAP_SPI1_DRIVING, IO_6_MA);
}
