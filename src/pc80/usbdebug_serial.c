/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include "../lib/usbdebug.c"

static void early_usbdebug_init(void)
{
	struct ehci_debug_info *dbg_info = (struct ehci_debug_info *)
	    (CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE - sizeof(struct ehci_debug_info));

	usbdebug_init(EHCI_BAR, EHCI_DEBUG_OFFSET, dbg_info);
}

void usbdebug_tx_byte(unsigned char data)
{
	struct ehci_debug_info *dbg_info;

	/* "Find" dbg_info structure in Cache */
	dbg_info = (struct ehci_debug_info *)
	    (CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE - sizeof(struct ehci_debug_info));

	if (dbg_info->ehci_debug) {
		dbgp_bulk_write_x(dbg_info, (char*)&data, 1);
	}
}

void usbdebug_ram_tx_byte(unsigned char data)
{
	struct ehci_debug_info *dbg_info;

	/* "Find" dbg_info structure in RAM */
	dbg_info = (struct ehci_debug_info *)
	    ((CONFIG_RAMTOP) - sizeof(struct ehci_debug_info));

	if (dbg_info->ehci_debug) {
		dbgp_bulk_write_x(dbg_info, (char*)&data, 1);
	}
}
