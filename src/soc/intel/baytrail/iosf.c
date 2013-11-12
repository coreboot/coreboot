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

uint32_t iosf_bunit_read(int reg)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_READ_BUNIT) |
	              IOSF_PORT(IOSF_PORT_BUNIT);
	return iosf_read_port(cr, reg);
}

void iosf_bunit_write(int reg, uint32_t val)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_WRITE_BUNIT) |
	              IOSF_PORT(IOSF_PORT_BUNIT);
	iosf_write_port(cr, reg, val);
}

uint32_t iosf_dunit_read(int reg)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_READ_SYSMEMC) |
	              IOSF_PORT(IOSF_PORT_SYSMEMC);
	return iosf_read_port(cr, reg);
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
	uint32_t cr = IOSF_OPCODE(IOSF_OP_WRITE_SYSMEMC) |
	              IOSF_PORT(IOSF_PORT_SYSMEMC);
	iosf_write_port(cr, reg, val);
}

uint32_t iosf_punit_read(int reg)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_READ_PMC) |
	              IOSF_PORT(IOSF_PORT_PMC);
	return iosf_read_port(cr, reg);
}

void iosf_punit_write(int reg, uint32_t val)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_WRITE_PMC) |
	              IOSF_PORT(IOSF_PORT_PMC);
	iosf_write_port(cr, reg, val);
}

uint32_t iosf_usbphy_read(int reg)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_READ_USBPHY) |
	              IOSF_PORT(IOSF_PORT_USBPHY);
	return iosf_read_port(cr, reg);
}

void iosf_usbphy_write(int reg, uint32_t val)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_WRITE_USBPHY) |
	              IOSF_PORT(IOSF_PORT_USBPHY);
	return iosf_write_port(cr, reg, val);
}

uint32_t iosf_ushphy_read(int reg)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_READ_USHPHY) |
	              IOSF_PORT(IOSF_PORT_USHPHY);
	return iosf_read_port(cr, reg);
}

void iosf_ushphy_write(int reg, uint32_t val)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_WRITE_USHPHY) |
	              IOSF_PORT(IOSF_PORT_USHPHY);
	return iosf_write_port(cr, reg, val);
}

uint32_t iosf_lpss_read(int reg)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_READ_LPSS) |
	              IOSF_PORT(IOSF_PORT_LPSS);
	return iosf_read_port(cr, reg);
}

void iosf_lpss_write(int reg, uint32_t val)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_WRITE_LPSS) |
	              IOSF_PORT(IOSF_PORT_LPSS);
	return iosf_write_port(cr, reg, val);
}

uint32_t iosf_ccu_read(int reg)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_READ_CCU) |
	              IOSF_PORT(IOSF_PORT_CCU);
	return iosf_read_port(cr, reg);
}

void iosf_ccu_write(int reg, uint32_t val)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_WRITE_CCU) |
	              IOSF_PORT(IOSF_PORT_CCU);
	return iosf_write_port(cr, reg, val);
}

uint32_t iosf_score_read(int reg)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_READ_SCORE) |
	              IOSF_PORT(IOSF_PORT_SCORE);
	return iosf_read_port(cr, reg);
}

void iosf_score_write(int reg, uint32_t val)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_WRITE_SCORE) |
	              IOSF_PORT(IOSF_PORT_SCORE);
	return iosf_write_port(cr, reg, val);
}

uint32_t iosf_scc_read(int reg)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_READ_SCC) |
	              IOSF_PORT(IOSF_PORT_SCC);
	return iosf_read_port(cr, reg);
}

void iosf_scc_write(int reg, uint32_t val)
{
	uint32_t cr = IOSF_OPCODE(IOSF_OP_WRITE_SCC) |
	              IOSF_PORT(IOSF_PORT_SCC);
	return iosf_write_port(cr, reg, val);
}
