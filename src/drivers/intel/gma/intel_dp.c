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

/* This code was created by the coccinnelle filters in the i915tool,
 * with some final hand filtering.
 */

#include <console/console.h>
#include <stdint.h>
#include <delay.h>
#include <drivers/intel/gma/i915.h>
#include <string.h>

/**
 * is_edp - is the given port attached to an eDP panel (either CPU or PCH)
 * @param intel_dp: DP struct
 *
 * If a CPU or PCH DP output is attached to an eDP panel, this function
 * will return 1, and 0 otherwise.
 */
static int is_edp(struct intel_dp *intel_dp)
{
	return intel_dp->type == INTEL_OUTPUT_EDP;
}

/**
 * is_pch_edp - is the port on the PCH and attached to an eDP panel?
 * @param intel_dp: DP struct
 *
 * Returns 1 if the given DP struct corresponds to a PCH DP port attached
 * to an eDP panel, 0 otherwise.  Helpful for determining whether we
 * may need FDI resources for a given DP output or not.
 */
static int is_pch_edp(struct intel_dp *intel_dp)
{
	return intel_dp->is_pch_edp;
}

/**
 * is_cpu_edp - is the port on the CPU and attached to an eDP panel?
 * @param intel_dp: DP struct
 *
 * Returns 1 if the given DP struct corresponds to a CPU eDP port.
 */
static int is_cpu_edp(struct intel_dp *intel_dp)
{
	return is_edp(intel_dp) && !is_pch_edp(intel_dp);
}

static uint32_t
pack_aux(uint8_t *src, int src_bytes)
{
	int	i;
	uint32_t v = 0;

	if (src_bytes > 4)
		src_bytes = 4;
	for (i = 0; i < src_bytes; i++)
		v |= ((uint32_t) src[i]) << ((3-i) * 8);
	return v;
}

void
unpack_aux(uint32_t src, uint8_t *dst, int dst_bytes)
{
	int i;
	if (dst_bytes > 4)
		dst_bytes = 4;
	for (i = 0; i < dst_bytes; i++)
		dst[i] = src >> ((3-i) * 8);
}

static int ironlake_edp_have_panel_power(struct intel_dp *intel_dp)
{

	return (gtt_read(PCH_PP_STATUS) & PP_ON) != 0;
}

static int ironlake_edp_have_panel_vdd(struct intel_dp *intel_dp)
{
	return (gtt_read(PCH_PP_CONTROL) & EDP_FORCE_VDD) != 0;
}

int
intel_dp_aux_ch(struct intel_dp *intel_dp,
		uint8_t *send, int send_bytes,
		uint8_t *recv, int recv_size)
{
	uint32_t output_reg = intel_dp->output_reg;
	uint32_t ch_ctl = output_reg + 0x10;
	uint32_t ch_data = ch_ctl + 4;
	int i;
	int recv_bytes;
	uint32_t status;
	int try;

	/* Try to wait for any previous AUX channel activity */
	for (try = 0; try < 3; try++) {
		status = gtt_read(ch_ctl);
		if ((status & DP_AUX_CH_CTL_SEND_BUSY) == 0)
			break;
		mdelay(1);
	}

	if (try == 3) {
		if (1) {
			status = gtt_read(ch_ctl);
			printk(BIOS_ERR,
			       "dp_aux_ch not started status 0x%08x\n",
			       status);
		}
		return -1;
	}

	/* Must try at least 3 times according to DP spec */
	for (try = 0; try < 5; try++) {
		/* Load the send data into the aux channel data registers */
		for (i = 0; i < send_bytes; i += 4){
			u32 val, addr;
			val = pack_aux(send + i, send_bytes - i);
			addr = ch_data + i;
			gtt_write(addr,val);
		}

		/* Send the command and wait for it to complete */
		gtt_write(ch_ctl, DP_AUX_CH_CTL_SEND_BUSY |
			  DP_AUX_CH_CTL_TIME_OUT_400us |
			  (send_bytes << DP_AUX_CH_CTL_MESSAGE_SIZE_SHIFT) |
			  (intel_dp->precharge << DP_AUX_CH_CTL_PRECHARGE_2US_SHIFT) |
			  (intel_dp->aux_clock_divider << DP_AUX_CH_CTL_BIT_CLOCK_2X_SHIFT) |
			  DP_AUX_CH_CTL_DONE |
			  DP_AUX_CH_CTL_TIME_OUT_ERROR |
			  DP_AUX_CH_CTL_RECEIVE_ERROR);
		for (;;) {
			status = gtt_read(ch_ctl);
			if ((status & DP_AUX_CH_CTL_SEND_BUSY) == 0)
				break;
			udelay(100);
		}

		/* Clear done status and any errors */
		gtt_write(ch_ctl, status |
			  DP_AUX_CH_CTL_DONE |
			  DP_AUX_CH_CTL_TIME_OUT_ERROR |
			  DP_AUX_CH_CTL_RECEIVE_ERROR);

		if (status & (DP_AUX_CH_CTL_TIME_OUT_ERROR |
			      DP_AUX_CH_CTL_RECEIVE_ERROR))
			continue;
		if (status & DP_AUX_CH_CTL_DONE)
			break;
	}

	if ((status & DP_AUX_CH_CTL_DONE) == 0) {
		printk(BIOS_ERR, "dp_aux_ch not done status 0x%08x\n", status);
		return -1;
	}

	/* Check for timeout or receive error.
	 * Timeouts occur when the sink is not connected
	 */
	if (status & DP_AUX_CH_CTL_RECEIVE_ERROR) {
		printk(BIOS_ERR,
		       "dp_aux_ch receive error status 0x%08x\n", status);
		return -1;
	}

	/* Timeouts occur when the device isn't connected, so they're
	 * "normal" -- don't fill the kernel log with these */
	if (status & DP_AUX_CH_CTL_TIME_OUT_ERROR) {
		printk(BIOS_ERR, "dp_aux_ch timeout status 0x%08x\n", status);
		return -1;
	}

	/* Unload any bytes sent back from the other side */
	recv_bytes = ((status & DP_AUX_CH_CTL_MESSAGE_SIZE_MASK) >>
		      DP_AUX_CH_CTL_MESSAGE_SIZE_SHIFT);
	if (recv_bytes > recv_size)
		recv_bytes = recv_size;

	for (i = 0; i < recv_bytes; i += 4)
		unpack_aux(gtt_read(ch_data + i),
			   recv + i, recv_bytes - i);

	return recv_bytes;
}

/* Write data to the aux channel in native mode */
static int
intel_dp_aux_native_write(struct intel_dp *intel_dp,
			  uint16_t address, uint8_t *send, int send_bytes)
{
	int ret;
	uint8_t	msg[20];
	int msg_bytes;
	uint8_t	ack;

	if (send_bytes > 16)
		return -1;
	msg[0] = AUX_NATIVE_WRITE << 4;
	msg[1] = address >> 8;
	msg[2] = address & 0xff;
	msg[3] = send_bytes - 1;
	memcpy(&msg[4], send, send_bytes);
	msg_bytes = send_bytes + 4;
	for (;;) {
		ret = intel_dp_aux_ch(intel_dp, msg, msg_bytes, &ack, 1);
		if (ret < 0)
			return ret;
		if ((ack & AUX_NATIVE_REPLY_MASK) == AUX_NATIVE_REPLY_ACK)
			break;
		else if ((ack & AUX_NATIVE_REPLY_MASK) == AUX_NATIVE_REPLY_DEFER)
			udelay(100);
		else
			return -1;
	}
	return send_bytes;
}

/* Write a single byte to the aux channel in native mode */
static int
intel_dp_aux_native_write_1(struct intel_dp *intel_dp,
			    uint16_t address, uint8_t byte)
{
	return intel_dp_aux_native_write(intel_dp, address, &byte, 1);
}

int intel_dp_set_bw(struct intel_dp *intel_dp)
{
	printk(BIOS_SPEW, "DP_LINK_BW_SET");
	return intel_dp_aux_native_write_1(intel_dp,
					   DP_LINK_BW_SET,
					   intel_dp->link_bw);
}

int intel_dp_set_lane_count(struct intel_dp *intel_dp)
{
	printk(BIOS_SPEW, "DP_LANE_COUNT_SET %d ", intel_dp->lane_count);
	return intel_dp_aux_native_write_1(intel_dp,
					   DP_LANE_COUNT_SET,
					   intel_dp->lane_count);
}

int intel_dp_set_training_pattern(struct intel_dp *intel_dp,
				  u8 pat)
{
	printk(BIOS_SPEW, "DP_TRAINING_PATTERN_SET");
	return intel_dp_aux_native_write_1(intel_dp,
					   DP_TRAINING_PATTERN_SET,
					   pat);
}

int intel_dp_set_training_lane0(struct intel_dp *intel_dp,
				u8 val)
{
	printk(BIOS_SPEW, "DP_TRAINING_LANE0_SET");
	return intel_dp_aux_native_write_1(intel_dp,
					   DP_TRAINING_LANE0_SET,
					   val);
}

/* read bytes from a native aux channel */
static int
intel_dp_aux_native_read(struct intel_dp *intel_dp,
			 uint16_t address, uint8_t *recv, int recv_bytes)
{
	uint8_t msg[4];
	int msg_bytes;
	uint8_t reply[20];
	int reply_bytes;
	uint8_t ack;
	int ret;

	msg[0] = AUX_NATIVE_READ << 4;
	msg[1] = address >> 8;
	msg[2] = address & 0xff;
	msg[3] = recv_bytes - 1;

	msg_bytes = 4;
	reply_bytes = recv_bytes + 1;

	for (;;) {
		ret = intel_dp_aux_ch(intel_dp, msg, msg_bytes,
				      reply, reply_bytes);
		if (ret == 0)
			return -1;
		if (ret < 0)
			return ret;
		ack = reply[0];
		if ((ack & AUX_NATIVE_REPLY_MASK) == AUX_NATIVE_REPLY_ACK) {
			memcpy(recv, reply + 1, ret - 1);
			return ret - 1;
		}
		else if ((ack & AUX_NATIVE_REPLY_MASK) == AUX_NATIVE_REPLY_DEFER)
			udelay(100);
		else
			return -1;
	}
}

int
intel_dp_i2c_aux_ch(struct intel_dp *intel_dp,
		    int mode, uint8_t write_byte, uint8_t *read_byte)
{
	uint8_t msg[5];
	uint8_t reply[2];
	unsigned retry;
	int msg_bytes;
	int reply_bytes;
	int ret;

	/* Set up the command byte */
	if (mode & MODE_I2C_READ)
		msg[0] = AUX_I2C_READ << 4;
	else
		msg[0] = AUX_I2C_WRITE << 4;

	if (!(mode & MODE_I2C_STOP))
		msg[0] |= AUX_I2C_MOT << 4;

	msg[1] = intel_dp->address >> 8;
	msg[2] = intel_dp->address;

	switch (mode) {
	case MODE_I2C_WRITE:
		msg[3] = 0;
		msg[4] = write_byte;
		msg_bytes = 5;
		reply_bytes = 1;
		break;
	case MODE_I2C_READ:
		msg[3] = 0;
		msg_bytes = 4;
		reply_bytes = 2;
		break;
	default:
		msg_bytes = 3;
		reply_bytes = 1;
		break;
	}

	for (retry = 0; retry < 5; retry++) {
		ret = intel_dp_aux_ch(intel_dp,
				      msg, msg_bytes,
				      reply, reply_bytes);
		if (ret < 0) {
			printk(BIOS_ERR, "aux_ch failed %d\n", ret);
			return ret;
		}

		switch (reply[0] & AUX_NATIVE_REPLY_MASK) {
		case AUX_NATIVE_REPLY_ACK:
			/* I2C-over-AUX Reply field is only valid
			 * when paired with AUX ACK.
			 */
			break;
		case AUX_NATIVE_REPLY_NACK:
			printk(BIOS_ERR, "aux_ch native nack\n");
			return -1;
		case AUX_NATIVE_REPLY_DEFER:
			udelay(100);
			continue;
		default:
			printk(BIOS_ERR, "aux_ch invalid native reply 0x%02x\n",
			       reply[0]);
			return -1;
		}

		switch (reply[0] & AUX_I2C_REPLY_MASK) {
		case AUX_I2C_REPLY_ACK:
			if (mode == MODE_I2C_READ) {
				*read_byte = reply[1];
			}
			return reply_bytes - 1;
		case AUX_I2C_REPLY_NACK:
			printk(BIOS_ERR, "aux_i2c nack\n");
			return -1;
		case AUX_I2C_REPLY_DEFER:
			printk(BIOS_ERR, "aux_i2c defer\n");
			udelay(100);
			break;
		default:
			printk(BIOS_ERR,
			       "aux_i2c invalid reply 0x%02x\n", reply[0]);
			return -1;
		}
	}

	printk(BIOS_ERR, "too many retries, giving up\n");
	return -1;
}

int intel_dp_i2c_write(struct intel_dp *intel_dp,
		       u8 val)
{
	return intel_dp_i2c_aux_ch(intel_dp,
				   MODE_I2C_WRITE,
				   val,
				   NULL);
}

int intel_dp_i2c_read(struct intel_dp *intel_dp,
		      u8 *val)
{
	return intel_dp_i2c_aux_ch(intel_dp,
				   MODE_I2C_READ,
				   0,
				   val);
}

int
intel_dp_i2c_init(struct intel_dp *intel_dp)
{
	int	ret = 0;

	/* not clear what we need to do here, if anything.
	 * this function was more about setting up the kernel.
	 * it's a handy placeholder, so we leave it in for now.
	 */
	return ret;
}

static void
intel_reduce_m_n_ratio(uint32_t *num, uint32_t *den)
{
	while (*num > DATA_LINK_M_N_MASK || *den > DATA_LINK_M_N_MASK) {
		*num >>= 1;
		*den >>= 1;
	}
}

unsigned int roundup_power_of_two(unsigned int n);

unsigned int roundup_power_of_two(unsigned int n)
{
	n--;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n++;
	return n;
}

static void compute_m_n(unsigned int m, unsigned int n,
			unsigned int *ret_m, unsigned int *ret_n)
{
	/* We noticed in the IO operations that
	 * the VBIOS was setting N to DATA_LINK_N_MAX.
	 * This makes sense, actually: the bigger N is, i.e.
	 * the bigger the denominator is, the bigger
	 * the numerator can be, and the more
	 * bits of numerator you get, the better the result.
	 * So, first pick the max of the two powers of two.
	 * And, in the (unlikely) event that you end up with
	 * something bigger than DATA_LINK_N_MAX, catch that
	 * case with a MIN. Note the second case is unlikely,
	 * but we are best off being careful.
	 */
	/*
	 * This code is incorrect and will always set ret_n
	 * to DATA_LINK_N_MAX.
	 */
	*ret_n = MAX(roundup_power_of_two(n), DATA_LINK_N_MAX);
	*ret_n = MIN(*ret_n, DATA_LINK_N_MAX);
	*ret_m = ( (unsigned long long)m * *ret_n) / n;
	intel_reduce_m_n_ratio(ret_m, ret_n);
}

void intel_dp_compute_m_n(unsigned int bits_per_pixel,
			  unsigned int nlanes,
			  unsigned int pixel_clock,
			  unsigned int link_clock,
			  struct intel_dp_m_n *m_n)
{
	m_n->tu = 64;
	compute_m_n(bits_per_pixel * pixel_clock,
		    link_clock * nlanes * 8,
		    &m_n->gmch_m, &m_n->gmch_n);

	compute_m_n(pixel_clock, link_clock,
		    &m_n->link_m, &m_n->link_n);
}

static void ironlake_edp_pll_off(void);

void
intel_dp_mode_set(struct intel_dp *intel_dp)
{
	/* Turn on the eDP PLL if needed */
	if (is_edp(intel_dp)) {
		if (!is_pch_edp(intel_dp))
			ironlake_edp_pll_on();
		else
			ironlake_edp_pll_off();
	}

	/*
	 * There are four kinds of DP registers:
	 *
	 * 	IBX PCH
	 * 	SNB CPU
	 *	IVB CPU
	 * 	CPT PCH
	 *
	 * IBX PCH and CPU are the same for almost everything,
	 * except that the CPU DP PLL is configured in this
	 * register
	 *
	 * CPT PCH is quite different, having many bits moved
	 * to the TRANS_DP_CTL register instead. That
	 * configuration happens (oddly) in ironlake_pch_enable
	 */

	/* Preserve the BIOS-computed detected bit. This is
	 * supposed to be read-only.
	 */
	intel_dp->DP = gtt_read(intel_dp->output_reg) & DP_DETECTED;
	printk(BIOS_SPEW, "%s: initial value is %08lx\n", __func__,
	       (unsigned long)intel_dp->DP);
	/* | 0 essentially */
	intel_dp->DP |=  DP_VOLTAGE_0_4 | DP_PRE_EMPHASIS_0;

	/* Handle DP bits in common between all three register formats */

	switch (intel_dp->lane_count) {
	case 1:
		intel_dp->DP |= DP_PORT_WIDTH_1;
		break;
	case 2:
		intel_dp->DP |= DP_PORT_WIDTH_2;
		break;
	case 4:
		intel_dp->DP |= DP_PORT_WIDTH_4;
		break;
	}
	memset(intel_dp->link_configuration, 0, DP_LINK_CONFIGURATION_SIZE);
	intel_dp->link_configuration[0] = intel_dp->link_bw;
	intel_dp->link_configuration[1] = intel_dp->lane_count;
	intel_dp->link_configuration[8] = DP_SET_ANSI_8B10B;
	/*
	 * Check for DPCD version > 1.1 and enhanced framing support
	 */
	if (intel_dp->dpcd[DP_DPCD_REV] >= 0x11 &&
	    (intel_dp->dpcd[DP_MAX_LANE_COUNT] & DP_ENHANCED_FRAME_CAP)) {
		intel_dp->link_configuration[1] |= DP_LANE_COUNT_ENHANCED_FRAME_EN;
	}

	/* Split out the IBX/CPU vs CPT settings */

	if (is_cpu_edp(intel_dp) && (intel_dp->gen == 7)) {
		/* what are these? We're not sure.
		   if (adjusted_mode->flags & DRM_MODE_FLAG_PHSYNC)
		   intel_dp->DP |= DP_SYNC_HS_HIGH;

		   if (adjusted_mode->flags & DRM_MODE_FLAG_PVSYNC)
		*/
		intel_dp->DP |= DP_SYNC_VS_HIGH;
		/* */
		intel_dp->DP |= DP_LINK_TRAIN_OFF_CPT;

		if (intel_dp->link_configuration[1] &
		    DP_LANE_COUNT_ENHANCED_FRAME_EN)
			intel_dp->DP |= DP_ENHANCED_FRAMING;

		intel_dp->DP |= intel_dp->pipe << 29;

		/* don't miss out required setting for eDP */
		intel_dp->DP |= DP_PLL_ENABLE;
		if (intel_dp->clock < 200000)
			intel_dp->DP |= DP_PLL_FREQ_160MHZ;
		else
			intel_dp->DP |= DP_PLL_FREQ_270MHZ;
	} else if (!intel_dp->has_pch_cpt || is_cpu_edp(intel_dp)) {
		intel_dp->DP |= intel_dp->color_range;
		intel_dp->DP |= DP_LINK_TRAIN_OFF;

		if (intel_dp->link_configuration[1] &
		    DP_LANE_COUNT_ENHANCED_FRAME_EN)
			intel_dp->DP |= DP_ENHANCED_FRAMING;

		if (intel_dp->pipe == 1)
			intel_dp->DP |= DP_PIPEB_SELECT;

		if (is_cpu_edp(intel_dp)) {
			/* don't miss out required setting for eDP */
			intel_dp->DP |= DP_PLL_ENABLE;
			if (intel_dp->clock < 200000)
				intel_dp->DP |= DP_PLL_FREQ_160MHZ;
			else
				intel_dp->DP |= DP_PLL_FREQ_270MHZ;
		}
	} else {
		intel_dp->DP |= DP_LINK_TRAIN_OFF_CPT;
	}
}

#define IDLE_ON_MASK		(PP_ON | 0 	  | PP_SEQUENCE_MASK | 0                     | PP_SEQUENCE_STATE_MASK)
#define IDLE_ON_VALUE   	(PP_ON | 0 	  | PP_SEQUENCE_NONE | 0                     | PP_SEQUENCE_STATE_ON_IDLE)

#define IDLE_OFF_MASK		(PP_ON | 0        | PP_SEQUENCE_MASK | 0                     | PP_SEQUENCE_STATE_MASK)
#define IDLE_OFF_VALUE		(0     | 0        | PP_SEQUENCE_NONE | 0                     | PP_SEQUENCE_STATE_OFF_IDLE)

#define IDLE_CYCLE_MASK		(PP_ON | 0        | PP_SEQUENCE_MASK | PP_CYCLE_DELAY_ACTIVE | PP_SEQUENCE_STATE_MASK)
#define IDLE_CYCLE_VALUE	(0     | 0        | PP_SEQUENCE_NONE | 0                     | PP_SEQUENCE_STATE_OFF_IDLE)

static void ironlake_wait_panel_status(struct intel_dp *intel_dp,
				       u32 mask,
				       u32 value)
{
	int i;
	u32 status;
	printk(BIOS_ERR, "[000000.0] [drm:%s], ", __func__);
	printk(BIOS_ERR, "mask %08lx value %08lx status %08lx control %08lx\n",
	       (unsigned long) mask, (unsigned long) value,
	       (unsigned long)gtt_read(PCH_PP_STATUS),
	       (unsigned long)gtt_read(PCH_PP_CONTROL));

	for(i = 0, status = gtt_read(PCH_PP_STATUS); ((status & mask) != value) && (i < 5000);
	    status = gtt_read(PCH_PP_STATUS)){
		udelay(10);
	}
	if (i > 5000){
		printk(BIOS_ERR,
		       "Panel status timeout: status %08lx control %08lx\n",
		       (unsigned long)gtt_read(PCH_PP_STATUS),
		       (unsigned long)gtt_read(PCH_PP_CONTROL));
	}
}

static void ironlake_wait_panel_on(struct intel_dp *intel_dp)
{
	printk(BIOS_ERR, "Wait for panel power on\n");
	ironlake_wait_panel_status(intel_dp, IDLE_ON_MASK, IDLE_ON_VALUE);
}

static void ironlake_wait_panel_off(struct intel_dp *intel_dp)
{
	printk(BIOS_ERR, "Wait for panel power off time\n");
	ironlake_wait_panel_status(intel_dp, IDLE_OFF_MASK, IDLE_OFF_VALUE);
}

static void ironlake_wait_panel_power_cycle(struct intel_dp *intel_dp)
{
	printk(BIOS_ERR, "Wait for panel power cycle\n");
	ironlake_wait_panel_status(intel_dp, IDLE_CYCLE_MASK, IDLE_CYCLE_VALUE);
}

void intel_dp_wait_reg(unsigned long addr,
		       unsigned long val)
{
	unsigned long newval;
	int tries = 0;

	while ((newval = gtt_read(addr)) != val) {
		udelay(1);
		if (tries++ > 1000) {
			printk(BIOS_ERR, "%s: Waiting on %08lx to be %08lx, got %08lx\n",
			       __func__, addr, val, newval);
			break;
		}
	}
}

void intel_dp_wait_panel_power_control(unsigned long val)
{
	intel_dp_wait_reg(PCH_PP_CONTROL, val);
}

/* Read the current pp_control value, unlocking the register if it
 * is locked
 */

static  u32 ironlake_get_pp_control(void)
{
	u32	control = gtt_read(PCH_PP_CONTROL);

	control &= ~PANEL_UNLOCK_MASK;
	control |= PANEL_UNLOCK_REGS;
	return control;
}

void ironlake_edp_panel_vdd_on(struct intel_dp *intel_dp)
{
	u32 pp;

	if (!is_edp(intel_dp))
		return;
	printk(BIOS_ERR, "Turn eDP VDD on\n");

	if (intel_dp->want_panel_vdd) {
		printk(BIOS_ERR, "eDP VDD already requested on\n");
	}

	intel_dp->want_panel_vdd = 1;

	if (ironlake_edp_have_panel_vdd(intel_dp)) {
		printk(BIOS_ERR, "eDP VDD already on\n");
		return;
	}

	if (!ironlake_edp_have_panel_power(intel_dp))
		ironlake_wait_panel_power_cycle(intel_dp);

	pp = ironlake_get_pp_control();
	pp |= EDP_FORCE_VDD;
	gtt_write(PCH_PP_CONTROL,pp);
	// remember this if we need it later. Not sure yet.
	////POSTING_READ(PCH_PP_CONTROL);
	printk(BIOS_ERR, "PCH_PP_STATUS: 0x%08lx PCH_PP_CONTROL: 0x%08lx\n",
		(unsigned long) gtt_read(PCH_PP_STATUS),
		(unsigned long) gtt_read(PCH_PP_CONTROL));

	/*
	 * If the panel wasn't on, delay before accessing aux channel
	 */
	if (!ironlake_edp_have_panel_power(intel_dp)) {
		printk(BIOS_ERR, "eDP was not running\n");
		mdelay(intel_dp->panel_power_up_delay);
	}
}

static void ironlake_panel_vdd_off_sync(struct intel_dp *intel_dp)
{
	u32 pp;

	if (!intel_dp->want_panel_vdd && ironlake_edp_have_panel_vdd(intel_dp)) {
		pp = ironlake_get_pp_control();
		pp &= ~EDP_FORCE_VDD;
		gtt_write(PCH_PP_CONTROL,pp);
		////POSTING_READ(PCH_PP_CONTROL);

		/* Make sure sequencer is idle before allowing subsequent activity */
		printk(BIOS_ERR, "PCH_PP_STATUS: 0x%08lx PCH_PP_CONTROL: 0x%08lx\n",
			(unsigned long) gtt_read(PCH_PP_STATUS),
			(unsigned long) gtt_read(PCH_PP_CONTROL));

		mdelay(intel_dp->panel_power_down_delay);
	}
}

void ironlake_edp_panel_vdd_off(struct intel_dp *intel_dp, int sync)
{
	if (!is_edp(intel_dp))
		return;

	printk(BIOS_ERR, "Turn eDP VDD off %d\n", intel_dp->want_panel_vdd);
	if (!intel_dp->want_panel_vdd) {
		printk(BIOS_ERR, "eDP VDD not forced on");
	}

	intel_dp->want_panel_vdd = 0;

	if (sync)
		ironlake_panel_vdd_off_sync(intel_dp);
}

void ironlake_edp_panel_on(struct intel_dp *intel_dp)
{
	u32 pp;

	if (!is_edp(intel_dp))
		return;

	printk(BIOS_ERR, "Turn eDP power on\n");

	if (ironlake_edp_have_panel_power(intel_dp)) {
		printk(BIOS_ERR, "eDP power already on\n");
		return;
	}

	ironlake_wait_panel_power_cycle(intel_dp);

	pp = ironlake_get_pp_control();
	if (intel_dp->gen == 5) {
		/* ILK workaround: disable reset around power sequence */
		pp &= ~PANEL_POWER_RESET;
		gtt_write(PCH_PP_CONTROL,pp);
		////POSTING_READ(PCH_PP_CONTROL);
	}

	pp |= POWER_TARGET_ON;
	if (!(intel_dp->gen == 5))
		pp |= PANEL_POWER_RESET;

	gtt_write(PCH_PP_CONTROL,pp);
	////POSTING_READ(PCH_PP_CONTROL);

	ironlake_wait_panel_on(intel_dp);

	if (intel_dp->gen == 5) {
		pp |= PANEL_POWER_RESET; /* restore panel reset bit */
		gtt_write(PCH_PP_CONTROL,pp);
		////POSTING_READ(PCH_PP_CONTROL);
	}
}

void ironlake_edp_panel_off(struct intel_dp *intel_dp)
{
	u32 pp;

	if (!is_edp(intel_dp))
		return;

	printk(BIOS_ERR, "Turn eDP power off\n");

	if (intel_dp->want_panel_vdd) {
		printk(BIOS_ERR, "Cannot turn power off while VDD is on\n");
	}

	pp = ironlake_get_pp_control();
	pp &= ~(POWER_TARGET_ON | EDP_FORCE_VDD |
		PANEL_POWER_RESET | EDP_BLC_ENABLE);
	gtt_write(PCH_PP_CONTROL,pp);
	////POSTING_READ(PCH_PP_CONTROL);

	ironlake_wait_panel_off(intel_dp);
}

void ironlake_edp_backlight_on(struct intel_dp *intel_dp)
{
	u32 pp;

	if (!is_edp(intel_dp))
		return;

	/*
	 * If we enable the backlight right away following a panel power
	 * on, we may see slight flicker as the panel syncs with the eDP
	 * link.  So delay a bit to make sure the image is solid before
	 * allowing it to appear.
	 */
	mdelay(intel_dp->backlight_on_delay);
	pp = ironlake_get_pp_control();
	pp |= EDP_BLC_ENABLE;
	gtt_write(PCH_PP_CONTROL,pp);
	////POSTING_READ(PCH_PP_CONTROL);
}

void ironlake_edp_backlight_off(struct intel_dp *intel_dp)
{
	u32 pp;

	if (!is_edp(intel_dp))
		return;

	pp = ironlake_get_pp_control();
	pp &= ~EDP_BLC_ENABLE;
	gtt_write(PCH_PP_CONTROL,pp);
	////POSTING_READ(PCH_PP_CONTROL);
	mdelay(intel_dp->backlight_off_delay);
}

void ironlake_edp_pll_on(void)
{
	u32 dpa_ctl;

	dpa_ctl = gtt_read(DP_A);
	dpa_ctl |= DP_PLL_ENABLE;
	gtt_write(DP_A,dpa_ctl);
	////POSTING_READ(DP_A);
	udelay(200);
}

static void ironlake_edp_pll_off(void)
{
	u32 dpa_ctl;

	dpa_ctl = gtt_read(DP_A);
	dpa_ctl &= ~DP_PLL_ENABLE;
	gtt_write(DP_A,dpa_ctl);
	////POSTING_READ(DP_A);
	udelay(200);
}

/* If the sink supports it, try to set the power state appropriately */
void intel_dp_sink_dpms(struct intel_dp *intel_dp, int mode)
{
	int ret, i;

	/* Should have a valid DPCD by this point */
	if (intel_dp->dpcd[DP_DPCD_REV] < 0x11)
		return;

	/* the convention, for whatever reason, is that mode > 0 means 'off' */
	if (mode) {
		ret = intel_dp_aux_native_write_1(intel_dp, DP_SET_POWER,
						  DP_SET_POWER_D3);
		if (ret != 1) {
			printk(BIOS_ERR, "failed to write sink power state\n");
		}
	} else {
		/*
		 * When turning on, we need to retry for 1ms to give the sink
		 * time to wake up.
		 */
		for (i = 0; i < 3; i++) {
			ret = intel_dp_aux_native_write_1(intel_dp,
							  DP_SET_POWER,
							  DP_SET_POWER_D0);
			if (ret == 1)
				break;
			mdelay(1);
		}
	}
}

/*
 * Native read with retry for link status and receiver capability reads for
 * cases where the sink may still be asleep.
 */
static int
intel_dp_aux_native_read_retry(struct intel_dp *intel_dp, uint16_t address,
			       uint8_t *recv, int recv_bytes)
{
	int ret, i;

	/*
	 * Sinks are *supposed* to come up within 1ms from an off state,
	 * but we're also supposed to retry 3 times per the spec.
	 */
	for (i = 0; i < 3; i++) {
		ret = intel_dp_aux_native_read(intel_dp, address, recv,
					       recv_bytes);
		if (ret == recv_bytes)
			return ret;
		mdelay(1);
	}

	return ret;
}

/*
 * Fetch AUX CH registers 0x202 - 0x207 which contain
 * link status information
 */
int
intel_dp_get_link_status(struct intel_dp *intel_dp,
			 uint8_t link_status[DP_LINK_STATUS_SIZE])
{
	int ret, i;

	ret = intel_dp_aux_native_read_retry(intel_dp,
					     DP_LANE0_1_STATUS,
					     link_status,
					     DP_LINK_STATUS_SIZE);

	printk(BIOS_SPEW, "%s:", __func__);
	for(i = 0; i < /* !!sizeof(link_status) == 4*/
		    DP_LINK_STATUS_SIZE; i++)
		printk(BIOS_SPEW, " %02x", link_status[i]);
	printk(BIOS_SPEW, "\n");
	return ret;
}

static uint8_t
intel_dp_link_status(uint8_t link_status[DP_LINK_STATUS_SIZE],
		     int r)
{
	return link_status[r - DP_LANE0_1_STATUS];
}

const char	*voltage_names[] = {
	"0.4V", "0.6V", "0.8V", "1.2V"
};
const char	*pre_emph_names[] = {
	"0dB", "3.5dB", "6dB", "9.5dB"
};
const char	*link_train_names[] = {
	"pattern 1", "pattern 2", "idle", "off"
};

/*
 * These are source-specific values; current Intel hardware supports
 * a maximum voltage of 800mV and a maximum pre-emphasis of 6dB
 */

static uint8_t
intel_dp_voltage_max(struct intel_dp *intel_dp)
{

	if ((intel_dp->gen == 7) && is_cpu_edp(intel_dp))
		return DP_TRAIN_VOLTAGE_SWING_800;
	else if (intel_dp->has_pch_cpt && !is_cpu_edp(intel_dp))
		return DP_TRAIN_VOLTAGE_SWING_1200;
	else
		return DP_TRAIN_VOLTAGE_SWING_800;
}

static uint8_t
intel_dp_pre_emphasis_max(struct intel_dp *intel_dp, uint8_t voltage_swing)
{

	if (intel_dp->is_haswell){
		switch (voltage_swing & DP_TRAIN_VOLTAGE_SWING_MASK) {
		case DP_TRAIN_VOLTAGE_SWING_400:
			return DP_TRAIN_PRE_EMPHASIS_9_5;
		case DP_TRAIN_VOLTAGE_SWING_600:
			return DP_TRAIN_PRE_EMPHASIS_6;
		case DP_TRAIN_VOLTAGE_SWING_800:
			return DP_TRAIN_PRE_EMPHASIS_3_5;
		case DP_TRAIN_VOLTAGE_SWING_1200:
		default:
			return DP_TRAIN_PRE_EMPHASIS_0;
		}
	} else if ((intel_dp->gen == 7) && is_cpu_edp(intel_dp)) {
		switch (voltage_swing & DP_TRAIN_VOLTAGE_SWING_MASK) {
		case DP_TRAIN_VOLTAGE_SWING_400:
			return DP_TRAIN_PRE_EMPHASIS_6;
		case DP_TRAIN_VOLTAGE_SWING_600:
		case DP_TRAIN_VOLTAGE_SWING_800:
			return DP_TRAIN_PRE_EMPHASIS_3_5;
		default:
			return DP_TRAIN_PRE_EMPHASIS_0;
		}
	} else {
		switch (voltage_swing & DP_TRAIN_VOLTAGE_SWING_MASK) {
		case DP_TRAIN_VOLTAGE_SWING_400:
			return DP_TRAIN_PRE_EMPHASIS_6;
		case DP_TRAIN_VOLTAGE_SWING_600:
			return DP_TRAIN_PRE_EMPHASIS_6;
		case DP_TRAIN_VOLTAGE_SWING_800:
			return DP_TRAIN_PRE_EMPHASIS_3_5;
		case DP_TRAIN_VOLTAGE_SWING_1200:
		default:
			return DP_TRAIN_PRE_EMPHASIS_0;
		}
	}
}

static void
intel_get_adjust_train(struct intel_dp *intel_dp,
		       uint8_t link_status[DP_LINK_STATUS_SIZE])
{
	uint8_t v = 0;
	uint8_t p = 0;
	int lane;
	uint8_t voltage_max;
	uint8_t preemph_max;

	for (lane = 0; lane < intel_dp->lane_count; lane++) {
		uint8_t this_v = drm_dp_get_adjust_request_voltage(
			link_status, lane);
		uint8_t this_p = drm_dp_get_adjust_request_pre_emphasis(
			link_status, lane);

		if (this_v > v)
			v = this_v;
		if (this_p > p)
			p = this_p;
	}

	voltage_max = intel_dp_voltage_max(intel_dp);
	if (v >= voltage_max)
		v = voltage_max | DP_TRAIN_MAX_SWING_REACHED;

	preemph_max = intel_dp_pre_emphasis_max(intel_dp, v);
	if (p >= preemph_max)
		p = preemph_max | DP_TRAIN_MAX_PRE_EMPHASIS_REACHED;

	printk(BIOS_SPEW, "%s: set to %s %s%s %s\n",
	       __func__,
	       voltage_names[v&3], pre_emph_names[p&3],
	       v & DP_TRAIN_MAX_SWING_REACHED ? ",max volt swing reached":"",
	       p & DP_TRAIN_MAX_PRE_EMPHASIS_REACHED ?
	       ", max pre emph reached" : "");
	for (lane = 0; lane < 4; lane++)
		intel_dp->train_set[lane] = v | p;
}

static uint32_t
intel_dp_signal_levels(uint8_t train_set)
{
	uint32_t	signal_levels = 0;

	switch (train_set & DP_TRAIN_VOLTAGE_SWING_MASK) {
	case DP_TRAIN_VOLTAGE_SWING_400:
	default:
		signal_levels |= DP_VOLTAGE_0_4;
		break;
	case DP_TRAIN_VOLTAGE_SWING_600:
		signal_levels |= DP_VOLTAGE_0_6;
		break;
	case DP_TRAIN_VOLTAGE_SWING_800:
		signal_levels |= DP_VOLTAGE_0_8;
		break;
	case DP_TRAIN_VOLTAGE_SWING_1200:
		signal_levels |= DP_VOLTAGE_1_2;
		break;
	}
	switch (train_set & DP_TRAIN_PRE_EMPHASIS_MASK) {
	case DP_TRAIN_PRE_EMPHASIS_0:
	default:
		signal_levels |= DP_PRE_EMPHASIS_0;
		break;
	case DP_TRAIN_PRE_EMPHASIS_3_5:
		signal_levels |= DP_PRE_EMPHASIS_3_5;
		break;
	case DP_TRAIN_PRE_EMPHASIS_6:
		signal_levels |= DP_PRE_EMPHASIS_6;
		break;
	case DP_TRAIN_PRE_EMPHASIS_9_5:
		signal_levels |= DP_PRE_EMPHASIS_9_5;
		break;
	}
	return signal_levels;
}

/* Gen6's DP voltage swing and pre-emphasis control */
static uint32_t
intel_gen6_edp_signal_levels(uint8_t train_set)
{
	int signal_levels = train_set & (DP_TRAIN_VOLTAGE_SWING_MASK |
					 DP_TRAIN_PRE_EMPHASIS_MASK);
	switch (signal_levels) {
	case DP_TRAIN_VOLTAGE_SWING_400 | DP_TRAIN_PRE_EMPHASIS_0:
	case DP_TRAIN_VOLTAGE_SWING_600 | DP_TRAIN_PRE_EMPHASIS_0:
		return EDP_LINK_TRAIN_400_600MV_0DB_SNB_B;
	case DP_TRAIN_VOLTAGE_SWING_400 | DP_TRAIN_PRE_EMPHASIS_3_5:
		return EDP_LINK_TRAIN_400MV_3_5DB_SNB_B;
	case DP_TRAIN_VOLTAGE_SWING_400 | DP_TRAIN_PRE_EMPHASIS_6:
	case DP_TRAIN_VOLTAGE_SWING_600 | DP_TRAIN_PRE_EMPHASIS_6:
		return EDP_LINK_TRAIN_400_600MV_6DB_SNB_B;
	case DP_TRAIN_VOLTAGE_SWING_600 | DP_TRAIN_PRE_EMPHASIS_3_5:
	case DP_TRAIN_VOLTAGE_SWING_800 | DP_TRAIN_PRE_EMPHASIS_3_5:
		return EDP_LINK_TRAIN_600_800MV_3_5DB_SNB_B;
	case DP_TRAIN_VOLTAGE_SWING_800 | DP_TRAIN_PRE_EMPHASIS_0:
	case DP_TRAIN_VOLTAGE_SWING_1200 | DP_TRAIN_PRE_EMPHASIS_0:
		return EDP_LINK_TRAIN_800_1200MV_0DB_SNB_B;
	default:
		printk(BIOS_ERR, "[000000.0] [drm:%s], ", __func__);
		printk(BIOS_ERR, "Unsupported voltage swing/pre-emphasis level:"
		       "0x%x\n", signal_levels);
		return EDP_LINK_TRAIN_400_600MV_0DB_SNB_B;
	}
}

/* Gen7's DP voltage swing and pre-emphasis control */
static uint32_t
intel_gen7_edp_signal_levels(uint8_t train_set)
{
	int signal_levels = train_set & (DP_TRAIN_VOLTAGE_SWING_MASK |
					 DP_TRAIN_PRE_EMPHASIS_MASK);
	switch (signal_levels) {
	case DP_TRAIN_VOLTAGE_SWING_400 | DP_TRAIN_PRE_EMPHASIS_0:
		return EDP_LINK_TRAIN_400MV_0DB_IVB;
	case DP_TRAIN_VOLTAGE_SWING_400 | DP_TRAIN_PRE_EMPHASIS_3_5:
		return EDP_LINK_TRAIN_400MV_3_5DB_IVB;
	case DP_TRAIN_VOLTAGE_SWING_400 | DP_TRAIN_PRE_EMPHASIS_6:
		return EDP_LINK_TRAIN_400MV_6DB_IVB;

	case DP_TRAIN_VOLTAGE_SWING_600 | DP_TRAIN_PRE_EMPHASIS_0:
		return EDP_LINK_TRAIN_600MV_0DB_IVB;
	case DP_TRAIN_VOLTAGE_SWING_600 | DP_TRAIN_PRE_EMPHASIS_3_5:
		return EDP_LINK_TRAIN_600MV_3_5DB_IVB;

	case DP_TRAIN_VOLTAGE_SWING_800 | DP_TRAIN_PRE_EMPHASIS_0:
		return EDP_LINK_TRAIN_800MV_0DB_IVB;
	case DP_TRAIN_VOLTAGE_SWING_800 | DP_TRAIN_PRE_EMPHASIS_3_5:
		return EDP_LINK_TRAIN_800MV_3_5DB_IVB;

	default:
		printk(BIOS_ERR, "[000000.0] [drm:%s], ", __func__);
		printk(BIOS_ERR, "Unsupported voltage swing/pre-emphasis level:"
		       "0x%x\n", signal_levels);
		return EDP_LINK_TRAIN_500MV_0DB_IVB;
	}
}

/* Gen7.5's (HSW) DP voltage swing and pre-emphasis control */
static uint32_t
intel_dp_signal_levels_hsw(uint8_t train_set)
{
	int signal_levels = train_set & (DP_TRAIN_VOLTAGE_SWING_MASK |
					 DP_TRAIN_PRE_EMPHASIS_MASK);
	switch (signal_levels) {
	case DP_TRAIN_VOLTAGE_SWING_400 | DP_TRAIN_PRE_EMPHASIS_0:
		return DDI_BUF_EMP_400MV_0DB_HSW;
	case DP_TRAIN_VOLTAGE_SWING_400 | DP_TRAIN_PRE_EMPHASIS_3_5:
		return DDI_BUF_EMP_400MV_3_5DB_HSW;
	case DP_TRAIN_VOLTAGE_SWING_400 | DP_TRAIN_PRE_EMPHASIS_6:
		return DDI_BUF_EMP_400MV_6DB_HSW;
	case DP_TRAIN_VOLTAGE_SWING_400 | DP_TRAIN_PRE_EMPHASIS_9_5:
		return DDI_BUF_EMP_400MV_9_5DB_HSW;

	case DP_TRAIN_VOLTAGE_SWING_600 | DP_TRAIN_PRE_EMPHASIS_0:
		return DDI_BUF_EMP_600MV_0DB_HSW;
	case DP_TRAIN_VOLTAGE_SWING_600 | DP_TRAIN_PRE_EMPHASIS_3_5:
		return DDI_BUF_EMP_600MV_3_5DB_HSW;
	case DP_TRAIN_VOLTAGE_SWING_600 | DP_TRAIN_PRE_EMPHASIS_6:
		return DDI_BUF_EMP_600MV_6DB_HSW;

	case DP_TRAIN_VOLTAGE_SWING_800 | DP_TRAIN_PRE_EMPHASIS_0:
		return DDI_BUF_EMP_800MV_0DB_HSW;
	case DP_TRAIN_VOLTAGE_SWING_800 | DP_TRAIN_PRE_EMPHASIS_3_5:
		return DDI_BUF_EMP_800MV_3_5DB_HSW;
	default:
		printk(BIOS_SPEW,
		       "Unsupported voltage swing/pre-emphasis level:"
		       "0x%x\n", signal_levels);
		return DDI_BUF_EMP_400MV_0DB_HSW;
	}
}


static uint8_t
intel_get_lane_status(uint8_t link_status[DP_LINK_STATUS_SIZE],
		      int lane)
{
	int s = (lane & 1) * 4;
	uint8_t l = link_status[lane>>1];

	return (l >> s) & 0xf;
}

/* Check for clock recovery is done on all channels */
static int
intel_clock_recovery_ok(uint8_t link_status[DP_LINK_STATUS_SIZE],
			int lane_count)
{
	int lane;
	uint8_t lane_status;

	for (lane = 0; lane < lane_count; lane++) {
		lane_status = intel_get_lane_status(link_status, lane);
		printk(BIOS_SPEW,
		       "%s: Lane %d, status %02x\n", __func__,
		       lane, lane_status);
		if ((lane_status & DP_LANE_CR_DONE) == 0)
			return 0;
	}
	return 1;
}

/* Check to see if channel eq is done on all channels */
#define CHANNEL_EQ_BITS (DP_LANE_CR_DONE|		\
			 DP_LANE_CHANNEL_EQ_DONE|	\
			 DP_LANE_SYMBOL_LOCKED)
int
intel_channel_eq_ok(struct intel_dp *intel_dp,
		    uint8_t link_status[DP_LINK_STATUS_SIZE])
{
	uint8_t lane_align;
	uint8_t lane_status;
	int lane;

	lane_align = intel_dp_link_status(link_status,
					  DP_LANE_ALIGN_STATUS_UPDATED);
	if ((lane_align & DP_INTERLANE_ALIGN_DONE) == 0)
		return 0;
	for (lane = 0; lane < intel_dp->lane_count; lane++) {
		lane_status = intel_get_lane_status(link_status, lane);
		if ((lane_status & CHANNEL_EQ_BITS) != CHANNEL_EQ_BITS)
			return 0;
	}
	return 1;
}

static int
intel_dp_set_link_train(struct intel_dp *intel_dp,
			uint32_t dp_reg_value,
			uint8_t dp_train_pat)
{
	int ret;
	u32 temp;
	int port = intel_dp->port;
	int i;

	printk(BIOS_SPEW, "%s: dp_reg_value %08lx dp_train_pat %02x\n",
	       __func__, (unsigned long) dp_reg_value, dp_train_pat);
	if (intel_dp->is_haswell){
		temp = gtt_read(DP_TP_CTL(port));

		if (dp_train_pat & DP_LINK_SCRAMBLING_DISABLE)
			temp |= DP_TP_CTL_SCRAMBLE_DISABLE;
		else
			temp &= ~DP_TP_CTL_SCRAMBLE_DISABLE;

		temp &= ~DP_TP_CTL_LINK_TRAIN_MASK;
		switch (dp_train_pat & DP_TRAINING_PATTERN_MASK) {
		case DP_TRAINING_PATTERN_DISABLE:
			temp |= DP_TP_CTL_LINK_TRAIN_IDLE;
			gtt_write(DP_TP_CTL(port), temp);

			for(i = 0; i < 10; i++){
				u32 status;
				status = gtt_read(DP_TP_STATUS(port));
				if (status & DP_TP_STATUS_IDLE_DONE)
					break;
			}

			if (i == 10)
				printk(BIOS_ERR,
				       "Timed out waiting for DP idle patterns\n");

			temp &= ~DP_TP_CTL_LINK_TRAIN_MASK;
			temp |= DP_TP_CTL_LINK_TRAIN_NORMAL;

			break;
		case DP_TRAINING_PATTERN_1:
			temp |= DP_TP_CTL_LINK_TRAIN_PAT1;
			break;
		case DP_TRAINING_PATTERN_2:
			temp |= DP_TP_CTL_LINK_TRAIN_PAT2;
			break;
		case DP_TRAINING_PATTERN_3:
			temp |= DP_TP_CTL_LINK_TRAIN_PAT3;
			break;
		}
		gtt_write(DP_TP_CTL(port), temp);

	} else if (intel_dp->has_pch_cpt &&
		   (intel_dp->gen != 7 || !is_cpu_edp(intel_dp))) {
		dp_reg_value &= ~DP_LINK_TRAIN_MASK_CPT;

		switch (dp_train_pat & DP_TRAINING_PATTERN_MASK) {
		case DP_TRAINING_PATTERN_DISABLE:
			dp_reg_value |= DP_LINK_TRAIN_OFF_CPT;
			break;
		case DP_TRAINING_PATTERN_1:
			dp_reg_value |= DP_LINK_TRAIN_PAT_1_CPT;
			break;
		case DP_TRAINING_PATTERN_2:
			dp_reg_value |= DP_LINK_TRAIN_PAT_2_CPT;
			break;
		case DP_TRAINING_PATTERN_3:
			printk(BIOS_ERR,
			       "DP training pattern 3 not supported\n");
			dp_reg_value |= DP_LINK_TRAIN_PAT_2_CPT;
			break;
		}

	} else {
		dp_reg_value &= ~DP_LINK_TRAIN_MASK;

		switch (dp_train_pat & DP_TRAINING_PATTERN_MASK) {
		case DP_TRAINING_PATTERN_DISABLE:
			dp_reg_value |= DP_LINK_TRAIN_OFF;
			break;
		case DP_TRAINING_PATTERN_1:
			dp_reg_value |= DP_LINK_TRAIN_PAT_1;
			break;
		case DP_TRAINING_PATTERN_2:
			dp_reg_value |= DP_LINK_TRAIN_PAT_2;
			break;
		case DP_TRAINING_PATTERN_3:
			printk(BIOS_ERR,"DP training pattern 3 not supported\n");
			dp_reg_value |= DP_LINK_TRAIN_PAT_2;
			break;
		}
	}

	gtt_write(intel_dp->output_reg, dp_reg_value);
	//POSTING_READ(intel_dp->output_reg);

	intel_dp_aux_native_write_1(intel_dp,
				    DP_TRAINING_PATTERN_SET,
				    dp_train_pat);

	if ((dp_train_pat & DP_TRAINING_PATTERN_MASK) !=
	    DP_TRAINING_PATTERN_DISABLE) {
		ret = intel_dp_aux_native_write(intel_dp,
						DP_TRAINING_LANE0_SET,
						intel_dp->train_set,
						intel_dp->lane_count);
		if (ret != intel_dp->lane_count){
			printk(BIOS_ERR, "%s: wanted %d, got %d\n", __func__,
			       intel_dp->lane_count, ret);
			return 0;
		}
	}

	printk(BIOS_SPEW, "%s: success\n", __func__);
	return 1;
}

/* Enable corresponding port and start training pattern 1 */
void
intel_dp_start_link_train(struct intel_dp *intel_dp)
{
	int i;
	uint8_t voltage;
	int clock_recovery = 0;
	int voltage_tries, loop_tries;
	u32 reg;
	uint32_t DP = intel_dp->DP;

	if (intel_dp->is_haswell)
		intel_ddi_prepare_link_retrain(intel_dp, intel_dp->port);

	/* Write the link configuration data */
	printk(BIOS_SPEW, "Write the link configuration data\n");
	intel_dp_aux_native_write(intel_dp, DP_LINK_BW_SET,
				  intel_dp->link_configuration,
				  DP_LINK_CONFIGURATION_SIZE);
	printk(BIOS_SPEW, "Written\n");

	DP |= DP_PORT_EN;

	memset(intel_dp->train_set, 0, 4);
	voltage = 0xff;
	voltage_tries = 0;
	loop_tries = 0;
	clock_recovery = 0;

	for (;;) {
		/* Use intel_dp->train_set[0] to set the voltage and pre emphasis values */
		uint8_t	    link_status[DP_LINK_STATUS_SIZE];
		uint32_t    signal_levels;

		if (intel_dp->is_haswell){
			signal_levels =
				intel_dp_signal_levels_hsw(intel_dp->train_set[0]);
			DP = (DP & ~DDI_BUF_EMP_MASK) | signal_levels;
			printk(BIOS_SPEW, "Haswell: levels %08x DP %08x\n", signal_levels, DP);
		} else if ((intel_dp->gen == 7) && is_cpu_edp(intel_dp)) {
			signal_levels = intel_gen7_edp_signal_levels(intel_dp->train_set[0]);
			DP = (DP & ~EDP_LINK_TRAIN_VOL_EMP_MASK_IVB) | signal_levels;
		} else if ((intel_dp->gen == 6) && is_cpu_edp(intel_dp)) {
			signal_levels = intel_gen6_edp_signal_levels(intel_dp->train_set[0]);
			DP = (DP & ~EDP_LINK_TRAIN_VOL_EMP_MASK_SNB) | signal_levels;
		} else {
			signal_levels = intel_dp_signal_levels(intel_dp->train_set[0]);
			DP = (DP & ~(DP_VOLTAGE_MASK|DP_PRE_EMPHASIS_MASK)) | signal_levels;
		}
		printk(BIOS_ERR, "training pattern 1 signal levels %08x\n", signal_levels);

		if (intel_dp->has_pch_cpt && ((intel_dp->gen == 7) || !is_cpu_edp(intel_dp)))
			reg = DP | DP_LINK_TRAIN_PAT_1_CPT;
		else
			reg = DP | DP_LINK_TRAIN_PAT_1;

		if (!intel_dp_set_link_train(intel_dp, reg,
					     DP_TRAINING_PATTERN_1 |
					     DP_LINK_SCRAMBLING_DISABLE))
			break;
		/* Set training pattern 1 */

		udelay(100);
		if (!intel_dp_get_link_status(intel_dp, link_status)) {
			printk(BIOS_ERR, "failed to get link status\n");
			break;
		}

		if (intel_clock_recovery_ok(link_status, intel_dp->lane_count)) {
			printk(BIOS_ERR, "clock recovery OK\n");
			clock_recovery = 1;
			break;
		}

		/* Check to see if we've tried the max voltage */
		for (i = 0; i < intel_dp->lane_count; i++)
			if ((intel_dp->train_set[i] & DP_TRAIN_MAX_SWING_REACHED) == 0)
				break;
		if (i == intel_dp->lane_count) {
			++loop_tries;
			if (loop_tries == 5) {
				printk(BIOS_ERR, "too many full retries, give up\n");
				break;
			}
			printk(BIOS_SPEW, "%s: reset train set\n", __func__);
			memset(intel_dp->train_set, 0, 4);
			voltage_tries = 0;
			continue;
		}

		/* Check to see if we've tried the same voltage 5 times */
		if ((intel_dp->train_set[0] & DP_TRAIN_VOLTAGE_SWING_MASK) == voltage) {
			++voltage_tries;
			if (voltage_tries == 5) {
				printk(BIOS_ERR, "too many voltage retries, give up\n");
				break;
			}
		} else
			voltage_tries = 0;
		voltage = intel_dp->train_set[0] & DP_TRAIN_VOLTAGE_SWING_MASK;
		printk(BIOS_SPEW, "%s: voltage now %s\n", __func__,
		       voltage_names[voltage]);

		/* Compute new intel_dp->train_set as requested by target */
		intel_get_adjust_train(intel_dp, link_status);
		printk(BIOS_SPEW, "%s: new intel train set is "
		       "%02x%02x%02x%02x",
		       __func__,
		       intel_dp->train_set[0], intel_dp->train_set[1],
		       intel_dp->train_set[2], intel_dp->train_set[3]);
	}

	intel_dp->DP = DP;
}

int
intel_dp_get_dpcd(struct intel_dp *intel_dp)
{
	int got = 0, want = sizeof(intel_dp->dpcd), rev;
	got = intel_dp_aux_native_read_retry(intel_dp, 0x000, intel_dp->dpcd,
					     want);
	if (got < want) {
		printk(BIOS_SPEW, "%s: got %d, wanted %d\n", __func__, got, want);
		return 0;
	}

	rev = intel_dp->dpcd[DP_DPCD_REV];
	if (!rev){
		printk(BIOS_SPEW, "%s: intel->dp[DP_DPCD_REV] is 0\n",
		       __func__);
		return 0;
	}

	printk(BIOS_SPEW, "DPCD: %02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n",
	       intel_dp->dpcd[0], intel_dp->dpcd[1], intel_dp->dpcd[2],
	       intel_dp->dpcd[3], intel_dp->dpcd[4], intel_dp->dpcd[5],
	       intel_dp->dpcd[6], intel_dp->dpcd[7]);
	return 1;
}

/* I have no idea how big max downspread is. 1 byte? Hard as hell to find. */
int
intel_dp_get_max_downspread(struct intel_dp *intel_dp, u8 *max_downspread)
{
	int got, want = 1;
	got = intel_dp_aux_native_read_retry(intel_dp, DP_MAX_DOWNSPREAD, max_downspread,
					     want);
	if (got < want) {
		printk(BIOS_SPEW, "%s: got %d, wanted %d\n", __func__, got, want);
		return 0;
	}

	printk(BIOS_SPEW, "%s: max_downspread is %02x\n", __func__, *max_downspread);
	return 1;
}

void intel_dp_set_m_n_regs(struct intel_dp *dp)
{
        gtt_write(PIPE_DATA_M1(dp->transcoder),
		  TU_SIZE(dp->m_n.tu) | dp->m_n.gmch_m);
        gtt_write(PIPE_DATA_N1(dp->transcoder),dp->m_n.gmch_n);
        gtt_write(PIPE_LINK_M1(dp->transcoder),dp->m_n.link_m);
        gtt_write(PIPE_LINK_N1(dp->transcoder),dp->m_n.link_n);
}

int intel_dp_bw_code_to_link_rate(u8 link_bw)
{
	switch (link_bw) {
        default:
		printk(BIOS_ERR,
			"ERROR: link_bw(%d) is bogus; must be one of 6, 0xa, or 0x14\n",
			link_bw);
        case DP_LINK_BW_1_62:
		return 162000;
        case DP_LINK_BW_2_7:
		return 270000;
        case DP_LINK_BW_5_4:
                return 540000;
	}
}

void intel_dp_set_resolution(struct intel_dp *intel_dp)
{
        gtt_write(HTOTAL(intel_dp->transcoder),intel_dp->htotal);
        gtt_write(HBLANK(intel_dp->transcoder),intel_dp->hblank);
        gtt_write(HSYNC(intel_dp->transcoder),intel_dp->hsync);
        gtt_write(VTOTAL(intel_dp->transcoder),intel_dp->vtotal);
        gtt_write(VBLANK(intel_dp->transcoder),intel_dp->vblank);
        gtt_write(VSYNC(intel_dp->transcoder),intel_dp->vsync);
}

int intel_dp_get_training_pattern(struct intel_dp *intel_dp,
				  u8 *recv)
{
	return intel_dp_aux_native_read_retry(intel_dp,
					      DP_TRAINING_PATTERN_SET,
					      recv,
					      0);
}

int intel_dp_get_lane_count(struct intel_dp *intel_dp,
			    u8 *recv)
{
	int val = intel_dp_aux_native_read_retry(intel_dp,
					      DP_LANE_COUNT_SET,
					      recv,
					      0);
	*recv &= DP_LANE_COUNT_MASK;
	printk(BIOS_SPEW, "Lane count %s:%d\n", val < 0 ? "fail" : "ok", *recv);
	return val;
}

int intel_dp_get_lane_align_status(struct intel_dp *intel_dp,
				   u8 *recv)
{
	return intel_dp_aux_native_read_retry(intel_dp,
					      DP_LANE_ALIGN_STATUS_UPDATED,
					      recv,
					      0);
}
