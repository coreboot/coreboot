/*
 * Copyright 2013 Google Inc.
 * Copyright © 2012 Intel Corporation
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
 *    Eugeni Dodonov <eugeni.dodonov@intel.com>
 *
 */
#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <device/device.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <delay.h>
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <boot/coreboot_tables.h>
#include <smbios.h>
#include <device/pci.h>
#include <ec/google/chromeec/ec.h>

#include <cpu/x86/tsc.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <edid.h>
#include <drivers/intel/gma/i915.h>

/* HDMI/DVI modes ignore everything but the last 2 items. So we share
 * them for both DP and FDI transports, allowing those ports to
 * automatically adapt to HDMI connections as well.
 */
static  u32 hsw_ddi_translations_dp[] = {
	0x00FFFFFF, 0x0006000E,		/* DP parameters */
	0x00D75FFF, 0x0005000A,
	0x00C30FFF, 0x00040006,
	0x80AAAFFF, 0x000B0000,
	0x00FFFFFF, 0x0005000A,
	0x00D75FFF, 0x000C0004,
	0x80C30FFF, 0x000B0000,
	0x00FFFFFF, 0x00040006,
	0x80D75FFF, 0x000B0000,
	0x00FFFFFF, 0x00040006		/* HDMI parameters */
};

static  u32 hsw_ddi_translations_fdi[] = {
	0x00FFFFFF, 0x0007000E,		/* FDI parameters */
	0x00D75FFF, 0x000F000A,
	0x00C30FFF, 0x00060006,
	0x00AAAFFF, 0x001E0000,
	0x00FFFFFF, 0x000F000A,
	0x00D75FFF, 0x00160004,
	0x00C30FFF, 0x001E0000,
	0x00FFFFFF, 0x00060006,
	0x00D75FFF, 0x001E0000,
	0x00FFFFFF, 0x00040006		/* HDMI parameters */
};

/* On Haswell, DDI port buffers must be programmed with correct values
 * in advance. The buffer values are different for FDI and DP modes,
 * but the HDMI/DVI fields are shared among those. So we program the DDI
 * in either FDI or DP modes only, as HDMI connections will work with both
 * of those.
 */
static void intel_prepare_ddi_buffers(int port, int use_fdi_mode)
{
	u32 reg;
	int i;
	u32 *ddi_translations = ((use_fdi_mode) ?
				 hsw_ddi_translations_fdi :
				 hsw_ddi_translations_dp);

	printk(BIOS_SPEW,  "Initializing DDI buffers for port %d in %s mode\n",
	       port,
	       use_fdi_mode ? "FDI" : "DP");

	for (i=0,reg=DDI_BUF_TRANS(port);i < ARRAY_SIZE(hsw_ddi_translations_fdi);i++) {
		gtt_write(reg,ddi_translations[i]);
		reg += 4;
	}
}

void intel_prepare_ddi(void)
{
	int port;
	u32 use_fdi = 1;

	for (port = PORT_A; port < PORT_E; port++)
		intel_prepare_ddi_buffers(port, !use_fdi);

	intel_prepare_ddi_buffers(PORT_E, use_fdi);
}

static void intel_wait_ddi_buf_idle(int port)
{
	uint32_t reg = DDI_BUF_CTL(port);
	int i;

	for (i = 0; i < 8; i++) {
		udelay(1);
		if (gtt_read(reg) & DDI_BUF_IS_IDLE){
			printk(BIOS_SPEW, "%s: buf is idle (success)\n", __func__);
			return;
		}
	}
	printk(BIOS_ERR, "Timeout waiting for DDI BUF %d idle bit\n", port);
}


void intel_ddi_prepare_link_retrain(struct intel_dp *intel_dp, int port)
{
	int wait = 0;
	uint32_t val;

	if (gtt_read(DP_TP_CTL(port)) & DP_TP_CTL_ENABLE) {
		val = gtt_read(DDI_BUF_CTL(port));
		if (val & DDI_BUF_CTL_ENABLE) {
			val &= ~DDI_BUF_CTL_ENABLE;
			gtt_write(val,DDI_BUF_CTL(port));
			wait = 1;
		}

		val = gtt_read(DP_TP_CTL(port));
		val &= ~(DP_TP_CTL_ENABLE | DP_TP_CTL_LINK_TRAIN_MASK);
		val |= DP_TP_CTL_LINK_TRAIN_PAT1;
		gtt_write(val,DP_TP_CTL(port));
		//POSTING_READ(DP_TP_CTL(port));

		if (wait)
			intel_wait_ddi_buf_idle(port);
	}

	val = DP_TP_CTL_ENABLE | DP_TP_CTL_MODE_SST |
	      DP_TP_CTL_LINK_TRAIN_PAT1 | DP_TP_CTL_SCRAMBLE_DISABLE;
	if (intel_dp->link_configuration[1] & DP_LANE_COUNT_ENHANCED_FRAME_EN)
		val |= DP_TP_CTL_ENHANCED_FRAME_ENABLE;
	gtt_write(val,DP_TP_CTL(port));
	//POSTING_READ(DP_TP_CTL(port));

	intel_dp->DP |= DDI_BUF_CTL_ENABLE;
	gtt_write(intel_dp->DP,DDI_BUF_CTL(port));
	//POSTING_READ(DDI_BUF_CTL(port));

	udelay(600);
}

u32 intel_ddi_calc_transcoder_flags(u32 pipe_bpp,
				    enum port port,
				    enum pipe pipe,
				    int type,
				    int lane_count,
				    int pf_sz,
				    u8 phsync,
				    u8 pvsync)
{
	u32 temp;

	temp = TRANS_DDI_FUNC_ENABLE;
	temp |= TRANS_DDI_SELECT_PORT(port);

	switch (pipe_bpp) {
	case 18:
		temp |= TRANS_DDI_BPC_6;
		break;
	case 24:
		temp |= TRANS_DDI_BPC_8;
		break;
	case 30:
		temp |= TRANS_DDI_BPC_10;
		break;
	case 36:
		temp |= TRANS_DDI_BPC_12;
		break;
	default:
		printk(BIOS_ERR, "Invalid pipe_bpp: %d, *** Initialization will not succeed ***\n", pipe_bpp);
	}

	if (port == PORT_A) {
		switch (pipe) {
		case PIPE_A:
			if (pf_sz)
				temp |= TRANS_DDI_EDP_INPUT_A_ONOFF;
			else
				temp |= TRANS_DDI_EDP_INPUT_A_ON;
			break;
		case PIPE_B:
			temp |= TRANS_DDI_EDP_INPUT_B_ONOFF;
			break;
		case PIPE_C:
			temp |= TRANS_DDI_EDP_INPUT_C_ONOFF;
			break;
		default:
			printk(BIOS_ERR, "Invalid pipe %d\n", pipe);
		}
	}

	if (phsync)
		temp |= TRANS_DDI_PHSYNC;

	if (pvsync)
		temp |= TRANS_DDI_PVSYNC;

	if (type == INTEL_OUTPUT_HDMI) {
		/* Need to understand when to set TRANS_DDI_MODE_SELECT_HDMI / TRANS_DDI_MODE_SELECT_DVI */
	} else if (type == INTEL_OUTPUT_ANALOG) {
		/* Set TRANS_DDI_MODE_SELECT_FDI with lane_count */
	} else if (type == INTEL_OUTPUT_DISPLAYPORT ||
		   type == INTEL_OUTPUT_EDP) {
		temp |= TRANS_DDI_MODE_SELECT_DP_SST;

		temp |= DDI_PORT_WIDTH(lane_count);
	} else {
		printk(BIOS_ERR, "Invalid type %d for pipe\n", type);
	}

	return temp;
}

enum transcoder intel_ddi_get_transcoder(enum port port,
					 enum pipe pipe)
{
	if (port == PORT_A)
		return TRANSCODER_EDP;
	return (enum transcoder)pipe;
}

void intel_ddi_set_pipe_settings(struct intel_dp *intel_dp)
{
	u32 val = TRANS_MSA_SYNC_CLK;

	switch (intel_dp->pipe_bits_per_pixel) {
	case 18:
		val |= TRANS_MSA_6_BPC;
		break;
	case 24:
		val |= TRANS_MSA_8_BPC;
		break;
	case 30:
		val |= TRANS_MSA_10_BPC;
		break;
	case 36:
		val |= TRANS_MSA_12_BPC;
		break;
	default:
		printk(BIOS_ERR, "Invalid bpp settings %d\n", intel_dp->pipe_bits_per_pixel);
	}
	gtt_write(TRANS_MSA_MISC(intel_dp->transcoder),val);
}
