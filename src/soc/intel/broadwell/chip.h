/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_INTEL_BROADWELL_CHIP_H_
#define _SOC_INTEL_BROADWELL_CHIP_H_

#include <drivers/intel/gma/i915.h>
#include <stdint.h>

struct soc_intel_broadwell_config {
	/*
	 * Digital Port Hotplug Enable:
	 *  0x04 = Enabled, 2ms short pulse
	 *  0x05 = Enabled, 4.5ms short pulse
	 *  0x06 = Enabled, 6ms short pulse
	 *  0x07 = Enabled, 100ms short pulse
	 */
	u8 gpu_dp_b_hotplug;
	u8 gpu_dp_c_hotplug;
	u8 gpu_dp_d_hotplug;

	/* Panel power sequence timings */
	u8 gpu_panel_power_cycle_delay;
	u16 gpu_panel_power_up_delay;
	u16 gpu_panel_power_down_delay;
	u16 gpu_panel_power_backlight_on_delay;
	u16 gpu_panel_power_backlight_off_delay;

	/* Panel backlight settings */
	unsigned int gpu_pch_backlight_pwm_hz;
	enum {
		GPU_BACKLIGHT_POLARITY_HIGH = 0,
		GPU_BACKLIGHT_POLARITY_LOW,
	} gpu_pch_backlight_polarity;

	/*
	 * Graphics CD Clock Frequency
	 * 0 = 337.5MHz
	 * 1 = 450MHz
	 * 2 = 540MHz
	 * 3 = 675MHz
	 */
	int cdclk;

	struct i915_gpu_controller_info gfx;

	/*
	 * Minimum voltage for C6/C7 state:
	 * 0x67 = 1.6V (full swing)
	 *  ...
	 * 0x79 = 1.7V
	 *  ...
	 * 0x83 = 1.8V (no swing)
	 */
	int vr_cpu_min_vid;

	/*
	 * Set slow VR ramp rate on C-state exit:
	 * 0 = Fast VR ramp rate / 2
	 * 1 = Fast VR ramp rate / 4
	 * 2 = Fast VR ramp rate / 8
	 * 3 = Fast VR ramp rate / 16
	 */
	int vr_slow_ramp_rate_set;

	/* Enable slow VR ramp rate */
	int vr_slow_ramp_rate_enable;

	/* Enable S0iX support */
	int s0ix_enable;

	/* TCC activation offset */
	uint32_t tcc_offset;
};

typedef struct soc_intel_broadwell_config config_t;

#endif
