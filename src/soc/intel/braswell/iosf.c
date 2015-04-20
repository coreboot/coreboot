/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 * Copyright (C) 2015 Intel Corp.
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
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <console/console.h>
#include <soc/iosf.h>

#if ENV_RAMSTAGE
#define IOSF_PCI_BASE (CONFIG_MMCONF_BASE_ADDRESS + (IOSF_PCI_DEV << 12))

static inline void write_iosf_reg(int reg, uint32_t value)
{
	write32((void *)(IOSF_PCI_BASE + reg), value);
}
static inline uint32_t read_iosf_reg(int reg)
{
	return read32((void *)(IOSF_PCI_BASE + reg));
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
#endif /* ENV_RAMSTAGE */

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
	(IOSF_OPCODE(IOSF_OP_READ_##port) | IOSF_PORT(IOSF_PORT_##port))
#define IOSF_WRITE(port) \
	(IOSF_OPCODE(IOSF_OP_WRITE_##port) | IOSF_PORT(IOSF_PORT_##port))

uint32_t iosf_bunit_read(int reg)
{
	return iosf_read_port(IOSF_READ(BUNIT), reg);
}

void iosf_bunit_write(int reg, uint32_t val)
{
	iosf_write_port(IOSF_WRITE(BUNIT), reg, val);
}

uint32_t iosf_punit_read(int reg)
{
	return iosf_read_port(IOSF_READ(PMC), reg);
}

void iosf_punit_write(int reg, uint32_t val)
{
	iosf_write_port(IOSF_WRITE(PMC), reg, val);
}

uint32_t iosf_score_read(int reg)
{
	return iosf_read_port(IOSF_READ(SCORE), reg);
}

void iosf_score_write(int reg, uint32_t val)
{
	iosf_write_port(IOSF_WRITE(SCORE), reg, val);
}

uint32_t iosf_lpss_read(int reg)
{
	return iosf_read_port(IOSF_READ(LPSS), reg);
}

void iosf_lpss_write(int reg, uint32_t val)
{
	iosf_write_port(IOSF_WRITE(LPSS), reg, val);
}

uint32_t iosf_port58_read(int reg)
{
	return iosf_read_port(IOSF_READ(0x58), reg);
}

void iosf_port58_write(int reg, uint32_t val)
{
	iosf_write_port(IOSF_WRITE(0x58), reg, val);
}

uint32_t iosf_scc_read(int reg)
{
	return iosf_read_port(IOSF_READ(SCC), reg);
}

void iosf_scc_write(int reg, uint32_t val)
{
	return iosf_write_port(IOSF_WRITE(SCC), reg, val);
}


#if ENV_RAMSTAGE
uint64_t reg_script_read_iosf(struct reg_script_context *ctx)
{
	const struct reg_script *step = ctx->step;

	/* Process the request */
	switch (step->id) {
	case IOSF_PORT_BUNIT:
		return iosf_bunit_read(step->reg);
	case IOSF_PORT_SCORE:
		return iosf_score_read(step->reg);
	case IOSF_PORT_LPSS:
		return iosf_lpss_read(step->reg);
	case IOSF_PORT_0x58:
		return iosf_port58_read(step->reg);
	case IOSF_PORT_SCC:
		return iosf_scc_read(step->reg);
	default:
		printk(BIOS_DEBUG, "No read support for IOSF port 0x%x.\n",
		       step->id);
		break;
	}
	return 0;
}

void reg_script_write_iosf(struct reg_script_context *ctx)
{
	const struct reg_script *step = ctx->step;

	/* Process the request */
	switch (step->id) {
	case IOSF_PORT_BUNIT:
		iosf_bunit_write(step->reg, step->value);
		break;
	case IOSF_PORT_SCORE:
		iosf_score_write(step->reg, step->value);
		break;
	case IOSF_PORT_LPSS:
		iosf_lpss_write(step->reg, step->value);
		break;
	case IOSF_PORT_0x58:
		iosf_port58_write(step->reg, step->value);
		break;
	case IOSF_PORT_SCC:
		iosf_scc_write(step->reg, step->value);
		break;

	default:
		printk(BIOS_DEBUG, "No write support for IOSF port 0x%x.\n",
		       step->id);
		break;
	}
}

const struct reg_script_bus_entry reg_script_bus_table[] = {
	{REG_SCRIPT_TYPE_IOSF, reg_script_read_iosf, reg_script_write_iosf}
};

const struct reg_script_bus_entry *platform_bus_table(size_t *table_entries)
{
	/* Return the table size and address */
	*table_entries = sizeof(reg_script_bus_table)
		/ sizeof(reg_script_bus_table[0]);
	return &reg_script_bus_table[0];
}

#endif	/* ENV_RAMSTAGE */
