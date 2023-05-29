/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <drivers/vpd/vpd.h>
#include <drivers/ocp/include/vpd.h>
#include <string.h>
#include <program_loading.h>

#define CMDLINE_LOGLVL_STR	"loglevel="

static void overwrite_kernel_loglevel(uintptr_t start)
{
	int val;
	if (!vpd_get_int(KERNEL_LOG_LEVEL, VPD_RW_THEN_RO, &val)) {
		printk(BIOS_DEBUG, "%s: not able to get VPD %s\n", __func__, KERNEL_LOG_LEVEL);
		return;
	}

	printk(BIOS_DEBUG, "%s: VPD %s, got %d\n", __func__, KERNEL_LOG_LEVEL, val);
	if (val < 0 || val > 7) {
		printk(BIOS_INFO, "Invalid VPD for Linux kernel log level\n");
		return;
	}

	int loglevel;
	char *loc = strstr((const char *)start, CMDLINE_LOGLVL_STR);
	if (!loc) {
		printk(BIOS_INFO, "%s is not found from LINUX_COMMAND_LINE\n",
			CMDLINE_LOGLVL_STR);
		return;
	}

	char *loc_bkup;
	loc += strlen(CMDLINE_LOGLVL_STR);
	loc_bkup = loc;
	loglevel = skip_atoi(&loc);
	printk(BIOS_DEBUG, "Original kernel log level is %d\n", loglevel);
	/* Unlikely but don't overwrite with such an unexpected case. */
	if (loglevel < 0 || loglevel > 7) {
		printk(BIOS_DEBUG, "Invalid kernel log level, must be from 0 to 7.\n");
		return;
	}

	char c = '0' + val;
	printk(BIOS_INFO, "Overwrite kernel log level with %c from VPD.\n", c);
	memcpy(loc_bkup, &c, 1);
}

void platform_segment_loaded(uintptr_t start, size_t size, int flags)
{
	/* CONFIG_LINUX_COMMAND_LINE is in the final segment. */
	if (flags != SEG_FINAL)
		return;

	overwrite_kernel_loglevel(start);
}
