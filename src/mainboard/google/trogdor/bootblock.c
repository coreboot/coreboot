/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include "board.h"
#include <soc/qcom_qup_se.h>
#include <soc/qupv3_spi.h>

void bootblock_mainboard_init(void)
{
	setup_chromeos_gpios();
	qup_spi_init(QUPV3_1_SE0, 1010 * KHz); /* H1 SPI */
	qup_spi_init(QUPV3_0_SE0, 1010 * KHz); /* EC SPI */
}
