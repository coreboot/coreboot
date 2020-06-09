/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <console/console.h>
#include <console/uart.h>
#include <console/streams.h>
#include <device/pci.h>
#include <option.h>
#include <version.h>

/* Mutable console log level only allowed when RAM comes online. */
#define CONSOLE_LEVEL_CONST !ENV_STAGE_HAS_DATA_SECTION

static int console_inited;
static int console_loglevel = CONFIG_DEFAULT_CONSOLE_LOGLEVEL;

static inline int get_log_level(void)
{
	if (console_inited == 0)
		return -1;
	if (CONSOLE_LEVEL_CONST)
		return get_console_loglevel();

	return console_loglevel;
}

static inline void set_log_level(int new_level)
{
	if (CONSOLE_LEVEL_CONST)
		return;

	console_loglevel = new_level;
}

static void init_log_level(void)
{
	int debug_level = get_console_loglevel();

	if (CONSOLE_LEVEL_CONST)
		return;

	get_option(&debug_level, "debug_level");

	set_log_level(debug_level);
}

int console_log_level(int msg_level)
{
	int log_level = get_log_level();

	if (log_level < 0)
		return CONSOLE_LOG_NONE;

	if (msg_level <= log_level)
		return CONSOLE_LOG_ALL;

	if (CONFIG(CONSOLE_CBMEM) && (msg_level <= BIOS_DEBUG))
		return CONSOLE_LOG_FAST;

	return 0;
}

asmlinkage void console_init(void)
{
	init_log_level();

	if (CONFIG(DEBUG_CONSOLE_INIT))
		console_inited = 1;

	if (CONFIG(EARLY_PCI_BRIDGE) && (ENV_BOOTBLOCK || ENV_ROMSTAGE))
		pci_early_bridge_init();

	console_hw_init();

	console_inited = 1;

	printk(BIOS_NOTICE, "\n\ncoreboot-%s%s %s " ENV_STRING " starting (log level: %i)...\n",
	       coreboot_version, coreboot_extra_version, coreboot_build,
	       get_log_level());
}
