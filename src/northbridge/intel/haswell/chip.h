/*
 * This file is part of the coreboot project.
 *
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

#ifndef NORTHBRIDGE_INTEL_HASWELL_CHIP_H
#define NORTHBRIDGE_INTEL_HASWELL_CHIP_H

#include <drivers/intel/gma/i915.h>

/*
 * Digital Port Hotplug Enable:
 *  0x04 = Enabled, 2ms   short pulse
 *  0x05 = Enabled, 4.5ms short pulse
 *  0x06 = Enabled, 6ms   short pulse
 *  0x07 = Enabled, 100ms short pulse
 */
struct northbridge_intel_haswell_config {
	u8 gpu_dp_b_hotplug; /* Digital Port B Hotplug Config */
	u8 gpu_dp_c_hotplug; /* Digital Port C Hotplug Config */
	u8 gpu_dp_d_hotplug; /* Digital Port D Hotplug Config */

	u8 gpu_panel_port_select; /* 0=LVDS 1=DP_B 2=DP_C 3=DP_D */
	u8 gpu_panel_power_cycle_delay;          /* T4 time sequence */
	u16 gpu_panel_power_up_delay;            /* T1+T2 time sequence */
	u16 gpu_panel_power_down_delay;          /* T3 time sequence */
	u16 gpu_panel_power_backlight_on_delay;  /* T5 time sequence */
	u16 gpu_panel_power_backlight_off_delay; /* Tx time sequence */

	unsigned int gpu_pch_backlight_pwm_hz;
	enum {
		GPU_BACKLIGHT_POLARITY_HIGH = 0,
		GPU_BACKLIGHT_POLARITY_LOW,
	} gpu_pch_backlight_polarity;

	bool gpu_ddi_e_connected;

	struct i915_gpu_controller_info gfx;
};

#endif /* NORTHBRIDGE_INTEL_HASWELL_CHIP_H */
