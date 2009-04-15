/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric W. Biederman <ebiederm@xmission.com>
 * Copyright (C) 2003 Linux Networx
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <pci.h>
#include <pci_ids.h>
#include <arch/io.h>
#include <southbridge/amd/amd768.h>


void amd768_hard_reset(void)
{
	pci_set_method();
	/* Allow the watchdog timer to reboot us, and enable 0xcf9 */
	pcibios_write_config_byte(0, (AMD768_DEV >> 8) | 3, 0x41, (1<<5)|(1<<1));
	/* Try rebooting though port 0xcf9 */
	outb((0<<3)|(1<<2)|(1<<1), 0xcf9);
	return;
}
