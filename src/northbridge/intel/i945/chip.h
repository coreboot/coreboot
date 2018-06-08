/*
 * This file is part of the coreboot project.
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

#ifndef NORTHBRIDGE_INTEL_I945_CHIP_H
#define NORTHBRIDGE_INTEL_I945_CHIP_H

#include <drivers/intel/gma/i915.h>

struct northbridge_intel_i945_config {
	/* In units of 100us timer */
	/* Timings as defined in VESA Notebook Panel Standard */
	u16 gpu_panel_power_up_delay;            /* T1+T2 time sequence */
	u16 gpu_panel_power_down_delay;          /* T3 time sequence */
	u16 gpu_panel_power_backlight_on_delay;  /* T5 time sequence */
	u16 gpu_panel_power_backlight_off_delay; /* Tx time sequence */
	/* In units of 0.1s */
	u8 gpu_panel_power_cycle_delay;

	u32 gpu_hotplug;
	u32 pwm_freq;
	int gpu_lvds_use_spread_spectrum_clock;
	struct i915_gpu_controller_info gfx;
	int pci_mmio_size;
};

#endif /* NORTHBRIDGE_INTEL_I945_CHIP_H */
