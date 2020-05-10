/* SPDX-License-Identifier: GPL-2.0-only */

#include <reset.h>

#include "gpio.h"

void do_board_reset(void)
{
	gpio_output(AP_SYS_RESET_L, 0);
}
