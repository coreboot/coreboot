/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/amd/agesa/state_machine.h>

void board_BeforeAgesa(struct sysinfo *cb)
{
	post_code(0x30);
}
