/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <types.h>

#include "ec.h"
#include "gpio.h"
#include "mainboard.h"

void bootblock_mainboard_early_init(void)
{
	configure_early_gpio_pads();

	/* Enable EMI (Embedded Memory Interface) 0 on the EC */
	ec_espi_io_program_iobase(EC_IDX_PORT, EMI_0_IOBASE_INDEX, EMI_0_IO_BASE_ADDR);
}
