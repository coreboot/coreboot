/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <reset.h>

#define SLCR_UNLOCK ((void *)0xF8000008u)
#define SLCR_UNLOCK_KEY 0xDF0D
#define PSS_RST_CTRL ((void *)0xF8000200u)
void do_board_reset(void)
{
	write16(SLCR_UNLOCK, SLCR_UNLOCK_KEY);
	write32(PSS_RST_CTRL, 1);
}
