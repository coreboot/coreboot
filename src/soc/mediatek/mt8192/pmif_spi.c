/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/iocfg.h>
#include <soc/pmif_spi.h>

/* IOCFG_LM, PWRAP_SPI0_DRIVING */
DEFINE_BITFIELD(PWRAP_SPI0_DRIVING, 2, 0)

void pmif_spi_iocfg(void)
{
	/* Set SoC SPI IO driving strength to 4 mA */
	SET32_BITFIELDS(&mtk_iocfg_lm->drv_cfg1, PWRAP_SPI0_DRIVING, IO_4_MA);
}
