/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <ec/acpi/ec.h>

void bootblock_mainboard_early_init(void)
{
	/* Enable USB Power. We need to do it early for usbdebug to work. */
	ec_set_bit(0x3b, 4);
}
