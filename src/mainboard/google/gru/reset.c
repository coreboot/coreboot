/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <reset.h>

#include "board.h"

void do_board_reset(void)
{
	gpio_output(GPIO_RESET, 1);
}
