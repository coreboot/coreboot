/*
 * This file is part of the coreboot project.
 *
 * Copyright 2012 Google Inc.
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

#include <drivers/intel/gma/i915_reg.h>
#include <drivers/intel/gma/drm_dp_helper.h>
#include <edid.h>

/* port types. We stick with the same defines as the kernel */
#define INTEL_OUTPUT_UNUSED 0
#define INTEL_OUTPUT_ANALOG 1
#define INTEL_OUTPUT_DVO 2
#define INTEL_OUTPUT_SDVO 3
#define INTEL_OUTPUT_LVDS 4
#define INTEL_OUTPUT_TVOUT 5
#define INTEL_OUTPUT_HDMI 6
#define INTEL_OUTPUT_DISPLAYPORT 7
#define INTEL_OUTPUT_EDP 8

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

/*
 * To communicate to and control the extracted-from-kernel code,
 * we need this struct. It has a counterpart in the ARM code, so
 * there is a precedent.
 */

#define DP_RECEIVER_CAP_SIZE	0xf
#define DP_LINK_STATUS_SIZE	6
#define DP_LINK_CHECK_TIMEOUT	(10 * 1000)

#define DP_LINK_CONFIGURATION_SIZE	9

struct intel_dp {
	int gen; // 6 for link, 7 for wtm2
	int has_pch_split; // 1 for link and wtm2
	int has_pch_cpt; // 1 for everything we know about.
	int is_haswell;
	/* output register offset in MMIO space. Usually DP_A */
	u32 output_reg;
	/* The initial value of the DP register.
	 * Mainboards can set this to a non-zero
	 * value in the case that there are undetectable
	 * but essential parameters.
	 */
	u32 DP;
	uint8_t  link_configuration[DP_LINK_CONFIGURATION_SIZE];
	u32 color_range;
	/* these paramaters are determined after reading the DPCD */
	int dpms_mode;
	uint8_t link_bw;
	uint8_t lane_count;
	/* This data is read from the panel via the AUX channel.*/
	uint8_t dpcd[DP_RECEIVER_CAP_SIZE];
	int type;
	int edp;
	int is_pch_edp;
	/* state related to training. */
	uint8_t	train_set[4];
	/* Determined from EDID or coreboot hard-sets. */
	int panel_power_up_delay;
	int panel_power_down_delay;
	int panel_power_cycle_delay;
	int backlight_on_delay;
	int backlight_off_delay;
	int want_panel_vdd;
	u32 clock;
	int port;
	int pipe;
	int bpp;
	/* i2c on aux is ... interesting.
	 * Before you do an i2c cycle, you need to set the address.
	 * This requires we remember it from one moment to the next.
	 * Remember it here.
	 */
	u16 address;
	/* timing parameters for aux channel IO. They used to compute these on
	 * each and every entry to the functions, which is kind of stupid, and it had
	 * errors anyway.
	 * note: you can get these from watching YABEL output. E.g.:
	 * you see an outb of 0x802300e1 to 64010. the 3 is the precharge
	 * and the e1 is the clock divider.
	 */
	u32 aux_clock_divider;
	u32 precharge;
	/* EDID, raw and processed */
	u8 rawedid[256];
	int edidlen;
	struct edid edid;
	/* computed values needed for "i915" registers */
	int bytes_per_pixel;
	u32 htotal;
	u32 hblank;
	u32 hsync;
	u32 vtotal;
	u32 vblank;
	u32 vsync;
	u32 pfa_sz;
	u32 pfa_pos;
	u32 pfa_ctl;
	u32 pipesrc;
	u32 stride;
};

/* we may yet need these. */
void intel_dp_mode_set(struct intel_dp *intel_dp);
void intel_dp_start_link_train(struct intel_dp *intel_dp);
void intel_dp_complete_link_train(struct intel_dp *intel_dp);
void intel_dp_link_down(struct intel_dp *intel_dp);

int intel_dp_i2c_init(struct intel_dp *intel_dp);
int intel_dp_i2c_aux_ch(struct intel_dp *intel_dp,
			int mode, uint8_t write_byte, uint8_t *read_byte);
void
intel_dp_dpms(struct intel_dp *intel_dp, int mode);

int intel_dp_get_dpcd(struct intel_dp *intel_dp);
struct edid *intel_dp_get_edid(struct intel_dp *intel_dp);

void intel_dp_sink_dpms(struct intel_dp *intel_dp, int mode);
void ironlake_edp_pll_on(void);
void ironlake_edp_panel_vdd_on(struct intel_dp *intel_dp);
void ironlake_edp_panel_vdd_off(struct intel_dp *intel_dp, int sync);
int intel_dp_get_max_downspread(struct intel_dp *intel_dp, u8 *max_downspread);
void ironlake_edp_panel_on(struct intel_dp *intel_dp);
void ironlake_edp_backlight_on(struct intel_dp *intel_dp);
/* needed only on haswell. */
void intel_prepare_ddi_buffers(int port, int use_fdi_mode);
void intel_ddi_prepare_link_retrain(struct intel_dp *intel_dp, int port);
int intel_dp_aux_ch(struct intel_dp *intel_dp,
		    uint8_t *send, int send_bytes,
		    uint8_t *recv, int recv_size);
void unpack_aux(u32 src, uint8_t *dst, int dst_bytes);


/* drm_dp_helper.c */
int drm_dp_channel_eq_ok(u8 link_status[DP_LINK_STATUS_SIZE],
			 int lane_count);
int drm_dp_clock_recovery_ok(u8 link_status[DP_LINK_STATUS_SIZE],
			     int lane_count);
u8 drm_dp_get_adjust_request_voltage(u8 link_status[DP_LINK_STATUS_SIZE],
				     int lane);
u8 drm_dp_get_adjust_request_pre_emphasis(u8 link_status[DP_LINK_STATUS_SIZE],
					  int lane);


void intel_dp_wait_reg(unsigned long addr,
		       unsigned long val);

void intel_dp_wait_panel_power_control(unsigned long val);

