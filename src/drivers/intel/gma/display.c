/*
 * Copyright 2013 Google Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors:
 *    Keith Packard <keithp@keithp.com>
 *
 */

/* This code was created by the coccinnelle filters in the i915tool,
 * with some final hand filtering.
 */

#include <console/console.h>
#include <stdint.h>
#include <delay.h>
#include <drivers/intel/gma/i915.h>
#include <string.h>

void compute_display_params(struct intel_dp *dp)
{
	struct edid *edid = &(dp->edid);
	struct edid_mode *mode = &edid->mode;

	/* step 1: get the constants in the dp struct set up. */
	dp->lane_count = dp->dpcd[DP_MAX_LANE_COUNT]&DP_LANE_COUNT_MASK;

	dp->link_bw = dp->dpcd[DP_MAX_LINK_RATE];
	dp->clock = intel_dp_bw_code_to_link_rate(dp->link_bw);
	dp->edid.link_clock = intel_dp_bw_code_to_link_rate(dp->link_bw);

	/* step 2. Do some computation of other stuff. */
	dp->bytes_per_pixel = dp->pipe_bits_per_pixel/8;

	dp->stride = edid->bytes_per_line;

	dp->htotal = (mode->ha - 1) | ((mode->ha + mode->hbl - 1) << 16);

	dp->hblank = (mode->ha - 1) | ((mode->ha + mode->hbl - 1) << 16);

	dp->hsync = (mode->ha + mode->hso - 1) |
		((mode->ha + mode->hso + mode->hspw - 1) << 16);

	dp->vtotal = (mode->va - 1) | ((mode->va + mode->vbl - 1) << 16);

	dp->vblank = (mode->va - 1) | ((mode->va + mode->vbl - 1) << 16);

	dp->vsync = (mode->va + mode->vso - 1) |
		((mode->va + mode->vso + mode->vspw - 1) << 16);

	/* PIPEASRC is wid-1 x ht-1 */
	dp->pipesrc = (mode->ha-1)<<16 | (mode->va-1);

	dp->pfa_pos = 0;

	dp->pfa_ctl = PF_ENABLE | PF_FILTER_MED_3x3;
	/* IVB hack */
	if (dp->gen == 6)
		dp->pfa_ctl |= PF_PIPE_SEL_IVB(dp->pipe);

	dp->pfa_sz = (mode->ha << 16) | (mode->va);

	/* step 3. Call the linux code we pulled in. */
	dp->flags = intel_ddi_calc_transcoder_flags(edid->panel_bits_per_pixel,
						    dp->port,
						    dp->pipe,
						    dp->type,
						    dp->lane_count,
						    dp->pfa_sz,
						    mode->phsync == '+'?1:0,
						    mode->pvsync == '+'?1:0);

	dp->transcoder = intel_ddi_get_transcoder(dp->port,
						  dp->pipe);

	intel_dp_compute_m_n(edid->panel_bits_per_pixel,
			     dp->lane_count,
			     dp->edid.mode.pixel_clock,
			     dp->edid.link_clock,
			     &dp->m_n);

	printk(BIOS_SPEW, "dp->lane_count  = 0x%08x\n",dp->lane_count);
	printk(BIOS_SPEW, "dp->stride  = 0x%08x\n",dp->stride);
	printk(BIOS_SPEW, "dp->htotal  = 0x%08x\n", dp->htotal);
	printk(BIOS_SPEW, "dp->hblank  = 0x%08x\n", dp->hblank);
	printk(BIOS_SPEW, "dp->hsync   = 0x%08x\n", dp->hsync);
	printk(BIOS_SPEW, "dp->vtotal  = 0x%08x\n", dp->vtotal);
	printk(BIOS_SPEW, "dp->vblank  = 0x%08x\n", dp->vblank);
	printk(BIOS_SPEW, "dp->vsync   = 0x%08x\n", dp->vsync);
	printk(BIOS_SPEW, "dp->pipesrc = 0x%08x\n", dp->pipesrc);
	printk(BIOS_SPEW, "dp->pfa_pos = 0x%08x\n", dp->pfa_pos);
	printk(BIOS_SPEW, "dp->pfa_ctl = 0x%08x\n", dp->pfa_ctl);
	printk(BIOS_SPEW, "dp->pfa_sz  = 0x%08x\n", dp->pfa_sz);
	printk(BIOS_SPEW, "dp->link_m  = 0x%08x\n", dp->m_n.link_m);
	printk(BIOS_SPEW, "dp->link_n  = 0x%08x\n", dp->m_n.link_n);
	printk(BIOS_SPEW, "0x6f030     = 0x%08x\n",
	       TU_SIZE(dp->m_n.tu) | dp->m_n.gmch_m);
	printk(BIOS_SPEW, "0x6f030     = 0x%08x\n", dp->m_n.gmch_m);
	printk(BIOS_SPEW, "0x6f034     = 0x%08x\n", dp->m_n.gmch_n);
	printk(BIOS_SPEW, "dp->flags   = 0x%08x\n", dp->flags);
}
