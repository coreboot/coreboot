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
{R, 2, "", DVOB, 0x00300000, 0},
{R, 1, "", DVOC, 0x00300000, 0},
{R, 1, "", TV_CTL, 0x00000000, 0},
{R, 1, "", TV_DAC, 0x00000000, 0},
{W, 1, "", TV_DAC, 0x08000000, 0},
{R, 1, "", TV_DAC, 0x08000000, 0},
{W, 1, "", TV_DAC, 0x00000000, 0},
{R, 1, "", TV_DAC, 0x00000000, 0},
{W, 1, "", TV_DAC, 0x00000000, 0},
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
{R, 1, "", _PIPEACONF, 0x00000000, 0},
{R, 1, "", _PIPEBCONF, 0x00000000, 0},
{R, 1, "", LVDS, 0x40000000, 0},
{R, 1, "", _FDI_TXB_CTL, 0x00000000, 0},
{R, 1, "", TV_CTL, 0x00000000, 0},
{R, 1, "", LVDS, 0x40000000, 0},
{R, 1, "", _FDI_TXB_CTL, 0x00000000, 0},
{R, 1, "", TV_CTL, 0x00000000, 0},
{R, 1, "", _PIPEACONF, 0x00000000, 0},
{W, 1, "", _PIPEACONF, 0x00000000, 0},
{R, 1, "", _DSPACNTR, 0x00000000, 0},
{R, 1, "", _PIPEBCONF, 0x00000000, 0},
{W, 1, "", _PIPEBCONF, 0x00000000, 0},
{R, 1, "", _DSPBCNTR, 0x00000000, 0},
{R, 1, "", LVDS, 0x40000000, 0},
{R, 1, "", _FDI_TXB_CTL, 0x00000000, 0},
{R, 1, "", TV_CTL, 0x00000000, 0},
{R, 1, "", LVDS, 0x40000000, 0},
{R, 1, "", _FDI_TXB_CTL, 0x00000000, 0},
{R, 1, "", TV_CTL, 0x00000000, 0},
{R, 1, "", _PIPEACONF, 0x00000000, 0},
{R, 1, "", _PIPEBCONF, 0x00000000, 0},
{W, 1, "", PORT_HOTPLUG_EN, 0x00000000, 0},
{R, 1, "", PORT_HOTPLUG_STAT, 0x00000000, 0},
{W, 1, "", PORT_HOTPLUG_STAT, 0x00000000, 0},
{W, 1, "", HWSTAM, 0x0000effe, 0},
{W, 1, "", _PIPEASTAT, 0x00000000, 0},
{W, 1, "", _PIPEBSTAT, 0x00000000, 0},
{W, 1, "", IMR, 0xffffffff, 0},
{W, 1, "", IER, 0x00000000, 0},
{W, 1, "", EMR, 0xffffffed, 0},
{W, 1, "", IMR, 0xfffd73ae, 0},
{W, 1, "", IER, 0x00028053, 0},
{R, 1, "", PORT_HOTPLUG_EN, 0x00000000, 0},
{W, 1, "", PORT_HOTPLUG_EN,  CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000220, 0},
{R, 1, "", PORT_HOTPLUG_EN,  CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000220, 0},
{W, 1, "", PORT_HOTPLUG_EN,  CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_FORCE_DETECT | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000228, 0},
{R, 1, "", PORT_HOTPLUG_EN,  CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_FORCE_DETECT | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000228, 0},
{R, 1, "", PORT_HOTPLUG_EN,  CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000220, 0},
{R, 1, "", PORT_HOTPLUG_STAT, 0x00000000, 0},
{W, 1, "", PORT_HOTPLUG_STAT,  CRT_HOTPLUG_INT_STATUS | 0x00000800, 0},
{W, 1, "", PORT_HOTPLUG_EN,  CRT_HOTPLUG_INT_EN | CRT_HOTPLUG_VOLTAGE_COMPARE_50 | 0x00000220, 0},
{W, 1, "", 0x5100, 0x00000002, 0},
{W, 1, "", 0x5104, 0x460100a1, 0},
{R, 2, "", 0x5108, 0x00009c00, 0},
{W, 1, "", 0x5104, 0x80000000, 0},
{W, 1, "", 0x5104, 0x00000000, 0},
{W, 2, "", 0x5100, 0x00000000, 0},
{R, 1, "", _PIPEACONF, 0x00000000, 0},
{W, 1, "", _FPA0, 0x00020e08, 0},
{W, 1, "", _FPA1, 0x00020e08, 0},
{W, 1, "", _DPLL_A, 0x14020003, 0},
{W, 2, "", _DPLL_A, 0x94020003, 0},
{R, 1, "", _PIPEACONF, 0x00000000, 0},
{W, 1, "", _HTOTAL_A, 0x06af04ff, 0},
{W, 1, "", _HBLANK_A, 0x06af04ff, 0},
{W, 1, "", _HSYNC_A, 0x05d70557, 0},
{W, 1, "", _VTOTAL_A, 0x044f03ff, 0},
{W, 1, "", _VBLANK_A, 0x044f03ff, 0},
{W, 1, "", _VSYNC_A, 0x04090402, 0},
{W, 1, "", _PIPEASRC, 0x04ff03ff, 0},
{W, 1, "", _DSPBSIZE, 0x03ff04ff, 0},
{W, 1, "", _DSPBPOS, 0x00000000, 0},
{W, 1, "", _PIPEACONF, 0x00000000, 0},
{R, 1, "", _DPLL_A, 0x94020003, 0},
{R, 1, "", _PIPEACONF, 0x00000000, 0},
{W, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _PIPEASTAT,  PIPE_GMBUS_INTERRUPT_STATUS | PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000a03, 0},
{W, 1, "", _PIPEASTAT,  PIPE_GMBUS_INTERRUPT_STATUS | PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000a03, 0},
{R, 2, "", _PIPEASTAT, 0x00000000, 0},
{R, 2, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{W, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{R, 1, "", _PIPEASTAT, 0x00000000, 0},
{R, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{W, 1, "", _DSPBCNTR, 0x40000000, 0},
{R, 1, "", _DSPBCNTR, 0x40000000, 0},
{W, 1, "", _DSPBCNTR, 0x58000000, 0},
{W, 1, "", _DSPBSTRIDE, 0x00001400, 0},
{W, 1, "", _DSPBADDR, 0x00020000, 0},
{R, 2, "", DSPARB, (DSPARB_CSTART_SHIFT & 0x4) | 0x00001d9c, 0},
{W, 1, "", INSTPM+0x20,  FW_BLC_SELF_EN_MASK | 0x80000000, 0},
{W, 1, "", FW_BLC, 0x011d011a, 0},
{W, 1, "", FW_BLC2, 0x00000102, 0},
{R, 1, "", TV_CTL, 0x00000000, 0},
{W, 1, "", TV_H_CTL_1, 0x00400359, 0},
{W, 1, "", TV_H_CTL_2, 0x80480022, 0},
{W, 1, "", TV_H_CTL_3, 0x007c0344, 0},
{W, 1, "", TV_V_CTL_1, 0x00f01415, 0},
{W, 1, "", TV_V_CTL_2, 0x00060607, 0},
{W, 1, "", TV_V_CTL_3, 0x80120001, 0},
{W, 1, "", TV_V_CTL_4, 0x000900f0, 0},
{W, 1, "", TV_V_CTL_5, 0x000a00f0, 0},
{W, 1, "", TV_V_CTL_6, 0x000900f0, 0},
{W, 1, "", TV_V_CTL_7, 0x000a00f0, 0},
{W, 1, "", TV_SC_CTL_1, 0xc1710087, 0},
{W, 1, "", TV_SC_CTL_2, 0x6b405140, 0},
{W, 1, "", TV_SC_CTL_3, 0x00000000, 0},
{W, 1, "", TV_CSC_Y, 0x0332012d, 0},
{W, 1, "", TV_CSC_Y2, 0x07d30104, 0},
{W, 1, "", TV_CSC_U, 0x0733052d, 0},
{W, 1, "", TV_CSC_U2, 0x05c70200, 0},
{W, 1, "", TV_CSC_V, 0x0340030c, 0},
{W, 1, "", TV_CSC_V2, 0x06d00200, 0},
{W, 1, "", TV_CLR_KNOBS, 0x00606000, 0},
{W, 1, "", TV_CLR_LEVEL, 0x010b00e1, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _DSPBCNTR, 0x58000000, 0},
{W, 1, "", _DSPBCNTR, 0x58000000, 0},
{R, 1, "", _DSPBADDR, 0x00020000, 0},
{W, 1, "", _DSPBADDR, 0x00020000, 0},
{W, 1, "", _PIPEACONF, 0x00000000, 0},
{R, 1, "", _PIPEADSL, 0x0000029e, 0},
{R, 3, "", _PIPEADSL, 0x000003ff, 0},
{W, 1, "", _PFA_CTL_1, 0x00000000, 0},
{W, 1, "", _PFA_WIN_POS, 0x00000000, 0},
{W, 1, "", _PFA_WIN_SZ, 0x00000000, 0},
{W, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{W, 1, "", _DSPBCNTR, 0x58000000, 0},
{R, 1, "", _DSPBADDR, 0x00020000, 0},
{W, 1, "", _DSPBADDR, 0x00020000, 0},
{R, 1, "", TV_DAC, 0x70000000, 0},
{W, 1, "", TV_DAC, 0x00000000, 0},
{W, 1, "", TV_CTL, 0x000c0000, 0},
{R, 2, "", DSPARB, (DSPARB_CSTART_SHIFT & 0x4) | 0x00001d9c, 0},
{W, 1, "", INSTPM+0x20,  FW_BLC_SELF_EN_MASK | 0x80000000, 0},
{W, 1, "", INSTPM+0x20, 0x0001002f, 0},
{W, 1, "", FW_BLC, 0x0101011a, 0},
{W, 1, "", FW_BLC2, 0x00000102, 0},
{W, 1, "", INSTPM+0x20,  FW_BLC_SELF_EN_MASK | 0x80008000, 0},
{R, 1, "", PP_CONTROL, 0xabcd0000, 0},
{R, 1, "", LVDS, 0x40000000, 0},
{R, 1, "", _DPLL_A, 0x94020003, 0},
{W, 3, "", _DPLL_A, 0x94020003, 0},
{R, 1, "", _DPLL_A, 0x94020003, 0},
{R, 2, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _DSPBCNTR, 0x58000000, 0},
{W, 1, "", _DSPBCNTR, 0xd8000000, 0},
{R, 1, "", _DSPBADDR, 0x00020000, 0},
{W, 1, "", _DSPBADDR, 0x00020000, 0},
{R, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{W, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{R, 2, "", _PIPEASTAT, 0x00000000, 0},
{R, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{R, 1, "", TV_CTL, 0x000c0000, 0},
{W, 1, "", TV_CTL, 0x800c0000, 0},
{R, 1, "", LVDS, 0x40000000, 0},
{R, 1, "", _FDI_TXB_CTL, 0x00000000, 0},
{R, 2, "", TV_CTL, 0x800c0000, 0},
{R, 1, "", LVDS, 0x40000000, 0},
{R, 1, "", _FDI_TXB_CTL, 0x00000000, 0},
{R, 1, "", TV_CTL, 0x800c0000, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _PIPEBCONF, 0x00000000, 0},
{R, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{W, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{R, 2, "", _PIPEASTAT, 0x00000000, 0},
{R, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{R, 1, "", TV_DAC, 0x70000000, 0},
{R, 1, "", TV_CTL, 0x800c0000, 0},
{W, 1, "", TV_CTL, 0x000c0007, 0},
{W, 1, "", TV_DAC, 0x0f0000aa, 0},
{R, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{W, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{R, 2, "", _PIPEASTAT, 0x00000000, 0},
{R, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{R, 1, "", TV_DAC, 0x7f0000aa, 0},
{W, 1, "", TV_DAC, 0x70000000, 0},
{W, 1, "", TV_CTL, 0x800c0000, 0},
{R, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{W, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{R, 4, "", _PIPEASTAT, 0x00000000, 0},
{R, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{R, 1, "", TV_CTL, 0x800c0000, 0},
{W, 1, "", TV_CTL, 0x000c0000, 0},
{W, 1, "", INSTPM, 0x08000800, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _PIPEA_FRMCOUNT_GM45, 0x00000000, 0},
{R, 1, "", _PIPEA_FLIPCOUNT_GM45, 0x62029b1b, 0},
{R, 1, "", _PIPEA_FRMCOUNT_GM45, 0x00000000, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _VTOTAL_A, 0x044f03ff, 0},
{R, 1, "", _PIPEA_FLIPCOUNT_GM45, 0x6217572c, 0},
{R, 1, "", _HTOTAL_A, 0x06af04ff, 0},
{R, 1, "", _VBLANK_A, 0x044f03ff, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _VTOTAL_A, 0x044f03ff, 0},
{R, 1, "", _PIPEA_FLIPCOUNT_GM45, 0x63156af7, 0},
{R, 1, "", _HTOTAL_A, 0x06af04ff, 0},
{R, 1, "", _VBLANK_A, 0x044f03ff, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _VTOTAL_A, 0x044f03ff, 0},
{R, 1, "", _PIPEA_FLIPCOUNT_GM45, 0x64137fac, 0},
{R, 1, "", _HTOTAL_A, 0x06af04ff, 0},
{R, 1, "", _VBLANK_A, 0x044f03ff, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _PIPEA_FRMCOUNT_GM45, 0x00000000, 0},
{R, 1, "", _PIPEA_FLIPCOUNT_GM45, 0x6510b8c6, 0},
{R, 1, "", _PIPEA_FRMCOUNT_GM45, 0x00000000, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _PIPEA_FRMCOUNT_GM45, 0x00000000, 0},
{R, 1, "", _PIPEA_FLIPCOUNT_GM45, 0x6607c3b5, 0},
{R, 1, "", _PIPEA_FRMCOUNT_GM45, 0x00000000, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _VTOTAL_A, 0x044f03ff, 0},
{R, 1, "", _PIPEA_FLIPCOUNT_GM45, 0x661c804f, 0},
{R, 1, "", _HTOTAL_A, 0x06af04ff, 0},
{R, 1, "", _VBLANK_A, 0x044f03ff, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _VTOTAL_A, 0x044f03ff, 0},
{R, 1, "", _PIPEA_FLIPCOUNT_GM45, 0x671a949c, 0},
{R, 1, "", _HTOTAL_A, 0x06af04ff, 0},
{R, 1, "", _VBLANK_A, 0x044f03ff, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _VTOTAL_A, 0x044f03ff, 0},
{R, 1, "", _PIPEA_FLIPCOUNT_GM45, 0x6818a96d, 0},
{R, 1, "", _HTOTAL_A, 0x06af04ff, 0},
{R, 1, "", _VBLANK_A, 0x044f03ff, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _PIPEA_FRMCOUNT_GM45, 0x00000000, 0},
{R, 1, "", _PIPEA_FLIPCOUNT_GM45, 0x6915e1d1, 0},
{R, 1, "", _PIPEA_FRMCOUNT_GM45, 0x00000000, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _PIPEA_FRMCOUNT_GM45, 0x00000000, 0},
{R, 1, "", _PIPEA_FLIPCOUNT_GM45, 0x6a0cec77, 0},
{R, 1, "", _PIPEA_FRMCOUNT_GM45, 0x00000000, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _VTOTAL_A, 0x044f03ff, 0},
{R, 1, "", _PIPEA_FLIPCOUNT_GM45, 0x6b04d273, 0},
{R, 1, "", _HTOTAL_A, 0x06af04ff, 0},
{R, 1, "", _VBLANK_A, 0x044f03ff, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _VTOTAL_A, 0x044f03ff, 0},
{R, 1, "", _PIPEA_FLIPCOUNT_GM45, 0x6c02e710, 0},
{R, 1, "", _HTOTAL_A, 0x06af04ff, 0},
{R, 1, "", _VBLANK_A, 0x044f03ff, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _VTOTAL_A, 0x044f03ff, 0},
{R, 1, "", _PIPEA_FLIPCOUNT_GM45, 0x6d00fb0c, 0},
{R, 1, "", _HTOTAL_A, 0x06af04ff, 0},
{R, 1, "", _VBLANK_A, 0x044f03ff, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{R, 1, "", _PIPEA_FRMCOUNT_GM45, 0x00000000, 0},
{R, 1, "", _PIPEA_FLIPCOUNT_GM45, 0x6d1b0b04, 0},
{R, 1, "", _PIPEA_FRMCOUNT_GM45, 0x00000000, 0},
{R, 1, "", _DSPBCNTR, 0xd8000000, 0},
{W, 1, "", _DSPBCNTR, 0x58000000, 0},
{R, 1, "", _DSPBADDR, 0x00020000, 0},
{W, 1, "", _DSPBADDR, 0x00020000, 0},
{R, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{W, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{R, 2, "", _PIPEASTAT, 0x00000000, 0},
{R, 1, "", _PIPEASTAT,  PIPE_VSYNC_INTERRUPT_STATUS | PIPE_VBLANK_INTERRUPT_STATUS | PIPE_OVERLAY_UPDATED_STATUS | 0x00000203, 0},
{R, 1, "", _DSPACNTR, 0x00000000, 0},
{R, 1, "", _DSPBCNTR, 0x58000000, 0},
{R, 1, "", _PIPEACONF,  PIPECONF_ENABLE | 0x80000000, 0},
{W, 1, "", _PIPEACONF, 0x00000000, 0},
{R, 2, "", _PIPEADSL, 0x000003ff, 0},
{R, 1, "", _PIPEACONF, 0x00000000, 0},
{R, 1, "", _DPLL_A, 0x94020003, 0},
{W, 1, "", _DPLL_A, 0x14020003, 0},
{R, 2, "", DSPARB, (DSPARB_CSTART_SHIFT & 0x4) | 0x00001d9c, 0},
{W, 1, "", INSTPM+0x20,  FW_BLC_SELF_EN_MASK | 0x80000000, 0},
{W, 1, "", FW_BLC, 0x011d011a, 0},
{W, 1, "", FW_BLC2, 0x00000102, 0},
{R, 1, "", _DSPBCNTR, 0x58000000, 0},
{R, 1, "", _PIPEACONF, 0x00000000, 0},
{R, 1, "", LVDS, 0x40000000, 0},
{R, 1, "", _FDI_TXB_CTL, 0x00000000, 0},
{R, 1, "", TV_CTL, 0x000c0000, 0},
{R, 1, "", LVDS, 0x40000000, 0},
{R, 1, "", _FDI_TXB_CTL, 0x00000000, 0},
{R, 1, "", TV_CTL, 0x000c0000, 0},
{R, 1, "", _PIPEACONF, 0x00000000, 0},
{R, 1, "", _PIPEBCONF, 0x00000000, 0},
{M, 1, "fbcon:inteldrmfb (fb0) is primary device", 0x0, 0x0, 0},
{R, 1, "", _PIPEBCONF, 0x00000000, 0},
{R, 2, "", LVDS, 0x40000000, 0},
{W, 1, "", _FPB0, 0x00020e09, 0},
{W, 1, "", _FPB1,  FP_M1_DIV_SHIFT | DPLLA_INPUT_BUFFER_ENABLE | VF_UNIT_CLOCK_GATE_DISABLE | 0x00020e09, 0},
{W, 1, "", _DPLL_B,  DPLL_VGA_MODE_DIS | DPLLB_MODE_LVDS | 0x18046000, 0},
{R, 1, "", LVDS, 0x40000000, 0},
{W, 1, "", LVDS,  LVDS_ON | PLL_P1_DIVIDE_BY_TWO | DISPLAY_RATE_SELECT_FPA1 | 0xc0300300, 0},
{W, 2, "", _DPLL_B,  DPLL_VCO_ENABLE | DPLL_VGA_MODE_DIS | DPLLB_MODE_LVDS | 0x98046000, 0},
{R, 1, "", _PIPEBCONF, 0x00000000, 0},
{W, 1, "", _HTOTAL_B, 0x053f03ff, 0},
{W, 1, "", _HBLANK_B, 0x053f03ff, 0},
{W, 1, "", _HSYNC_B, 0x049f0417, 0},
{W, 1, "", _VTOTAL_B, 0x032502ff, 0},
{W, 1, "", _VBLANK_B, 0x032502ff, 0},
{W, 1, "", _VSYNC_B, 0x03080302, 0},
{W, 1, "", _PIPEBSRC, 0x03ff02ff, 0},
{W, 1, "", _DSPASIZE, 0x02ff03ff, 0},
{W, 1, "", _DSPASTRIDE+0x4, 0x00000000, 0},
{W, 1, "", _PIPEBCONF, 0x00000000, 0},
{R, 1, "", _DPLL_B,  DPLL_VCO_ENABLE | DPLL_VGA_MODE_DIS | DPLLB_MODE_LVDS | 0x98046000, 0},
{R, 1, "", _PIPEBCONF, 0x00000000, 0},
{W, 1, "", _PIPEBCONF, 0x80000000, 0},
{R, 1, "", _PIPEBSTAT, 0x00000040, 0},
{W, 1, "", _PIPEBSTAT, 0x00000042, 0},
{R, 1, "", _PIPEBSTAT, 0x00000000, 0},
{R, 2, "", _PIPEBSTAT, 0x00000202, 0},
{W, 1, "", _PIPEBSTAT, 0x00000202, 0},
{R, 3, "", _PIPEBSTAT, 0x00000000, 0},
{R, 1, "", _PIPEBSTAT, 0x00000202, 0},
{W, 1, "", _DSPACNTR, 0x41000000, 0},
{R, 1, "", _DSPACNTR, 0x41000000, 0},
{W, 1, "", _DSPACNTR, 0x59000000, 0},
{W, 1, "", _DSPASTRIDE, 0x00001000, 0},
{W, 1, "", _DSPAADDR, 0x00020000, 0},
{R, 2, "", DSPARB, (DSPARB_CSTART_SHIFT & 0x4) | 0x00001d9c, 0},
{W, 1, "", INSTPM+0x20,  FW_BLC_SELF_EN_MASK | 0x80000000, 0},
{W, 1, "", FW_BLC, 0x011d011a, 0},
{W, 1, "", FW_BLC2, 0x00000102, 0},
{R, 2, "", DSPARB, (DSPARB_CSTART_SHIFT & 0x4) | 0x00001d9c, 0},
{W, 1, "", INSTPM+0x20,  FW_BLC_SELF_EN_MASK | 0x80000000, 0},
{W, 1, "", INSTPM+0x20, 0x0001003f, 0},
{W, 1, "", FW_BLC, 0x011d0109, 0},
{W, 1, "", FW_BLC2, 0x00000102, 0},
{W, 1, "", INSTPM+0x20,  FW_BLC_SELF_EN_MASK | 0x80008000, 0},
{R, 1, "", PP_CONTROL, 0xabcd0000, 0},
{R, 1, "", LVDS,  LVDS_ON | PLL_P1_DIVIDE_BY_TWO | DISPLAY_RATE_SELECT_FPA1 | 0xc0300300, 0},
{R, 1, "", _DPLL_B,  DPLL_VCO_ENABLE | DPLL_VGA_MODE_DIS | DPLLB_MODE_LVDS | 0x98046000, 0},
{W, 3, "", _DPLL_B,  DPLL_VCO_ENABLE | DPLL_VGA_MODE_DIS | DPLLB_MODE_LVDS | 0x98046000, 0},
{R, 1, "", _DPLL_B,  DPLL_VCO_ENABLE | DPLL_VGA_MODE_DIS | DPLLB_MODE_LVDS | 0x98046000, 0},
{R, 2, "", _PIPEBCONF, 0x80000000, 0},
{R, 1, "", _DSPACNTR,  0x59000000, 0},
{W, 1, "", _DSPACNTR,  0xd9000000, 0},
{R, 1, "", _DSPAADDR, 0x00020000, 0},
{W, 1, "", _DSPAADDR, 0x00020000, 0},
{R, 1, "", _PIPEBSTAT, 0x00000202, 0},
{W, 1, "", _PIPEBSTAT, 0x00000202, 0},
{R, 3, "", _PIPEBSTAT, 0x00000000, 0},
{R, 1, "", _PIPEBSTAT, 0x00000202, 0},
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
