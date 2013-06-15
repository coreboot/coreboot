/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include "drivers/pc80/i8254.c"
#include "southbridge.h"

/************************************************************************/
/* south bridge configuration space access routines			*/
/************************************************************************/

uint8_t pci_read_sb_config8(unsigned where)
{
	return (pci_io_read_config8(SB, where));
}

void pci_write_sb_config8(unsigned where, uint8_t value)
{
	pci_io_write_config8(SB, where, value);
}

uint16_t pci_read_sb_config16(unsigned where)
{
	return (pci_io_read_config16(SB, where));
}

void pci_write_sb_config16(unsigned where, uint16_t value)
{
	pci_io_write_config16(SB, where, value);
}

uint32_t pci_read_sb_config32(unsigned where)
{
	return (pci_io_read_config32(SB, where));
}

void pci_write_sb_config32(unsigned where, uint32_t value)
{
	pci_io_write_config32(SB, where, value);
}

/************************************************************************/
/* bootblock south bridge initialize					*/
/************************************************************************/

void bootblock_southbridge_init(void)
{
	/* enable fast ga20 */
	pci_write_sb_config8(SB_REG_LPCCR, pci_read_sb_config8(SB_REG_LPCCR) | 0x16);
	outb(inb(SYSTEM_CTL_PORT) | 0x02, SYSTEM_CTL_PORT);
	setup_i8254();
}
