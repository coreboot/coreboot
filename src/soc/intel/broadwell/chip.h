/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_INTEL_BROADWELL_CHIP_H_
#define _SOC_INTEL_BROADWELL_CHIP_H_

#include <drivers/intel/gma/gma.h>
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

	/* IGD panel configuration */
	struct i915_gpu_panel_config panel_cfg;

	bool ec_present;

	bool dq_pins_interleaved;

	/*
	 * Graphics CD Clock Frequency
	 * 0 = 337.5MHz
	 * 1 = 450MHz
	 * 2 = 540MHz
	 * 3 = 675MHz
	 */
	int cdclk;

	struct i915_gpu_controller_info gfx;
};

#endif
