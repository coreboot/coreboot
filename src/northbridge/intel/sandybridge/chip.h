/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NORTHBRIDGE_INTEL_SANDYBRIDGE_CHIP_H
#define NORTHBRIDGE_INTEL_SANDYBRIDGE_CHIP_H

#include <drivers/intel/gma/i915.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * Digital Port Hotplug Enable:
 *  0x04 = Enabled, 2ms   short pulse
 *  0x05 = Enabled, 4.5ms short pulse
 *  0x06 = Enabled, 6ms   short pulse
 *  0x07 = Enabled, 100ms short pulse
 */
struct northbridge_intel_sandybridge_config {
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

	/*
	 * Maximum memory clock.
	 * For example 666 for DDR3-1333, or 800 for DDR3-1600
	 */
	u16 max_mem_clock_mhz;

	struct i915_gpu_controller_info gfx;

	/* Data for RAM init */

	/* DIMM SPD address. Use 8bit notation where BIT0 is always zero. */
	u8 spd_addresses[4];

	/* PEI data for RAM init and early silicon init */
	u8 ts_addresses[4];

	bool ec_present;
	bool ddr3lv_support;

	/*
	 * N mode functionality. Leave this setting at 0.
	 * 0 Auto
	 * 1 1N
	 * 2 2N
	 */
	enum {
		DDR_NMODE_AUTO = 0,
		DDR_NMODE_1N,
		DDR_NMODE_2N,
	} nmode;

	/*
	 * DDR refresh rate config. JEDEC Standard No.21-C Annex K allows for DIMM SPD data to
	 * specify whether double-rate is required for extended operating temperature range.
	 *
	 *   0 Enable double rate based upon temperature thresholds
	 *   1 Normal rate
	 *   2 Always enable double rate
	 */
	enum {
		DDR_REFRESH_RATE_TEMP_THRES = 0,
		DDR_REFRESH_REATE_NORMAL,
		DDR_REFRESH_RATE_DOUBLE,
	} ddr_refresh_rate_config;

	/*
	 * USB Port Configuration:
	 *  [0] = enable
	 *  [1] = overcurrent pin
	 *  [2] = length
	 *
	 * Ports 0-7  can be mapped to OC0-OC3
	 * Ports 8-13 can be mapped to OC4-OC7
	 *
	 * Port Length
	 *  MOBILE:
	 *   < 0x050 = Setting 1 (back panel, 1-5in, lowest tx amplitude)
	 *   < 0x140 = Setting 2 (back panel, 5-14in, highest tx amplitude)
	 *  DESKTOP:
	 *   < 0x080 = Setting 1 (front/back panel, <8in, lowest tx amplitude)
	 *   < 0x130 = Setting 2 (back panel, 8-13in, higher tx amplitude)
	 *   < 0x150 = Setting 3 (back panel, 13-15in, highest tx amplitude)
	 */
	u16 usb_port_config[16][3];

	struct {
		/* 0: Disable, 1: Enable, 2: Auto, 3: Smart Auto */
		u8 mode			: 2;
		/*  4 bit mask, 1: switchable, 0: not switchable */
		u8 hs_port_switch_mask	: 4;
		/* 0: No xHCI preOS driver, 1: xHCI preOS driver */
		u8 preboot_support	: 1;
		/* 0: Disable, 1: Enable */
		u8 xhci_streams		: 1;
	} usb3;
};

#endif /* NORTHBRIDGE_INTEL_SANDYBRIDGE_CHIP_H */
