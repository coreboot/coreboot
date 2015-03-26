/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <console/console.h>
#include <console/uart.h>
#include <console/streams.h>
#include <device/pci.h>
#include <option.h>
#include <rules.h>
#include <version.h>

/* While in romstage, console loglevel is built-time constant. */
static ROMSTAGE_CONST int console_loglevel = CONFIG_DEFAULT_CONSOLE_LOGLEVEL;

int console_log_level(int msg_level)
{
	return (console_loglevel >= msg_level);
}

void console_init(void)
{
#if !defined(__PRE_RAM__)
	console_loglevel = CONFIG_DEFAULT_CONSOLE_LOGLEVEL;
	get_option(&console_loglevel, "debug_level");
#endif

#if CONFIG_EARLY_PCI_BRIDGE && !defined(__SMM__)
	pci_early_bridge_init();
#endif

	console_hw_init();

	printk(BIOS_INFO, "\n\ncoreboot-%s%s %s %s starting...\n",
		      coreboot_version, coreboot_extra_version, coreboot_build,
#if ENV_BOOTBLOCK
		      "bootblock"
#elif ENV_ROMSTAGE
		      "romstage"
#elif ENV_RAMSTAGE
		      "ramstage"
#elif ENV_VERSTAGE
		      "verstage"
#elif ENV_SECMON
		      "secmon"
#else
		      "UNKNOWN"
#endif
		      );
}
