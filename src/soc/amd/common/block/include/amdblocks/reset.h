/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_RESET_H
#define AMD_BLOCK_RESET_H

#include <amdblocks/acpimmio.h>
#include <arch/cache.h>
#include <console/console.h>
#include <halt.h>
#include <soc/southbridge.h>

void do_warm_reset(void);
void do_cold_reset(void);
void set_warm_reset_flag(void);
int is_warm_reset(void);

static inline __noreturn void warm_reset(void)
{
	printk(BIOS_INFO, "%s() called!\n", __func__);
	dcache_clean_all();
	do_warm_reset();
	halt();
}

static inline __noreturn void cold_reset(void)
{
	printk(BIOS_INFO, "%s() called!\n", __func__);
	dcache_clean_all();
	do_cold_reset();
	halt();
}

static inline void set_resets_to_cold(void)
{
	/* De-assert and then assert all PwrGood signals on CF9 reset. */
	pm_write16(PWR_RESET_CFG, pm_read16(PWR_RESET_CFG) | TOGGLE_ALL_PWR_GOOD);
}

#endif /* AMD_BLOCK_RESET_H */
