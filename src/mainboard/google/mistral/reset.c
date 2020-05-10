/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <reset.h>

#define GCNT_PSHOLD			((void *)0x004AB000u)
#define TCSR_BOOT_MISC_DETECT		((void *)0x0193D100)
#define TCSR_RESET_DEBUG_SW_ENTRY	((void *)0x01940000)

void do_board_reset(void)
{
	/*
	 * At boot time the boot loaders would have set a magic cookie
	 * here to detect watchdog reset. However, since this is a
	 * normal reset clear the magic numbers.
	 */
	write32(TCSR_BOOT_MISC_DETECT, 0);
	write32(TCSR_RESET_DEBUG_SW_ENTRY, 0);
	write32(GCNT_PSHOLD, 0);
}
