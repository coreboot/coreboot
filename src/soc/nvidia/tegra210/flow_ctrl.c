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
#define FLOW_CTRL_HALT_SCLK		(1 << 27)
#define FLOW_CTRL_HALT_LIC_IRQ		(1 << 11)
#define FLOW_CTRL_HALT_LIC_FIQ		(1 << 10)
#define FLOW_CTRL_HALT_GIC_IRQ		(1 << 9)
#define FLOW_CTRL_HALT_GIC_FIQ		(1 << 8)
#define FLOW_CTRL_CPU0_CSR		0x8
#define FLOW_CTRL_CSR_INTR_FLAG		(1 << 15)
#define FLOW_CTRL_CSR_EVENT_FLAG	(1 << 14)
#define FLOW_CTRL_CSR_WFI_CPU0		(1 << 8)
#define FLOW_CTRL_CSR_WFI_BITMAP	(0xF << 8)
#define FLOW_CTRL_CSR_WFE_BITMAP	(0xF << 4)
#define FLOW_CTRL_CSR_ENABLE		(1 << 0)
#define FLOW_CTRL_HALT_CPU1_EVENTS	0x14
#define FLOW_CTRL_CPU1_CSR		0x18
#define FLOW_CTRL_CC4_CORE0_CTRL	0x6c

static void *tegra_flowctrl_base = (void *)TEGRA_FLOW_BASE;

static const uint8_t flowctrl_offset_halt_cpu[] = {
	FLOW_CTRL_HALT_CPU0_EVENTS,
	FLOW_CTRL_HALT_CPU1_EVENTS,
	FLOW_CTRL_HALT_CPU1_EVENTS + 8,
	FLOW_CTRL_HALT_CPU1_EVENTS + 16
};

static const uint8_t flowctrl_offset_cpu_csr[] = {
	FLOW_CTRL_CPU0_CSR,
	FLOW_CTRL_CPU1_CSR,
	FLOW_CTRL_CPU1_CSR + 8,
	FLOW_CTRL_CPU1_CSR + 16
};

static const uint8_t flowctrl_offset_cc4_ctrl[] = {
	FLOW_CTRL_CC4_CORE0_CTRL,
	FLOW_CTRL_CC4_CORE0_CTRL + 4,
	FLOW_CTRL_CC4_CORE0_CTRL + 8,
	FLOW_CTRL_CC4_CORE0_CTRL + 12
};

void flowctrl_write_cpu_csr(int cpu, uint32_t val)
{
	write32(tegra_flowctrl_base + flowctrl_offset_cpu_csr[cpu], val);
	val = read32(tegra_flowctrl_base + flowctrl_offset_cpu_csr[cpu]);
}

void flowctrl_write_cpu_halt(int cpu, uint32_t val)
{
	write32(tegra_flowctrl_base + flowctrl_offset_halt_cpu[cpu], val);
	val = read32(tegra_flowctrl_base + flowctrl_offset_halt_cpu[cpu]);
}

void flowctrl_write_cc4_ctrl(int cpu, uint32_t val)
{
	write32(tegra_flowctrl_base + flowctrl_offset_cc4_ctrl[cpu], val);
	val = read32(tegra_flowctrl_base + flowctrl_offset_cc4_ctrl[cpu]);
}

void flowctrl_cpu_off(int cpu)
{
	uint32_t val = FLOW_CTRL_CSR_INTR_FLAG | FLOW_CTRL_CSR_EVENT_FLAG |
		FLOW_CTRL_CSR_ENABLE | (FLOW_CTRL_CSR_WFI_CPU0 << cpu);

	flowctrl_write_cpu_csr(cpu, val);
	flowctrl_write_cpu_halt(cpu, FLOW_CTRL_WAITEVENT);
	flowctrl_write_cc4_ctrl(cpu, 0);
}

void flowctrl_cpu_on(int cpu)
{
	flowctrl_write_cpu_csr(cpu, FLOW_CTRL_CSR_ENABLE);
	flowctrl_write_cpu_halt(cpu, FLOW_CTRL_WAITEVENT |
				FLOW_CTRL_HALT_SCLK);
}

void flowctrl_cpu_suspend(int cpu)
{
	uint32_t val;

	val = FLOW_CTRL_HALT_GIC_IRQ | FLOW_CTRL_HALT_GIC_FIQ |
	      FLOW_CTRL_HALT_LIC_IRQ | FLOW_CTRL_HALT_LIC_FIQ |
	      FLOW_CTRL_WAITEVENT;
	flowctrl_write_cpu_halt(cpu, val);

	val = FLOW_CTRL_CSR_INTR_FLAG | FLOW_CTRL_CSR_EVENT_FLAG |
	      FLOW_CTRL_CSR_ENABLE | (FLOW_CTRL_CSR_WFI_CPU0 << cpu);
	flowctrl_write_cpu_csr(cpu, val);
}
