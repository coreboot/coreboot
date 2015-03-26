/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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
 * Foundation, Inc.
 */

#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/psci.h>
#include <soc/addressmap.h>
#include <soc/clk_rst.h>
#include <soc/cpu.h>
#include <soc/flow_ctrl.h>
#include <soc/power.h>

#include <console/console.h>

static void *cpu_on_entry_point;

void psci_soc_init(uintptr_t cpu_on_entry)
{
	/*
	 * Stash secmon entry point for CPUs starting up. The 32-bit reset
	 * vector register is accessible in < EL3 so one has to attempt to
	 * plug the potential race for that register being changed out from
	 * under us. Therefore, we set the appropriate registers here, but
	 * it is also done on each CPU_ON request.
	 */
	cpu_on_entry_point = (void *)cpu_on_entry;
	cpu_prepare_startup(cpu_on_entry_point);
}

static size_t children_at_level(int parent_level, uint64_t mpidr)
{
	if (mpidr != 0)
		return 0;

	/* T132 just has 2 cores. 0. Level 1 has 2 children at level 0. */
	switch (parent_level) {
	case PSCI_AFFINITY_ROOT:
		return 1;
	case PSCI_AFFINITY_LEVEL_3:
		return 1;
	case PSCI_AFFINITY_LEVEL_2:
		return 1;
	case PSCI_AFFINITY_LEVEL_1:
		return 2;
	case PSCI_AFFINITY_LEVEL_0:
		return 0;
	default:
		return 0;
	}
}

#define TEGRA132_PM_CORE_C7	0x3

static inline void tegra132_enter_sleep(unsigned long pmstate)
{
	asm volatile(
	"       isb\n"
	"       msr actlr_el1, %0\n"
	"       wfi\n"
	:
	: "r" (pmstate));
}

static void prepare_cpu_on(int cpu)
{
	uint32_t partid;

	partid = cpu ? POWER_PARTID_CE1 : POWER_PARTID_CE0;

	power_ungate_partition(partid);
	flowctrl_write_cpu_halt(cpu, 0);
}

static int cmd_prepare(struct psci_cmd *cmd)
{
	int ret;

	switch (cmd->type) {
	case PSCI_CMD_ON:
		prepare_cpu_on(cmd->target->cpu_state.ci->id);
		ret = PSCI_RET_SUCCESS;
		break;
	case PSCI_CMD_OFF:
		if (cmd->state_id != -1) {
			ret = PSCI_RET_INVALID_PARAMETERS;
			break;
		}
		cmd->state_id = TEGRA132_PM_CORE_C7;
		ret = PSCI_RET_SUCCESS;
		break;
	default:
		ret = PSCI_RET_NOT_SUPPORTED;
		break;
	}
	return ret;
}

static int cmd_commit(struct psci_cmd *cmd)
{
	int ret;
	struct cpu_info *ci;

	ci = cmd->target->cpu_state.ci;

	switch (cmd->type) {
	case PSCI_CMD_ON:
		/* Take CPU out of reset */
		start_cpu_silent(ci->id, cpu_on_entry_point);
		ret = PSCI_RET_SUCCESS;
		break;
	case PSCI_CMD_OFF:
		flowctrl_cpu_off(ci->id);
		tegra132_enter_sleep(cmd->state_id);
		/* Never reach here */
		ret = PSCI_RET_NOT_SUPPORTED;
		printk(BIOS_ERR, "t132 CPU%d PSCI_CMD_OFF fail\n", ci->id);
		break;
	default:
		ret = PSCI_RET_NOT_SUPPORTED;
		break;
	}
	return ret;
}

struct psci_soc_ops soc_psci_ops = {
	.children_at_level = &children_at_level,
	.cmd_prepare = &cmd_prepare,
	.cmd_commit = &cmd_commit,
};
