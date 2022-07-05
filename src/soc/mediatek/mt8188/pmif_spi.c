/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/iocfg.h>
#include <soc/pmif_spi.h>

/* IOCFG_BM, PWRAP_SPI_DRIVING */
DEFINE_BITFIELD(PWRAP_SPI_DRIVING, 11, 9)

void pmif_spi_iocfg(void)
{
	/* Set SoC SPI IO driving strength to 6 mA */
	SET32_BITFIELDS(&mtk_iocfg_lt->drv_cfg2, PWRAP_SPI_DRIVING, IO_6_MA);
}
