/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <cf9_reset.h>
#include <reset.h>
#include <soc/southbridge.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/reset.h>

void do_cold_reset(void)
{
	set_resets_to_cold();
	outb(RST_CPU | SYS_RST, RST_CNT);
}

void do_warm_reset(void)
{
	/* If warm resets are not supported, executed a cold reset */
	if (!CONFIG(SOC_AMD_SUPPORTS_WARM_RESET))
		do_cold_reset(); /* Does not return */

	outb(RST_CPU | SYS_RST, RST_CNT);
}

void do_board_reset(void)
{
	do_cold_reset();
}
