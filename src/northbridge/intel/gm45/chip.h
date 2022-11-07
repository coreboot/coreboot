/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NORTHBRIDGE_INTEL_GM45_CHIP_H
#define NORTHBRIDGE_INTEL_GM45_CHIP_H

#include <drivers/intel/gma/i915.h>

struct northbridge_intel_gm45_config {
	u16 gpu_panel_power_up_delay;            /* T1+T2 time sequence */
	u16 gpu_panel_power_down_delay;          /* T3 time sequence */
	u16 gpu_panel_power_backlight_on_delay;  /* T5 time sequence */
	u16 gpu_panel_power_backlight_off_delay; /* Tx time sequence */
	u8 gpu_panel_power_cycle_delay;          /* T4 time sequence */
	struct i915_gpu_controller_info gfx;
	u16 default_pwm_freq;
	u8 duty_cycle;

	/*
	 * Maximum PCI mmio size in MiB.
	 */
	u16 pci_mmio_size;
	int slfm;
};

#endif				/* NORTHBRIDGE_INTEL_GM45_CHIP_H */
