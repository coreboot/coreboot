/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2014, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <soc/addressmap.h>
#include <soc/flow_ctrl.h>

#define FLOW_CTRL_HALT_CPU0_EVENTS	0x0
#define FLOW_CTRL_WAITEVENT		(2 << 29)
#define FLOW_CTRL_WAIT_FOR_INTERRUPT	(4 << 29)
#define FLOW_CTRL_HALT_LIC_IRQ		(1 << 11)
#define FLOW_CTRL_HALT_LIC_FIQ		(1 << 10)
#define FLOW_CTRL_CPU0_CSR		0x8
#define FLOW_CTRL_CSR_INTR_FLAG		(1 << 15)
#define FLOW_CTRL_CSR_EVENT_FLAG	(1 << 14)
#define FLOW_CTRL_CSR_WFI_CPU0		(1 << 8)
#define FLOW_CTRL_CSR_WFI_BITMAP	(0xF << 8)
#define FLOW_CTRL_CSR_WFE_BITMAP	(0xF << 4)
#define FLOW_CTRL_CSR_ENABLE		(1 << 0)
#define FLOW_CTRL_HALT_CPU1_EVENTS	0x14
#define FLOW_CTRL_CPU1_CSR		0x18

#define HALT_REG_CORE0 (\
	FLOW_CTRL_WAIT_FOR_INTERRUPT | \
	FLOW_CTRL_HALT_LIC_IRQ | \
	FLOW_CTRL_HALT_LIC_FIQ)

#define HALT_REG_CORE1 FLOW_CTRL_WAITEVENT

static void *tegra_flowctrl_base = (void*)TEGRA_FLOW_BASE;

static const uint8_t flowctrl_offset_halt_cpu[] = {
	FLOW_CTRL_HALT_CPU0_EVENTS,
	FLOW_CTRL_HALT_CPU1_EVENTS
};

static const uint8_t flowctrl_offset_cpu_csr[] = {
	FLOW_CTRL_CPU0_CSR,
	FLOW_CTRL_CPU1_CSR
};

static uint32_t flowctrl_read_cpu_csr(int cpu)
{
	return read32(tegra_flowctrl_base + flowctrl_offset_cpu_csr[cpu]);
}

static void flowctrl_write_cpu_csr(int cpu, uint32_t val)
{
	write32(tegra_flowctrl_base + flowctrl_offset_cpu_csr[cpu], val);
	val = read32(tegra_flowctrl_base + flowctrl_offset_cpu_csr[cpu]);
}

void flowctrl_write_cpu_halt(int cpu, uint32_t val)
{
	write32(tegra_flowctrl_base + flowctrl_offset_halt_cpu[cpu], val);
	val = read32(tegra_flowctrl_base + flowctrl_offset_halt_cpu[cpu]);
}

static void flowctrl_prepare_cpu_off(int cpu)
{
	uint32_t reg;

	reg = flowctrl_read_cpu_csr(cpu);
	reg &= ~FLOW_CTRL_CSR_WFE_BITMAP;	/* clear wfe bitmap */
	reg &= ~FLOW_CTRL_CSR_WFI_BITMAP;	/* clear wfi bitmap */
	reg |= FLOW_CTRL_CSR_INTR_FLAG;		/* clear intr flag */
	reg |= FLOW_CTRL_CSR_EVENT_FLAG;	/* clear event flag */
	reg |= FLOW_CTRL_CSR_WFI_CPU0 << cpu;	/* power gating on wfi */
	reg |= FLOW_CTRL_CSR_ENABLE;		/* enable power gating */
	flowctrl_write_cpu_csr(cpu, reg);
}

void flowctrl_cpu_off(int cpu)
{
	uint32_t reg;

	reg = cpu ? HALT_REG_CORE1 : HALT_REG_CORE0;
	flowctrl_prepare_cpu_off(cpu);
	flowctrl_write_cpu_halt(cpu, reg);
}
