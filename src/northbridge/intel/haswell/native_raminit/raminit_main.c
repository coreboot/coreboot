/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <console/console.h>
#include <cpu/intel/haswell/haswell.h>
#include <delay.h>
#include <device/pci_ops.h>
#include <northbridge/intel/haswell/chip.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/raminit.h>
#include <string.h>
#include <types.h>

#include "raminit_native.h"

struct task_entry {
	enum raminit_status (*task)(struct sysinfo *);
	bool is_enabled;
	const char *name;
};

static const struct task_entry cold_boot[] = {
};

/* Return a generic stepping value to make stepping checks simpler */
static enum generic_stepping get_stepping(const uint32_t cpuid)
{
	switch (cpuid) {
	case CPUID_HASWELL_A0:
		die("Haswell stepping A0 is not supported\n");
	case CPUID_HASWELL_B0:
	case CPUID_HASWELL_ULT_B0:
	case CPUID_CRYSTALWELL_B0:
		return STEPPING_B0;
	case CPUID_HASWELL_C0:
	case CPUID_HASWELL_ULT_C0:
	case CPUID_CRYSTALWELL_C0:
		return STEPPING_C0;
	default:
		/** TODO: Add Broadwell support someday **/
		die("Unknown CPUID 0x%x\n", cpuid);
	}
}

static void initialize_ctrl(struct sysinfo *ctrl)
{
	const struct northbridge_intel_haswell_config *cfg = config_of_soc();
	const enum raminit_boot_mode bootmode = ctrl->bootmode;

	memset(ctrl, 0, sizeof(*ctrl));

	ctrl->cpu = cpu_get_cpuid();
	ctrl->stepping = get_stepping(ctrl->cpu);
	ctrl->dq_pins_interleaved = cfg->dq_pins_interleaved;
	ctrl->bootmode = bootmode;
}

static enum raminit_status try_raminit(struct sysinfo *ctrl)
{
	const struct task_entry *const schedule = cold_boot;
	const size_t length = ARRAY_SIZE(cold_boot);

	enum raminit_status status = RAMINIT_STATUS_UNSPECIFIED_ERROR;

	for (size_t i = 0; i < length; i++) {
		const struct task_entry *const entry = &schedule[i];
		assert(entry);
		assert(entry->name);
		if (!entry->is_enabled)
			continue;

		assert(entry->task);
		printk(RAM_DEBUG, "\nExecuting raminit task %s\n", entry->name);
		status = entry->task(ctrl);
		printk(RAM_DEBUG, "\n");
		if (status) {
			printk(BIOS_ERR, "raminit failed on step %s\n", entry->name);
			break;
		}
	}

	return status;
}

void raminit_main(const enum raminit_boot_mode bootmode)
{
	/*
	 * The mighty_ctrl struct. Will happily nuke the pre-RAM stack
	 * if left unattended. Make it static and pass pointers to it.
	 */
	static struct sysinfo mighty_ctrl;

	mighty_ctrl.bootmode = bootmode;
	initialize_ctrl(&mighty_ctrl);

	/** TODO: Try more than once **/
	enum raminit_status status = try_raminit(&mighty_ctrl);

	if (status != RAMINIT_STATUS_SUCCESS)
		die("Memory initialization was met with utmost failure and misery\n");

	/** TODO: Implement the required magic **/
	die("NATIVE RAMINIT: More Magic (tm) required.\n");
}
