/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <stddef.h>
#include <arch/io.h>
#include <baytrail/iomap.h>
#include <baytrail/iosf.h>
#include <baytrail/pci_devs.h>
#include <baytrail/pmc.h>
#include <baytrail/romstage.h>

void tco_disable(void)
{
	uint32_t reg;

	reg = inl(ACPI_BASE_ADDRESS + TCO1_CNT);
	reg |= TCO_TMR_HALT;
	outl(reg, ACPI_BASE_ADDRESS + TCO1_CNT);
}

/* This sequence signals the PUNIT to start running. */
void punit_init(void)
{
	uint32_t reg;

	/* Write bits 17:16 of SB_BIOS_CONFIG in the PUNIT. */
	reg = SB_BIOS_CONFIG_PERF_MODE | SB_BIOS_CONFIG_PDM_MODE;
	pci_write_config32(IOSF_PCI_DEV, MDR_REG, reg);
	reg = IOSF_OPCODE(IOSF_OP_WRITE_PMC) | IOSF_PORT(IOSF_PORT_PMC) |
	     IOSF_REG(SB_BIOS_CONFIG) | IOSF_BYTE_EN_2;
	pci_write_config32(IOSF_PCI_DEV, MCR_REG, reg);

	/* Write bits 1:0 of BIOS_RESET_CPL in the PUNIT. */
	reg = BIOS_RESET_CPL_ALL_DONE | BIOS_RESET_CPL_RESET_DONE;
	pci_write_config32(IOSF_PCI_DEV, MDR_REG, reg);
	reg = IOSF_OPCODE(IOSF_OP_WRITE_PMC) | IOSF_PORT(IOSF_PORT_PMC) |
	     IOSF_REG(BIOS_RESET_CPL) | IOSF_BYTE_EN_0;
	pci_write_config32(IOSF_PCI_DEV, MCR_REG, reg);
}

