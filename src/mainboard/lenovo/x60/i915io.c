/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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

#include <stdint.h>
#include "i915io.h"

struct iodef iodefs[] = {
{V, 0,},
{M, 1, "Linux agpgart interface v0.103", 0x0, 0x0, 0},
{M, 1, "agpgart-intel 0000:00:00.0:Intel 945GM Chipset", 0x0, 0x0, 0},
{M, 1, "agpgart-intel 0000:00:00.0:detected gtt size:262144K total, 262144K mappable", 0x0, 0x0, 0},
{M, 1, "agpgart-intel 0000:00:00.0:detected 8192K stolen memory", 0x0, 0x0, 0},
{M, 1, "agpgart-intel 0000:00:00.0:AGP aperture is 256M @ 0xd0000000", 0x0, 0x0, 0},
{M, 1, "[drm] Initialized drm 1.1.0 20060810", 0x0, 0x0, 0},
{M, 1, "[drm] radeon defaulting to kernel modesetting.", 0x0, 0x0, 0},
{M, 1, "[drm] radeon kernel modesetting enabled.", 0x0, 0x0, 0},
{M, 1, "i915 0000:00:02.0:setting latency timer to 64", 0x0, 0x0, 0},
{W, 1, "", 0x5100, 0x00000000, 0},
{R, 1, "", PP_ON_DELAYS, 0x00000000, 0},
{R, 1, "", PP_OFF_DELAYS, 0x00000000, 0},
{W, 1, "", PP_ON_DELAYS, 0x00000000, 0},
{W, 1, "", PP_OFF_DELAYS, 0x00000000, 0},
{W, 1, "", INSTPM+0x24,  MI_ARB_C3_LP_WRITE_ENABLE | 0x08000800, 0},
{W, 1, "", RENDER_RING_BASE, 0x00000000, 0},
{W, 1, "", RENDER_RING_BASE+0x4, 0x00000000, 0},
{W, 1, "", RENDER_RING_BASE+0x8, 0x00000000, 0},
{W, 1, "", RENDER_RING_BASE+0xc, 0x00000000, 0},
{W, 1, "", RENDER_RING_BASE+0x10, 0x00000000, 0},
{W, 1, "", RENDER_RING_BASE+0x14, 0x00000000, 0},
{W, 1, "", RENDER_RING_BASE+0x18, 0x00000000, 0},
{W, 1, "", RENDER_RING_BASE+0x1c, 0x00000000, 0},
{W, 1, "", FENCE_REG_965_0, 0x00000000, 0},
{W, 1, "", FENCE_REG_965_0+0x4, 0x00000000, 0},
{W, 1, "", FENCE_REG_965_0+0x8, 0x00000000, 0},
{W, 1, "", FENCE_REG_965_0+0xc, 0x00000000, 0},
{W, 1, "", FENCE_REG_965_0+0x10, 0x00000000, 0},
{W, 1, "", FENCE_REG_965_0+0x14, 0x00000000, 0},
{W, 1, "", FENCE_REG_965_0+0x18, 0x00000000, 0},
{W, 1, "", FENCE_REG_965_0+0x1c, 0x00000000, 0},
{R, 1, "", DCC,  DCC_ADDRESSING_MODE_DUAL_CHANNEL_INTERLEAVED | DCC_CHANNEL_XOR_BIT_17 | 0x000f0202, 0},
{M, 1, "[drm] Supports vblank timestamp caching Rev 1 (10.10.2010).", 0x0, 0x0, 0},
{M, 1, "[drm] Driver supports precise vblank timestamp query.", 0x0, 0x0, 0},
{M, 1, "i915 0000:00:02.0:Invalid ROM contents", 0x0, 0x0, 0},
{M, 1, "[drm] failed to find VBIOS tables", 0x0, 0x0, 0},
{M, 1, "vgaarb:device changed decodes:PCI:0000:00:02.0,olddecodes=io+mem,decodes=io+mem:owns=io+mem", 0x0, 0x0, 0},
{W, 1, "", VGACNTRL,  VGA_DISP_DISABLE | 0x80000000, 0},
{R, 1, "", PFIT_CONTROL, 0x00000000, 0},
{R, 1, "", PP_CONTROL, 0x00000000, 0},
{W, 1, "", PP_CONTROL, 0xabcd0000, 0},
{R, 2, "", BLC_PWM_CTL, 0x00000000, 0},
{M, 1, "tsc:Refined TSC clocksource calibration:1662.499 MHz", 0x0, 0x0, 0},
{W, 1, "", HWS_PGA, 0x352d2000, 0},
{W, 1, "", PRB0_CTL, 0x00000000, 0},
{M, 1, "Switching to clocksource tsc", 0x0, 0x0, 0},
{W, 1, "", PRB0_HEAD, 0x00000000, 0},
{W, 1, "", PRB0_TAIL, 0x00000000, 0},
{R, 1, "", PRB0_HEAD, 0x00000000, 0},
{W, 1, "", PRB0_START, 0x00000000, 0},
{W, 1, "", PRB0_CTL, 0x0001f001, 0},
{R, 1, "", PRB0_CTL, 0x0001f001, 0},
{R, 1, "", PRB0_START, 0x00000000, 0},
{R, 2, "", PRB0_HEAD, 0x00000000, 0},
{R, 1, "", PRB0_TAIL, 0x00000000, 0},
{R, 1, "", 0x6104, 0x00000000, 0},
{W, 1, "", 0x6104, 0x0000000b, 0},
{W, 1, "", ECOSKPD, 0x00010000, 0},
{M, 1, "[drm] initialized overlay support", 0x0, 0x0, 0},
{W, 1, "", PORT_HOTPLUG_EN, 0x00000000, 0},
{R, 1, "", PORT_HOTPLUG_STAT, 0x00000000, 0},
{W, 1, "", PORT_HOTPLUG_STAT, 0x00000000, 0},
{W, 1, "", HWSTAM, 0x0000effe, 0},
{R, 1, "", PORT_HOTPLUG_EN, 0x00000000, 0},
{W, 1, "", PORT_HOTPLUG_EN,  CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000220, 0},
{R, 1, "", PORT_HOTPLUG_EN,  CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000220, 0},
{W, 1, "", PORT_HOTPLUG_EN,  CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_FORCE_DETECT | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000228, 0},
{R, 1, "", PORT_HOTPLUG_EN,  CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_FORCE_DETECT | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000228, 0},
{R, 1, "", PORT_HOTPLUG_EN,  CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000220, 0},
{R, 1, "", PORT_HOTPLUG_STAT, 0x00000000, 0},
{W, 1, "", PORT_HOTPLUG_STAT,  CRT_HOTPLUG_INT_STATUS | 0x00000800, 0},
{W, 1, "", PORT_HOTPLUG_EN,  CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000220, 0},
{W, 1, "", FW_BLC_SELF,  FW_BLC_SELF_EN_MASK | 0x80000000, 0},
{W, 1, "", FW_BLC, 0x011d011a, 0},
{W, 1, "", FW_BLC2, 0x00000102, 0},
{W, 1, "", FW_BLC_SELF,  FW_BLC_SELF_EN_MASK | 0x80000000, 0},
{W, 1, "", FW_BLC_SELF, 0x0001002f, 0},
{W, 1, "", FW_BLC, 0x0101011a, 0},
{W, 1, "", FW_BLC2, 0x00000102, 0},
{W, 1, "", FW_BLC_SELF,  FW_BLC_SELF_EN_MASK | 0x80008000, 0},

{W, 1, "", INSTPM, 0x08000800, 0},
{W, 1, "", FW_BLC_SELF,  FW_BLC_SELF_EN_MASK | 0x80000000, 0},
{W, 1, "", FW_BLC, 0x011d011a, 0},
{W, 1, "", FW_BLC2, 0x00000102, 0},
{M, 1, "fbcon:inteldrmfb (fb0) is primary device", 0x0, 0x0, 0},
{W, 1, "", FW_BLC_SELF,  FW_BLC_SELF_EN_MASK | 0x80000000, 0},
{W, 1, "", FW_BLC, 0x011d011a, 0},
{W, 1, "", FW_BLC2, 0x00000102, 0},
{W, 1, "", FW_BLC_SELF,  FW_BLC_SELF_EN_MASK | 0x80000000, 0},
{W, 1, "", FW_BLC_SELF, 0x0001003f, 0},
{W, 1, "", FW_BLC, 0x011d0109, 0},
{W, 1, "", FW_BLC2, 0x00000102, 0},
{W, 1, "", FW_BLC_SELF,  FW_BLC_SELF_EN_MASK | 0x80008000, 0},
{W, 1, "", PFIT_PGM_RATIOS, 0x00000000, 0},
{W, 1, "", PFIT_CONTROL, (PFIT_PIPE_SHIFT & 0x8) | 0x00000008, 0},
{M, 1, "i915:fixme:max PWM is zero", 0x0, 0x0, 0},
{R, 1, "", BLC_PWM_CTL, 0x00000000, 0},
{W, 1, "", BLC_PWM_CTL, 0x00000002, 0},
{M, 1, "Console:switching to colour frame buffer device 128x48", 0x0, 0x0, 0},
{M, 1, "i915 0000:00:02.0:fb0:inteldrmfb frame buffer device", 0x0, 0x0, 0},
{M, 1, "i915 0000:00:02.0:registered panic notifier", 0x0, 0x0, 0},
{M, 1, "[drm] Initialized i915 1.6.0 20080730 for 0000:00:02.0 on minor 0", 0x0, 0x0, 0},
{0,},
};

int niodefs = sizeof(iodefs) / sizeof(iodefs[0]);
