/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <program_loading.h>
#include <soc/mmu.h>
#include <soc/aop.h>
#include <soc/clock.h>

void aop_fw_load_reset(void)
{
	bool aop_fw_entry;

	struct prog aop_fw_prog =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/aop");

	if (prog_locate(&aop_fw_prog))
		die("SOC image: AOP_FW not found");

	aop_fw_entry = selfload(&aop_fw_prog);
	if (!aop_fw_entry)
		die("SOC image: AOP load failed");

	clock_reset_aop();

	printk(BIOS_DEBUG, "\nSOC:AOP brought out of reset.\n");
}
