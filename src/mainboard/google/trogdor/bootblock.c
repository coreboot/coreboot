/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include "board.h"

void bootblock_mainboard_init(void)
{
	setup_chromeos_gpios();
}
