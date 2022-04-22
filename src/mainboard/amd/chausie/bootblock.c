/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/espi.h>
#include "ec.h"
#include "gpio.h"

void bootblock_mainboard_early_init(void)
{
	mainboard_program_early_gpios();

	espi_switch_to_spi1_pads();
}

void bootblock_mainboard_init(void)
{
	chausie_ec_init();
}
