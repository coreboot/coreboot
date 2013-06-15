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
#include "northbridge.h"

/************************************************************************/
/* north bridge configuration space access routines			*/
/************************************************************************/

uint8_t pci_read_nb_config8(unsigned where)
{
	return (pci_io_read_config8(NB, where));
}

void pci_write_nb_config8(unsigned where, uint8_t value)
{
	pci_io_write_config8(NB, where, value);
}

uint16_t pci_read_nb_config16(unsigned where)
{
	return (pci_io_read_config16(NB, where));
}

void pci_write_nb_config16(unsigned where, uint16_t value)
{
	pci_io_write_config16(NB, where, value);
}

uint32_t pci_read_nb_config32(unsigned where)
{
	return (pci_io_read_config32(NB, where));
}

void pci_write_nb_config32(unsigned where, uint32_t value)
{
	pci_io_write_config32(NB, where, value);
}

void pci_write_nb1_config32(unsigned where, uint32_t value)
{
	pci_io_write_config32(NB1, where, value);
}

/************************************************************************/
/* bootblock north bridge initialize					*/
/************************************************************************/

void bootblock_northbridge_init(void)
{
	pci_write_nb_config8(NB_REG_PMCR, (pci_read_nb_config8(NB_REG_PMCR)&~0xf0) | 0x05);
	pci_write_nb_config8(NB_REG_MBR, (pci_read_nb_config8(NB_REG_MBR)&0xfc));
	pci_write_nb_config8(NB_REG_SMM, 0);
	pci_write_nb_config8(NB_REG_SDRAM_MCR, (pci_read_nb_config8(NB_REG_SDRAM_MCR)&~0xf9) | 0x06);
	pci_write_nb_config8(NB_REG_MRR, 0);
	pci_write_nb_config8(NB_REG_CPU_DCR+1, (pci_read_nb_config8(NB_REG_CPU_DCR+1)&~0xfb) | 0x04);
	pci_write_nb_config8(NB_REG_PACR, (pci_read_nb_config8(NB_REG_PACR)&~0xfc) | 0x01);
	pci_write_nb_config32(NB_REG_BUF_SC, 0x83e7cf9f);
}
