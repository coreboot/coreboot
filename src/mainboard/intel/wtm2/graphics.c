/*
* This file is part of the coreboot project.
*
* Copyright 2013 Google Inc.
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
/* this file was for the most part machine generated, and in future
 * will be all machine generated. Avoid editing.
 */
#include <console/console.h>
#include <device/i915.h>

void graphics_register_reset(u32 aux_ctl, u32 aux_data, int verbose)
{

	io_i915_write32(0x80000000,0x45400);
	io_i915_write32(0x00000000,_CURACNTR);
	io_i915_write32((/* PIPEA */0x0<<24)|0x00000000,_DSPACNTR);
	io_i915_write32(0x00000000,_DSPBCNTR);
	io_i915_write32(0x80000000,CPU_VGACNTRL);
	io_i915_write32(0x00000000,_DSPASIZE+0xc);
	io_i915_write32(0x00000000,_DSPBSURF);
	io_i915_write32(0x00000000,0x4f050);
	io_i915_write32( DP_LINK_TRAIN_PAT_1 | DP_LINK_TRAIN_PAT_1_CPT |
		DP_VOLTAGE_0_4 | DP_PRE_EMPHASIS_0 | DP_PORT_WIDTH_1 |
		DP_PLL_FREQ_270MHZ | DP_SCRAMBLING_DISABLE_IRONLAKE |
		DP_SYNC_VS_HIGH |0x00000091,DP_A);
	io_i915_write32(0x00200090,_FDI_RXA_MISC);
	io_i915_write32(0x0a000000,_FDI_RXA_MISC);
	/* not yet documented anywhere that we can find. */
	io_i915_write32(0x00000070,0x46408);
	io_i915_write32(0x04000000,0x42090);
	io_i915_write32(0x40000000,0x4f050);
	io_i915_write32(0x00000000,0x9840);
	io_i915_write32(0xa4000000,0x42090);
	io_i915_write32(0x00004000,0x42080);
	io_i915_write32(0x00ffffff,0x64f80);
	io_i915_write32(0x0007000e,0x64f84);
	io_i915_write32(0x00d75fff,0x64f88);
	io_i915_write32(0x000f000a,0x64f8c);
	io_i915_write32(0x00c30fff,0x64f90);
	io_i915_write32(0x00060006,0x64f94);
	io_i915_write32(0x00aaafff,0x64f98);
	io_i915_write32(0x001e0000,0x64f9c);
	io_i915_write32(0x00ffffff,0x64fa0);
	io_i915_write32(0x000f000a,0x64fa4);
	io_i915_write32(0x00d75fff,0x64fa8);
	io_i915_write32(0x00160004,0x64fac);
	io_i915_write32(0x00c30fff,0x64fb0);
	io_i915_write32(0x001e0000,0x64fb4);
	io_i915_write32(0x00ffffff,0x64fb8);
	io_i915_write32(0x00060006,0x64fbc);
	io_i915_write32(0x00d75fff,0x64fc0);
	io_i915_write32(0x001e0000,0x64fc4);
	io_i915_write32(0x00ffffff,0x64e00);
	io_i915_write32(0x0006000e,0x64e04);
	io_i915_write32(0x00d75fff,0x64e08);
	io_i915_write32(0x0005000a,0x64e0c);
	io_i915_write32(0x00c30fff,0x64e10);
	io_i915_write32(0x00040006,0x64e14);
	io_i915_write32(0x80aaafff,0x64e18);
	io_i915_write32(0x000b0000,0x64e1c);
	io_i915_write32(0x00ffffff,0x64e20);
	io_i915_write32(0x0005000a,0x64e24);
	io_i915_write32(0x00d75fff,0x64e28);
	io_i915_write32(0x000c0004,0x64e2c);
	io_i915_write32(0x80c30fff,0x64e30);
	io_i915_write32(0x000b0000,0x64e34);
	io_i915_write32(0x00ffffff,0x64e38);
	io_i915_write32(0x00040006,0x64e3c);
	io_i915_write32(0x80d75fff,0x64e40);
	io_i915_write32(0x000b0000,0x64e44);
	/* end not yet documented. */
	io_i915_write32(0x10000000,SDEISR+0x30);
}


