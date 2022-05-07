/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <console/console.h>
#include <cpu/intel/haswell/haswell.h>
#include <delay.h>
#include <device/pci_ops.h>
#include <northbridge/intel/haswell/chip.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/raminit.h>
#include <static.h>
#include <string.h>
#include <types.h>

#include "raminit_native.h"

static enum raminit_status pre_training(struct sysinfo *ctrl)
{
	/* Skip on S3 resume */
	if (ctrl->bootmode == BOOTMODE_S3)
		return RAMINIT_STATUS_SUCCESS;

	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		for (uint8_t slot = 0; slot < NUM_SLOTS; slot++) {
			if (!rank_in_ch(ctrl, slot + slot, channel))
				continue;

			printk(RAM_DEBUG, "C%uS%u:\n", channel, slot);
			printk(RAM_DEBUG, "\tMR0: 0x%04x\n", ctrl->mr0[channel][slot]);
			printk(RAM_DEBUG, "\tMR1: 0x%04x\n", ctrl->mr1[channel][slot]);
			printk(RAM_DEBUG, "\tMR2: 0x%04x\n", ctrl->mr2[channel][slot]);
			printk(RAM_DEBUG, "\tMR3: 0x%04x\n", ctrl->mr3[channel][slot]);
			printk(RAM_DEBUG, "\n");
		}
		if (ctrl->is_ecc) {
			union mad_dimm_reg mad_dimm = {
				.raw = mchbar_read32(MAD_DIMM(channel)),
			};
			/* Enable ECC I/O */
			mad_dimm.ecc_mode = 1;
			mchbar_write32(MAD_DIMM(channel), mad_dimm.raw);
			/* Wait 4 usec after enabling the ECC I/O, needed by HW */
			udelay(4);
		}
	}
	setup_wdb(ctrl);
	return RAMINIT_STATUS_SUCCESS;
}

struct task_entry {
	enum raminit_status (*task)(struct sysinfo *);
	bool is_enabled;
	const char *name;
};

static const struct task_entry cold_boot[] = {
	{ collect_spd_info,                                       true, "PROCSPD",    },
	{ initialise_mpll,                                        true, "INITMPLL",   },
	{ convert_timings,                                        true, "CONVTIM",    },
	{ configure_mc,                                           true, "CONFMC",     },
	{ configure_memory_map,                                   true, "MEMMAP",     },
	{ do_jedec_init,                                          true, "JEDECINIT",  },
	{ pre_training,                                           true, "PRETRAIN",   },
	{ train_receive_enable,                                   true, "RCVET",      },
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
	ctrl->vdd_mv = is_hsw_ult() ? 1350 : 1500; /** FIXME: Hardcoded, does it matter? **/
	ctrl->dq_pins_interleaved = cfg->dq_pins_interleaved;
	ctrl->restore_mrs = false;
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
