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

#include <northbridge/intel/haswell/i915_reg.h>
#include <northbridge/intel/haswell/drm_dp_helper.h>

/* things that are, strangely, not defined anywhere? */
#define PCH_PP_UNLOCK	0xabcd0000
#define WMx_LP_SR_EN	(1<<31)
#define PRB0_TAIL	0x02030
#define PRB0_HEAD	0x02034
#define PRB0_START	0x02038
#define PRB0_CTL	0x0203c


/* mainboard-specific defines */
/* how many bytes do we need for the framebuffer?
 * Well, this gets messy. To get an exact answer, we have
 * to ask the panel, but we'd rather zero the memory
 * and set up the gtt while the panel powers up. So,
 * we take a reasonable guess, secure in the knowledge that the
 * MRC has to overestimate the number of bytes used.
 * 8 MiB is a very safe guess. There may be a better way later, but
 * fact is, the initial framebuffer is only very temporary. And taking
 * a little long is ok; this is done much faster than the AUX
 * channel is ready for IO.
 */
#define FRAME_BUFFER_BYTES (8*MiB)
/* how many 4096-byte pages do we need for the framebuffer?
 * There are hard ways to get this, and easy ways:
 * there are FRAME_BUFFER_BYTES/4096 pages, since pages are 4096
 * on this chip.
 */
#define FRAME_BUFFER_PAGES (FRAME_BUFFER_BYTES/(4096))


/* debug enums. These are for printks that, due to their place in the
 * middle of graphics device IO, might change timing.  Use with care
 * or not at all.
 */
enum {
	vio = 2, /* dump every IO */
	vspin = 4, /* print # of times we spun on a register value */
};

/* i915.c */
unsigned long io_i915_READ32(unsigned long addr);
void io_i915_WRITE32(unsigned long val, unsigned long addr);
int vbe_mode_info_valid(void);
void fill_lb_framebuffer(struct lb_framebuffer *framebuffer);

/* intel_dp.c */
u32 pack_aux(u32 *src, int src_bytes);
void unpack_aux(u32 src, u8 *dst, int dst_bytes);
int intel_dp_aux_ch(u32 ch_ctl, u32 ch_data, u32 *send, int send_bytes,
	u8 *recv, int recv_size);

/* i915io.c */
void graphics_register_reset(u32 aux_ctl, u32 aux_data, int verbose);
