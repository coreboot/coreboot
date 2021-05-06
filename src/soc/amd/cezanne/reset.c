/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <cf9_reset.h>
#include <reset.h>
#include <soc/southbridge.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/reset.h>

/* TODO: is NCP_ERR still valid?  It appears reserved and always 0xff.  b/184281092 */
void set_warm_reset_flag(void)
{
	uint8_t ncp = inw(NCP_ERR);

	outb(NCP_ERR, ncp | NCP_WARM_BOOT);
}

int is_warm_reset(void)
{
	return !!(inb(NCP_ERR) & NCP_WARM_BOOT);
}

void do_cold_reset(void)
{
	/* De-assert and then assert all PwrGood signals on CF9 reset. */
	pm_write16(PWR_RESET_CFG, pm_read16(PWR_RESET_CFG) |
		TOGGLE_ALL_PWR_GOOD);
	outb(RST_CPU | SYS_RST, RST_CNT);
}

void do_warm_reset(void)
{
	/* Warm resets are not supported and must be executed as cold */
	pm_write16(PWR_RESET_CFG, pm_read16(PWR_RESET_CFG) |
		TOGGLE_ALL_PWR_GOOD);
	outb(RST_CPU | SYS_RST, RST_CNT);
}

void do_board_reset(void)
{
	do_cold_reset();
}
