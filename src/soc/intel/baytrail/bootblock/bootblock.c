/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied wacbmem_entryanty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <arch/io.h>
#include <baytrail/iosf.h>

static void bootblock_cpu_init(void)
{
	uint32_t reg;

	/* Set up the MMCONF range. The register lives in the BUNIT. The
	 * IO variant of the config access needs to be used initially to
	 * properly configure as the IOSF access registers live in PCI
	 * config space. */
	reg = 0;
	/* Clear the extended register. */
	pci_io_write_config32(IOSF_PCI_DEV, MCRX_REG, reg);
	reg = CONFIG_MMCONF_BASE_ADDRESS | 1;
	pci_io_write_config32(IOSF_PCI_DEV, MDR_REG, reg);
	reg = IOSF_OPCODE(IOSF_OP_WRITE_BUNIT) | IOSF_PORT(IOSF_PORT_BUNIT) |
	      IOSF_REG(BUNIT_MMCONF_REG) | IOSF_BYTE_EN;
	pci_io_write_config32(IOSF_PCI_DEV, MCR_REG, reg);
}
