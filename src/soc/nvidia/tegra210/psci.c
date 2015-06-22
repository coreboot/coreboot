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

#include <assert.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/psci.h>
#include <soc/addressmap.h>
#include <soc/clk_rst.h>
#include <soc/cpu.h>
#include <soc/flow_ctrl.h>
#include <soc/power.h>
#include <stdlib.h>

#include <console/console.h>

extern void tegra210_reset_handler(void);

#define TEGRA210_PM_STATE_C7	7

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
	cpu_on_entry_point = tegra210_reset_handler;
	cpu_prepare_startup(cpu_on_entry_point);
}

static size_t children_at_level(int parent_level, uint64_t mpidr)
{
	if (mpidr != 0)
		return 0;

	/*
	 * T210 has 2 clusters. Each cluster has 4 cores. Currently we are
	 * concentrating only on one of the clusters i.e. A57 cluster. For A53
	 * bringup, correct the cluster details for A53 cluster as well.
	 * Since, A57 cluster has 4 cores, level 1 has 4 children at level 0.
	 * TODO(furquan): Update for A53.
	 */
	switch (parent_level) {
	case PSCI_AFFINITY_ROOT:
		return 1;
	case PSCI_AFFINITY_LEVEL_3:
		return 1;
	case PSCI_AFFINITY_LEVEL_2:
		return 1;
	case PSCI_AFFINITY_LEVEL_1:
		return 4;
	case PSCI_AFFINITY_LEVEL_0:
		return 0;
	default:
		return 0;
	}
}

static void prepare_cpu_on(int cpu)
{
	cpu_prepare_startup(cpu_on_entry_point);
}

static void prepare_cpu_suspend(int cpu, uint32_t state_id)
{
	flowctrl_write_cc4_ctrl(cpu, 0xffffffff);
	switch (state_id) {
	case TEGRA210_PM_STATE_C7:
		flowctrl_cpu_suspend(cpu);
		break;
	default:
		return;
	}
}

static void prepare_cpu_resume(int cpu)
{
	flowctrl_write_cpu_csr(cpu, 0);
	flowctrl_write_cpu_halt(cpu, 0);
	flowctrl_write_cc4_ctrl(cpu, 0);
}

static void cpu_suspend_commit(int cpu, uint32_t state_id)
{
	int l2_flush;

	switch (state_id) {
	case TEGRA210_PM_STATE_C7:
		l2_flush = NO_L2_FLUSH;
		break;
	default:
		return;
	}

	cortex_a57_cpu_power_down(l2_flush);
	/* should never be here */
}

static int cmd_prepare(struct psci_cmd *cmd)
{
	int ret;
	struct cpu_info *ci;

	ci = cmd->target->cpu_state.ci;

	switch (cmd->type) {
	case PSCI_CMD_SUSPEND:
		cmd->state_id = cmd->state->id;
		prepare_cpu_on(ci->id);
		prepare_cpu_suspend(ci->id, cmd->state_id);
		ret = PSCI_RET_SUCCESS;
		break;
	case PSCI_CMD_RESUME:
		prepare_cpu_resume(ci->id);
		ret = PSCI_RET_SUCCESS;
		break;
	case PSCI_CMD_ON:
		prepare_cpu_on(ci->id);
		ret = PSCI_RET_SUCCESS;
		break;
	case PSCI_CMD_OFF:
		if (cmd->state_id != -1) {
			ret = PSCI_RET_INVALID_PARAMETERS;
			break;
		}
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
	case PSCI_CMD_SUSPEND:
		cpu_suspend_commit(ci->id, cmd->state_id);
		ret = PSCI_RET_SUCCESS;
		break;
	case PSCI_CMD_RESUME:
		ret = PSCI_RET_SUCCESS;
		break;
	case PSCI_CMD_ON:
		/* Take CPU out of reset */
		flowctrl_cpu_on(ci->id);
		ret = PSCI_RET_SUCCESS;
		break;
	case PSCI_CMD_OFF:
		flowctrl_cpu_off(ci->id);
		cortex_a57_cpu_power_down(NO_L2_FLUSH);
		/* Never reach here */
		ret = PSCI_RET_NOT_SUPPORTED;
		printk(BIOS_ERR, "t210 CPU%d PSCI_CMD_OFF fail\n", ci->id);
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
