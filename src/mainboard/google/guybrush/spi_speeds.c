/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/spi.h>
#include <boardid.h>
#include <stdint.h>

void mainboard_spi_fast_speed_override(uint8_t *fast_speed)
{
	uint32_t board_ver = board_id();

	if (board_ver >= CONFIG_OVERRIDE_EFS_SPI_SPEED_MIN_BOARD)
		*fast_speed = CONFIG_OVERRIDE_EFS_SPI_SPEED;
}
