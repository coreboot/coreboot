/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <ec/kontron/kempld/kempld.h>

void bootblock_mainboard_early_init(void)
{
	kempld_enable_uart_for_console();
}
