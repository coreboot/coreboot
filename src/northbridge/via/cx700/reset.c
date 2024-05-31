/* SPDX-License-Identifier: GPL-2.0-only */

#include <reset.h>
#include <cf9_reset.h>

void do_board_reset(void)
{
	full_reset();
}
