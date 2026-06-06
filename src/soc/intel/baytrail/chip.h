/* SPDX-License-Identifier: GPL-2.0-only */

/* The devicetree parser expects chip.h to reside directly in the path
 * specified by the devicetree. */

#ifndef _BAYTRAIL_CHIP_H_
#define _BAYTRAIL_CHIP_H_

#include <drivers/intel/gma/i915.h>
#include <stdbool.h>
#include <stdint.h>

struct soc_intel_baytrail_config {
	bool enable_xdp_tap;
	uint8_t sata_port_map;
	bool sata_ahci;
	bool ide_legacy_combined;
	bool clkreq_enable;

	/* VR low power settings -- enable PS2 mode for gfx and core */
	bool vnn_ps2_enable;
	bool vcc_ps2_enable;

	/* Disable SLP_X stretching after SUS power well loss. */
	bool disable_slp_x_stretch_sus_fail;

	/* USB Port Disable mask */
	uint16_t usb2_port_disable_mask;
	uint16_t usb3_port_disable_mask;

	/* USB routing */
	bool usb_route_to_xhci;

	/* USB PHY settings specific to the board */
	uint32_t usb2_per_port_lane0;
	uint32_t usb2_per_port_rcomp_hs_pullup0;
	uint32_t usb2_per_port_lane1;
	uint32_t usb2_per_port_rcomp_hs_pullup1;
	uint32_t usb2_per_port_lane2;
	uint32_t usb2_per_port_rcomp_hs_pullup2;
	uint32_t usb2_per_port_lane3;
	uint32_t usb2_per_port_rcomp_hs_pullup3;
	uint32_t usb2_comp_bg;

	/* LPE Audio Clock configuration. */
	int lpe_codec_clk_freq; /* 19 or 25 are valid. */
	int lpe_codec_clk_num; /* Platform clock pins. [0:5] are valid. */

	/* Native SD Card controller - override controller capabilities. */
	uint32_t sdcard_cap_low;
	uint32_t sdcard_cap_high;

	/* Enable devices in ACPI mode */
	bool lpss_acpi_mode;
	bool scc_acpi_mode;
	bool lpe_acpi_mode;

	/* Allow PCIe devices to wake system from suspend. */
	bool pcie_wake_enable;

	uint8_t gpu_pipea_port_select;	/* Port select: 1=DP_B 2=DP_C */
	uint16_t gpu_pipea_power_on_delay;
	uint16_t gpu_pipea_light_on_delay;
	uint16_t gpu_pipea_power_off_delay;
	uint16_t gpu_pipea_light_off_delay;
	uint16_t gpu_pipea_power_cycle_delay;
	int gpu_pipea_pwm_freq_hz;

	uint8_t gpu_pipeb_port_select;	/* Port select: 1=DP_B 2=DP_C */
	uint16_t gpu_pipeb_power_on_delay;
	uint16_t gpu_pipeb_light_on_delay;
	uint16_t gpu_pipeb_power_off_delay;
	uint16_t gpu_pipeb_light_off_delay;
	uint16_t gpu_pipeb_power_cycle_delay;
	int gpu_pipeb_pwm_freq_hz;
	bool disable_ddr_2x_refresh_rate;

	struct i915_gpu_controller_info gfx;
};

#endif /* _BAYTRAIL_CHIP_H_ */
