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

#if !defined(__PRE_RAM__)
#define IOSF_PCI_BASE (CONFIG_MMCONF_BASE_ADDRESS + (IOSF_PCI_DEV << 12))

static inline void write_iosf_reg(int reg, uint32_t value)
{
	write32(IOSF_PCI_BASE + reg, value);
}
static inline uint32_t read_iosf_reg(int reg)
{
	return read32(IOSF_PCI_BASE + reg);
}
#else
static inline void write_iosf_reg(int reg, uint32_t value)
{
	pci_write_config32(IOSF_PCI_DEV, reg, value);
}
static inline uint32_t read_iosf_reg(int reg)
{
	return pci_read_config32(IOSF_PCI_DEV, reg);
}
#endif

/* Common sequences for all the port accesses. */
static uint32_t iosf_read_port(uint32_t cr, int reg)
{
	cr |= IOSF_REG(reg) | IOSF_BYTE_EN;
	write_iosf_reg(MCRX_REG, IOSF_REG_UPPER(reg));
	write_iosf_reg(MCR_REG, cr);
	return read_iosf_reg(MDR_REG);
}

static void iosf_write_port(uint32_t cr, int reg, uint32_t val)
{
	cr |= IOSF_REG(reg) | IOSF_BYTE_EN;
	write_iosf_reg(MDR_REG, val);
	write_iosf_reg(MCRX_REG, IOSF_REG_UPPER(reg));
	write_iosf_reg(MCR_REG, cr);
}

#define IOSF_READ(port) \
	IOSF_OPCODE(IOSF_OP_READ_##port) | IOSF_PORT(IOSF_PORT_##port)
#define IOSF_WRITE(port) \
	IOSF_OPCODE(IOSF_OP_WRITE_##port) | IOSF_PORT(IOSF_PORT_##port)

uint32_t iosf_bunit_read(int reg)
{
	return iosf_read_port(IOSF_READ(BUNIT), reg);
}

void iosf_bunit_write(int reg, uint32_t val)
{
	iosf_write_port(IOSF_WRITE(BUNIT), reg, val);
}

uint32_t iosf_dunit_read(int reg)
{
	return iosf_read_port(IOSF_READ(SYSMEMC), reg);
}

uint32_t iosf_dunit_ch0_read(int reg)
{
	return iosf_dunit_read(reg);
}

uint32_t iosf_dunit_ch1_read(int reg)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_READ_SYSMEMC) |
	              IOSF_PORT(IOSF_PORT_DUNIT_CH1);
	return iosf_read_port(cr, reg);
}

void iosf_dunit_write(int reg, uint32_t val)
{
	iosf_write_port(IOSF_WRITE(SYSMEMC), reg, val);
}

uint32_t iosf_punit_read(int reg)
{
	return iosf_read_port(IOSF_READ(PMC), reg);
}

void iosf_punit_write(int reg, uint32_t val)
{
	iosf_write_port(IOSF_WRITE(PMC), reg, val);
}

uint32_t iosf_usbphy_read(int reg)
{
	return iosf_read_port(IOSF_READ(USBPHY), reg);
}

void iosf_usbphy_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(USBPHY), reg, val);
}

uint32_t iosf_ushphy_read(int reg)
{
	return iosf_read_port(IOSF_READ(USHPHY), reg);
}

void iosf_ushphy_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(USHPHY), reg, val);
}

uint32_t iosf_lpss_read(int reg)
{
	return iosf_read_port(IOSF_READ(LPSS), reg);
}

void iosf_lpss_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(LPSS), reg, val);
}

uint32_t iosf_ccu_read(int reg)
{
	return iosf_read_port(IOSF_READ(CCU), reg);
}

void iosf_ccu_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(CCU), reg, val);
}

uint32_t iosf_score_read(int reg)
{
	return iosf_read_port(IOSF_READ(SCORE), reg);
}

void iosf_score_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(SCORE), reg, val);
}

uint32_t iosf_scc_read(int reg)
{
	return iosf_read_port(IOSF_READ(SCC), reg);
}

void iosf_scc_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(SCC), reg, val);
}

uint32_t iosf_aunit_read(int reg)
{
	return iosf_read_port(IOSF_READ(AUNIT), reg);
}

void iosf_aunit_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(AUNIT), reg, val);
}

uint32_t iosf_cpu_bus_read(int reg)
{
	return iosf_read_port(IOSF_READ(CPU_BUS), reg);
}

void iosf_cpu_bus_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(CPU_BUS), reg, val);
}

uint32_t iosf_sec_read(int reg)
{
	return iosf_read_port(IOSF_READ(SEC), reg);
}

void iosf_sec_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(SEC), reg, val);
}

uint32_t iosf_port45_read(int reg)
{
	return iosf_read_port(IOSF_READ(0x45), reg);
}

void iosf_port45_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(0x45), reg, val);
}

uint32_t iosf_port46_read(int reg)
{
	return iosf_read_port(IOSF_READ(0x46), reg);
}

void iosf_port46_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(0x46), reg, val);
}

uint32_t iosf_port47_read(int reg)
{
	return iosf_read_port(IOSF_READ(0x47), reg);
}

void iosf_port47_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(0x47), reg, val);
}

uint32_t iosf_port55_read(int reg)
{
	return iosf_read_port(IOSF_READ(0x55), reg);
}

void iosf_port55_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(0x55), reg, val);
}

uint32_t iosf_port58_read(int reg)
{
	return iosf_read_port(IOSF_READ(0x58), reg);
}

void iosf_port58_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(0x58), reg, val);
}

uint32_t iosf_port59_read(int reg)
{
	return iosf_read_port(IOSF_READ(0x59), reg);
}

void iosf_port59_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(0x59), reg, val);
}

uint32_t iosf_port5a_read(int reg)
{
	return iosf_read_port(IOSF_READ(0x5a), reg);
}

void iosf_port5a_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(0x5a), reg, val);
}

uint32_t iosf_porta2_read(int reg)
{
	return iosf_read_port(IOSF_READ(0xa2), reg);
}

void iosf_porta2_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(0xa2), reg, val);
}

uint32_t iosf_ssus_read(int reg)
{
	return iosf_read_port(IOSF_READ(SSUS), reg);
}

void iosf_ssus_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(SSUS), reg, val);
}
