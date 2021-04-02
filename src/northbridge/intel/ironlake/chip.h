/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NORTHBRIDGE_INTEL_IRONLAKE_CHIP_H
#define NORTHBRIDGE_INTEL_IRONLAKE_CHIP_H

#include <drivers/intel/gma/i915.h>

/*
 * Digital Port Hotplug Enable:
 *  0x04 = Enabled, 2ms short pulse
 *  0x05 = Enabled, 4.5ms short pulse
 *  0x06 = Enabled, 6ms short pulse
 *  0x07 = Enabled, 100ms short pulse
 */
struct northbridge_intel_ironlake_config {
	u8 gpu_dp_b_hotplug; /* Digital Port B Hotplug Config */
	u8 gpu_dp_c_hotplug; /* Digital Port C Hotplug Config */
	u8 gpu_dp_d_hotplug; /* Digital Port D Hotplug Config */

	enum {
		PANEL_PORT_LVDS = 0,
		PANEL_PORT_DP_A = 1, /* Also known as eDP */
		PANEL_PORT_DP_C = 2,
		PANEL_PORT_DP_D = 3,
	} gpu_panel_port_select;

	u8 gpu_panel_power_cycle_delay;          /* T4 time sequence */
	u16 gpu_panel_power_up_delay;            /* T1+T2 time sequence */
	u16 gpu_panel_power_down_delay;          /* T3 time sequence */
	u16 gpu_panel_power_backlight_on_delay;  /* T5 time sequence */
	u16 gpu_panel_power_backlight_off_delay; /* Tx time sequence */

	u32 gpu_cpu_backlight;	/* CPU Backlight PWM value */
	u32 gpu_pch_backlight;	/* PCH Backlight PWM value */

	struct i915_gpu_controller_info gfx;
};

#endif /* NORTHBRIDGE_INTEL_IRONLAKE_CHIP_H */
