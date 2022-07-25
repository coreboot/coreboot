/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <reset.h>

#include "gpio.h"

void do_board_reset(void)
{
	gpio_output(GPIO_AP_EC_WARM_RST_REQ, 1);
}
