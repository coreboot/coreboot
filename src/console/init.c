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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <build.h>
#include <console/console.h>
#include <console/uart.h>
#include <option.h>

#if CONFIG_EARLY_PCI_BRIDGE
/* FIXME: ROMCC chokes on PCI headers. */
#include <device/pci.h>
#endif

void console_init(void)
{
#if !defined(__PRE_RAM__)
	if(get_option(&console_loglevel, "debug_level") != CB_SUCCESS)
		console_loglevel=CONFIG_DEFAULT_CONSOLE_LOGLEVEL;
#endif

#if CONFIG_EARLY_PCI_BRIDGE
	pci_early_bridge_init();
#endif

	console_hw_init();

#if defined(__PRE_RAM__)
	static const char console_test[] =
		"\n\ncoreboot-"
		COREBOOT_VERSION
		COREBOOT_EXTRA_VERSION
		" "
		COREBOOT_BUILD
		" starting...\n";

	print_info(console_test);
#endif
}
