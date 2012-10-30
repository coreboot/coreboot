/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
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

/*
 * Digital Port Hotplug Enable:
 *  0x04 = Enabled, 2ms short pulse
 *  0x05 = Enabled, 4.5ms short pulse
 *  0x06 = Enabled, 6ms short pulse
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

	u32 gpu_cpu_backlight;	/* CPU Backlight PWM value */
	u32 gpu_pch_backlight;	/* PCH Backlight PWM value */
};

extern struct chip_operations northbridge_intel_haswell_ops;
