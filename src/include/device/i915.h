/*
 * This file is part of the coreboot project.
 *
 * Copyright 2012 Google Inc.
 *
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

#include <device/i915_reg.h>
#include <device/drm_dp_helper.h>

/* things that are, strangely, not defined anywhere? */
#define PCH_PP_UNLOCK	0xabcd0000
#define WMx_LP_SR_EN	(1<<31)
#define PRB0_TAIL	0x02030
#define PRB0_HEAD	0x02034
#define PRB0_START	0x02038
#define PRB0_CTL	0x0203c

/* debug enums. These are for printks that, due to their place in the
 * middle of graphics device IO, might change timing.  Use with care
 * or not at all.
 */
enum {
	vio = 2, /* dump every IO */
	vspin = 4, /* print # of times we spun on a register value */
};

/* The mainboard must provide these functions. */
unsigned long io_i915_read32(unsigned long addr);
void io_i915_write32(unsigned long val, unsigned long addr);
void graphics_register_reset(u32 aux_ctl, u32 aux_data, int verbose);

/* intel_dp.c */
u32 pack_aux(u32 *src, int src_bytes);
void unpack_aux(u32 src, u8 *dst, int dst_bytes);
int intel_dp_aux_ch(u32 ch_ctl, u32 ch_data, u32 *send, int send_bytes,
	u8 *recv, int recv_size);


