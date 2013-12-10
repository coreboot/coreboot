/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* The devicetree parser expects chip.h to reside directly in the path
 * specified by the devicetree. */

#ifndef _BAYTRAIL_CHIP_H_
#define _BAYTRAIL_CHIP_H_

#include <stdint.h>

struct soc_intel_baytrail_config {
        uint8_t sata_port_map;
        uint8_t sata_ahci;
        uint8_t ide_legacy_combined;
	uint8_t clkreq_enable;

	/* USB Port Disable mask */
	uint16_t usb2_port_disable_mask;
	uint16_t usb3_port_disable_mask;

	/* USB routing */
	int usb_route_to_xhci;

	/* USB PHY settings specific to the board */
	uint32_t usb2_per_port_lane0;
	uint32_t usb2_per_port_rcomp_hs_pullup0;
	uint32_t usb2_per_port_lane1;
	uint32_t usb2_per_port_rcomp_hs_pullup1;
	uint32_t usb2_per_port_lane2;
	uint32_t usb2_per_port_rcomp_hs_pullup2;
	uint32_t usb2_per_port_lane3;
	uint32_t usb2_per_port_rcomp_hs_pullup3;

	/* LPE Audio Clock configuration. */
	int lpe_codec_clk_freq; /* 19 or 25 are valid. */
	int lpe_codec_clk_num; /* Platform clock pins. [0:5] are valid. */

	/* Native SD Card controller - override controller capabilities. */
	uint32_t sdcard_cap_low;
	uint32_t sdcard_cap_high;

	/* Enable devices in ACPI mode */
	int lpss_acpi_mode;
	int scc_acpi_mode;
	int lpe_acpi_mode;

	/*
	 * Digital Port Hotplug Enable:
	 *  0x04 = Enabled, 2ms short pulse
	 *  0x05 = Enabled, 4.5ms short pulse
	 *  0x06 = Enabled, 6ms short pulse
	 *  0x07 = Enabled, 100ms short pulse
	 */
	int gpu_pipea_hotplug;
	int gpu_pipea_port_select;	/* Port select: 1=DP_B 2=DP_C */
	uint16_t gpu_pipea_power_on_delay;
	uint16_t gpu_pipea_light_on_delay;
	uint16_t gpu_pipea_power_off_delay;
	uint16_t gpu_pipea_light_off_delay;
	uint16_t gpu_pipea_power_cycle_delay;
	uint32_t gpu_pipea_backlight_pwm;

	int gpu_pipeb_hotplug;
	int gpu_pipeb_port_select;	/* Port select: 1=DP_B 2=DP_C */
	uint16_t gpu_pipeb_power_on_delay;
	uint16_t gpu_pipeb_light_on_delay;
	uint16_t gpu_pipeb_power_off_delay;
	uint16_t gpu_pipeb_light_off_delay;
	uint16_t gpu_pipeb_power_cycle_delay;
	uint32_t gpu_pipeb_backlight_pwm;
};

extern struct chip_operations soc_intel_baytrail_ops;
#endif /* _BAYTRAIL_CHIP_H_ */
