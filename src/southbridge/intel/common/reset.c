/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <cf9_reset.h>
#include <reset.h>

void do_board_reset(void)
{
	system_reset();
}
