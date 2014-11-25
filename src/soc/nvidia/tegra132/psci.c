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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <arch/psci.h>
#include <soc/cpu.h>

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

static int cmd_prepare(struct psci_cmd *cmd)
{
	return PSCI_RET_NOT_SUPPORTED;
}

static int cmd_commit(struct psci_cmd *cmd)
{
	return PSCI_RET_NOT_SUPPORTED;
}

struct psci_soc_ops soc_psci_ops = {
	.children_at_level = &children_at_level,
	.cmd_prepare = &cmd_prepare,
	.cmd_commit = &cmd_commit,
};
