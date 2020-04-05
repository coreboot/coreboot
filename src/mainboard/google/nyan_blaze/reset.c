/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <gpio.h>
#include <reset.h>

void do_board_reset(void)
{
        gpio_output(GPIO(I5), 0);
}
