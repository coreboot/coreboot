/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <program_loading.h>
#include <soc/mmu.h>
#include <soc/aop_common.h>
#include <soc/clock.h>

void aop_fw_load_reset(void)
{
	struct prog aop_fw_prog =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/aop");

	if (!selfload(&aop_fw_prog))
		die("SOC image: AOP load failed");

	clock_reset_aop();

	printk(BIOS_DEBUG, "SOC:AOP brought out of reset.\n");
}
