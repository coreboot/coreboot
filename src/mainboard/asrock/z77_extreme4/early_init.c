/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <superio/nuvoton/nct6776/nct6776.h>
#include <superio/nuvoton/common/nuvoton.h>

void bootblock_mainboard_early_init(void)
{
	/* Enable early serial */
	if (CONFIG(CONSOLE_SERIAL))
		nuvoton_enable_serial(NCT6776_SP1, CONFIG_TTYS0_BASE);
}
