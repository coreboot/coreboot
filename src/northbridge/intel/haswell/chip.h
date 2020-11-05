/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NORTHBRIDGE_INTEL_HASWELL_CHIP_H
#define NORTHBRIDGE_INTEL_HASWELL_CHIP_H

#include <drivers/intel/gma/gma.h>
#include <types.h>

struct peg_config {
	bool is_onboard;
	uint8_t power_limit_scale;
	uint8_t power_limit_value;
	uint16_t phys_slot_number;
};

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

	/* IGD panel configuration */
	struct i915_gpu_panel_config panel_cfg;

	struct peg_config peg_cfg[3];

	bool gpu_ddi_e_connected;

	bool ec_present;

	bool dq_pins_interleaved;

	bool usb_xhci_on_resume;

	struct i915_gpu_controller_info gfx;
};

#endif /* NORTHBRIDGE_INTEL_HASWELL_CHIP_H */
