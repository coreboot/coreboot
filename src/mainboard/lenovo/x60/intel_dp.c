/*
 * Copyright 2013 Google Inc.
 * Copyright © 2008 Intel Corporation
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

#include <console/console.h>
#include <stdint.h>
#include <delay.h>
#include "i915io.h"

u32
pack_aux(u32 *src32, int src_bytes)
{
	u8 *src = (u8 *)src32;
	int	i;
	u32 v = 0;

	if (src_bytes > 4)
		src_bytes = 4;
	for (i = 0; i < src_bytes; i++)
		v |= ((u32) src[i]) << ((3-i) * 8);
	return v;
}

void
unpack_aux(u32 src, u32 *dst32, int dst_bytes)
{
	u8 *dst = (u8 *)dst32;

	int i;
	if (dst_bytes > 4)
		dst_bytes = 4;
	for (i = 0; i < dst_bytes; i++)
		dst[i] = src >> ((3-i) * 8);
}

int
intel_dp_aux_ch(u32 ch_ctl, u32 ch_data, u32 *send, int send_bytes,
		u32 *recv, int recv_size)
{
	int i;
	int recv_bytes;
	u32 status;
	u32 aux_clock_divider;
	int try, precharge = 5;

	/* The clock divider is based off the hrawclk,
	 * and would like to run at 2MHz. So, take the
	 * hrawclk value and divide by 2 and use that
	 *
	 * Note that PCH attached eDP panels should use a 125MHz input
	 * clock divider.
	 */
	/* 200 on link */
	aux_clock_divider = 200; /* SNB & IVB eDP input clock at 400Mhz */

	/* Try to wait for any previous AUX channel activity */
	for (try = 0; try < 3; try++) {
		status = io_i915_READ32(ch_ctl);
		if ((status & DP_AUX_CH_CTL_SEND_BUSY) == 0)
			break;
		udelay(1000);
	}

	if (try == 3) {
	  printk(BIOS_SPEW, "[000000.0] [drm:%s], ", __func__);
	  printk(BIOS_SPEW, "dp_aux_ch not started status 0x%08lx\n",
		  io_i915_READ32(ch_ctl));
	  return -1;
	}

	/* Must try at least 3 times according to DP spec */
	for (try = 0; try < 5; try++) {
		/* Load the send data into the aux channel data registers */
		for (i = 0; i < send_bytes; i += 4)
			io_i915_WRITE32(send[i], ch_data + i);

		/* Send the command and wait for it to complete */
		io_i915_WRITE32(
			   DP_AUX_CH_CTL_SEND_BUSY |
			   DP_AUX_CH_CTL_TIME_OUT_400us |
			   (send_bytes << DP_AUX_CH_CTL_MESSAGE_SIZE_SHIFT) |
			   (precharge << DP_AUX_CH_CTL_PRECHARGE_2US_SHIFT) |
			   (aux_clock_divider << DP_AUX_CH_CTL_BIT_CLOCK_2X_SHIFT) |
			   DP_AUX_CH_CTL_DONE |
			   DP_AUX_CH_CTL_TIME_OUT_ERROR |
			   DP_AUX_CH_CTL_RECEIVE_ERROR, ch_ctl);
		for (;;) {
			status = io_i915_READ32(ch_ctl);
			if ((status & DP_AUX_CH_CTL_SEND_BUSY) == 0)
				break;
			udelay(100);
		}

		/* Clear done status and any errors */
		io_i915_WRITE32(
			   status |
			   DP_AUX_CH_CTL_DONE |
			   DP_AUX_CH_CTL_TIME_OUT_ERROR |
			   DP_AUX_CH_CTL_RECEIVE_ERROR, ch_ctl);

		if (status & (DP_AUX_CH_CTL_TIME_OUT_ERROR |
			      DP_AUX_CH_CTL_RECEIVE_ERROR))
			continue;
		if (status & DP_AUX_CH_CTL_DONE)
			break;
	}

	if ((status & DP_AUX_CH_CTL_DONE) == 0) {
		printk(BIOS_SPEW, "[000000.0] [drm:%s], ", __func__);
		printk(BIOS_SPEW, "dp_aux_ch not done status 0x%08x\n", status);
		return -1;
	}

	/* Check for timeout or receive error.
	 * Timeouts occur when the sink is not connected
	 */
	if (status & DP_AUX_CH_CTL_RECEIVE_ERROR) {
		printk(BIOS_SPEW, "[000000.0] [drm:%s], ", __func__);
		printk(BIOS_SPEW, "dp_aux_ch receive error status 0x%08x\n", status);
		return -1;
	}

	/* Timeouts occur when the device isn't connected, so they're
	 * "normal" -- don't fill the kernel log with these */
	if (status & DP_AUX_CH_CTL_TIME_OUT_ERROR) {
		printk(BIOS_SPEW, "[000000.0] [drm:%s], ", __func__);
		printk(BIOS_SPEW, "dp_aux_ch timeout status 0x%08x\n", status);
		return -1;
	}

	/* Unload any bytes sent back from the other side */
	recv_bytes = ((status & DP_AUX_CH_CTL_MESSAGE_SIZE_MASK) >>
		      DP_AUX_CH_CTL_MESSAGE_SIZE_SHIFT);
	if (recv_bytes > recv_size)
		recv_bytes = recv_size;

	for (i = 0; i < recv_bytes; i += 4)
		unpack_aux(io_i915_READ32(ch_data + i),
			   recv + i, recv_bytes - i);

	return recv_bytes;
}

