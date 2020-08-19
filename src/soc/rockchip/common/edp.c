/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <edid.h>
#include <string.h>
#include <soc/addressmap.h>
#include <soc/display.h>
#include <soc/edp.h>
#include <timer.h>
#include <types.h>

#define edp_debug(x...) do {if (0) printk(BIOS_DEBUG, x); } while (0)

static struct rk_edp rk_edp;

#define MAX_CR_LOOP 5
#define MAX_EQ_LOOP 5
#define DP_LINK_STATUS_SIZE 6

static const char *voltage_names[] = {
	"0.4V", "0.6V", "0.8V", "1.2V"
};
static const char *pre_emph_names[] = {
	"0dB", "3.5dB", "6dB", "9.5dB"
};

#define DP_VOLTAGE_MAX         DP_TRAIN_VOLTAGE_SWING_1200
#define DP_PRE_EMPHASIS_MAX    DP_TRAIN_PRE_EMPHASIS_9_5

static void rk_edp_init_refclk(struct rk_edp *edp)
{
	write32(&edp->regs->analog_ctl_2, SEL_24M);
	write32(&edp->regs->pll_reg_1, REF_CLK_24M);

	/*initial value*/
	write32(&edp->regs->pll_reg_2, LDO_OUTPUT_V_SEL_145 | KVCO_DEFALUT |
		CHG_PUMP_CUR_SEL_5US | V2L_CUR_SEL_1MA);

	write32(&edp->regs->pll_reg_3, LOCK_DET_CNT_SEL_256 |
		LOOP_FILTER_RESET | PALL_SSC_RESET | LOCK_DET_BYPASS |
		PLL_LOCK_DET_MODE | PLL_LOCK_DET_FORCE);

	write32(&edp->regs->pll_reg_5, REGULATOR_V_SEL_950MV | STANDBY_CUR_SEL |
		CHG_PUMP_INOUT_CTRL_1200MV | CHG_PUMP_INPUT_CTRL_OP);

	write32(&edp->regs->ssc_reg, SSC_OFFSET | SSC_MODE | SSC_DEPTH);

	write32(&edp->regs->tx_common, TX_SWING_PRE_EMP_MODE |
		PRE_DRIVER_PW_CTRL1 | LP_MODE_CLK_REGULATOR |
		RESISTOR_MSB_CTRL | RESISTOR_CTRL);

	write32(&edp->regs->dp_aux, DP_AUX_COMMON_MODE |
		DP_AUX_EN | AUX_TERM_50OHM);

	write32(&edp->regs->dp_bias, DP_BG_OUT_SEL | DP_DB_CUR_CTRL |
		DP_BG_SEL | DP_RESISTOR_TUNE_BG);

	write32(&edp->regs->dp_reserv2,
		CH1_CH3_SWING_EMP_CTRL | CH0_CH2_SWING_EMP_CTRL);
}

static void rk_edp_init_interrupt(struct rk_edp *edp)
{
	/* Set interrupt pin assertion polarity as high */
	write32(&edp->regs->int_ctl, INT_POL);

	/* Clear pending registers */
	write32(&edp->regs->common_int_sta_1, 0xff);
	write32(&edp->regs->common_int_sta_2, 0x4f);
	write32(&edp->regs->common_int_sta_3, 0xff);
	write32(&edp->regs->common_int_sta_4, 0x27);
	write32(&edp->regs->dp_int_sta, 0x7f);

	/* 0:mask,1: unmask */
	write32(&edp->regs->common_int_mask_1, 0x00);
	write32(&edp->regs->common_int_mask_2, 0x00);
	write32(&edp->regs->common_int_mask_3, 0x00);
	write32(&edp->regs->common_int_mask_4, 0x00);
	write32(&edp->regs->int_sta_mask, 0x00);
}

static void rk_edp_enable_sw_function(struct rk_edp *edp)
{
	clrbits32(&edp->regs->func_en_1, SW_FUNC_EN_N);
}

static int rk_edp_get_pll_lock_status(struct rk_edp *edp)
{
	u32 val;

	val = read32(&edp->regs->dp_debug_ctl);
	return (val & PLL_LOCK) ? DP_PLL_LOCKED : DP_PLL_UNLOCKED;
}

static void rk_edp_init_analog_func(struct rk_edp *edp)
{
	struct stopwatch sw;

	write32(&edp->regs->dp_pd, 0x00);

	write32(&edp->regs->common_int_sta_1, PLL_LOCK_CHG);

	clrbits32(&edp->regs->dp_debug_ctl, F_PLL_LOCK | PLL_LOCK_CTRL);

	stopwatch_init_msecs_expire(&sw, PLL_LOCK_TIMEOUT);

	while (rk_edp_get_pll_lock_status(edp) == DP_PLL_UNLOCKED) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "%s: PLL is not locked\n", __func__);
			return;
		}
	}

	/* Enable Serdes FIFO function and Link symbol clock domain module */
	clrbits32(&edp->regs->func_en_2, SERDES_FIFO_FUNC_EN_N |
				       LS_CLK_DOMAIN_FUNC_EN_N | AUX_FUNC_EN_N |
				       SSC_FUNC_EN_N);
}

static void rk_edp_init_aux(struct rk_edp *edp)
{
	/* Clear interrupts related to AUX channel */
	write32(&edp->regs->dp_int_sta, AUX_FUNC_EN_N);

	/* Disable AUX channel module */
	setbits32(&edp->regs->func_en_2, AUX_FUNC_EN_N);

	/* Receive AUX Channel DEFER commands equal to DEFFER_COUNT*64 */
	write32(&edp->regs->aux_ch_defer_dtl, DEFER_CTRL_EN | DEFER_COUNT(1));

	/* Enable AUX channel module */
	clrbits32(&edp->regs->func_en_2, AUX_FUNC_EN_N);
}

static int rk_edp_aux_enable(struct rk_edp *edp)
{
	struct stopwatch sw;

	setbits32(&edp->regs->aux_ch_ctl_2, AUX_EN);
	stopwatch_init_msecs_expire(&sw, 20);
	do {
		if (!(read32(&edp->regs->aux_ch_ctl_2) & AUX_EN))
			return 0;
	} while (!stopwatch_expired(&sw));

	return -1;

}

static int rk_edp_is_aux_reply(struct rk_edp *edp)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, 10);

	while (!(read32(&edp->regs->dp_int_sta) & RPLY_RECEIV)) {
		if (stopwatch_expired(&sw))
			return -1;
	}

	write32(&edp->regs->dp_int_sta, RPLY_RECEIV);

	return 0;
}

static int rk_edp_start_aux_transaction(struct rk_edp *edp)
{
	int val;

	/* Enable AUX CH operation */
	if (rk_edp_aux_enable(edp)) {
		edp_debug("AUX CH enable timeout!\n");
		return -1;
	}

	/* Is AUX CH command reply received? */
	if (rk_edp_is_aux_reply(edp)) {
		edp_debug("AUX CH command reply failed!\n");
		return -1;
	}

	/* Clear interrupt source for AUX CH access error */
	val = read32(&edp->regs->dp_int_sta);
	if (val & AUX_ERR) {
		write32(&edp->regs->dp_int_sta, AUX_ERR);
		return -1;
	}

	/* Check AUX CH error access status */
	val = read32(&edp->regs->dp_int_sta);
	if ((val & AUX_STATUS_MASK) != 0) {
		edp_debug("AUX CH error happens: %d\n\n",
			val & AUX_STATUS_MASK);
		return -1;
	}

	return 0;
}

static int rk_edp_dpcd_transfer(struct rk_edp *edp,
				unsigned int val_addr, u8 *data,
				unsigned int length,
				enum dpcd_request request)
{
	int val;
	int i, try_times;
	int retval = 0;
	u32 len = 0;

	while (length) {
		len = MIN(length, 16);
		for (try_times = 0; try_times < 10; try_times++) {

			/* Clear AUX CH data buffer */
			val = BUF_CLR;
			write32(&edp->regs->buf_data_ctl, val);

			/* Select DPCD device address */
			val = AUX_ADDR_7_0(val_addr);
			write32(&edp->regs->aux_addr_7_0, val);
			val = AUX_ADDR_15_8(val_addr);
			write32(&edp->regs->aux_addr_15_8, val);
			val = AUX_ADDR_19_16(val_addr);
			write32(&edp->regs->aux_addr_19_16, val);

			/*
			 * Set DisplayPort transaction and read 1 byte
			 * If bit 3 is 1, DisplayPort transaction.
			 * If Bit 3 is 0, I2C transaction.
			 */
			if (request == DPCD_WRITE) {
				val = AUX_LENGTH(len) |
					AUX_TX_COMM_DP_TRANSACTION |
					AUX_TX_COMM_WRITE;
				for (i = 0; i < len; i++)
					write32(&edp->regs->buf_data[i],
						*data++);
			} else
				val = AUX_LENGTH(len) |
					AUX_TX_COMM_DP_TRANSACTION |
					AUX_TX_COMM_READ;

			write32(&edp->regs->aux_ch_ctl_1, val);

			/* Start AUX transaction */
			retval = rk_edp_start_aux_transaction(edp);
			if (retval == 0)
				break;
			else
				printk(BIOS_WARNING, "read dpcd Aux Transaction fail!\n");

		}

		if (retval)
			return -1;

		if (request == DPCD_READ) {
			for (i = 0; i < len; i++)
				*data++ = (u8)read32(&edp->regs->buf_data[i]);
		}

		length -= len;
		val_addr += 16;
	}
	return 0;
}

static int rk_edp_dpcd_read(struct rk_edp *edp, u32 addr,
			    u8 *values, size_t size)
{
	return rk_edp_dpcd_transfer(edp, addr, values, size, DPCD_READ);
}

static int rk_edp_dpcd_write(struct rk_edp *edp, u32 addr,
			     u8 *values, size_t size)
{
	return rk_edp_dpcd_transfer(edp, addr, values, size, DPCD_WRITE);
}

static int rk_edp_link_power_up(struct rk_edp *edp)
{
	u8 value;
	int err;

	/* DP_SET_POWER register is only available on DPCD v1.1 and later */
	if (edp->link_train.revision < 0x11)
		return 0;

	err = rk_edp_dpcd_read(edp, DPCD_LINK_POWER_STATE, &value, 1);
	if (err < 0)
		return err;

	value &= ~DP_SET_POWER_MASK;
	value |= DP_SET_POWER_D0;

	err = rk_edp_dpcd_write(edp, DPCD_LINK_POWER_STATE, &value, 1);
	if (err < 0)
		return err;

	/*
	 * According to the DP 1.1 specification, a "Sink Device must exit the
	 * power saving state within 1 ms" (Section 2.5.3.1, Table 5-52, "Sink
	 * Control Field" (register 0x600).
	 */
	mdelay(1);

	return 0;
}

static int rk_edp_link_configure(struct rk_edp *edp)
{
	u8 values[2];

	values[0] = edp->link_train.link_rate;
	values[1] = edp->link_train.lane_count;

	return rk_edp_dpcd_write(edp, DPCD_LINK_BW_SET, values, sizeof(values));
}

static void rk_edp_set_link_training(struct rk_edp *edp,
				     const u8 *training_values)
{
	int i;

	for (i = 0; i < edp->link_train.lane_count; i++)
		write32(&edp->regs->ln_link_trn_ctl[i], training_values[i]);
}

static u8 edp_link_status(const u8 *link_status, int r)
{
	return link_status[r - DPCD_LANE0_1_STATUS];
}

static int rk_edp_dpcd_read_link_status(struct rk_edp *edp, u8 *link_status)
{
	return rk_edp_dpcd_read(edp, DPCD_LANE0_1_STATUS, link_status,
				DP_LINK_STATUS_SIZE);
}

static u8 edp_get_lane_status(const u8 *link_status, int lane)
{
	int i = DPCD_LANE0_1_STATUS + (lane >> 1);
	int s = (lane & 1) * 4;
	u8 l = edp_link_status(link_status, i);

	return (l >> s) & 0xf;
}

static int rk_edp_clock_recovery_ok(const u8 *link_status, int lane_count)
{
	int lane;
	u8 lane_status;

	for (lane = 0; lane < lane_count; lane++) {
		lane_status = edp_get_lane_status(link_status, lane);
		if ((lane_status & DP_LANE_CR_DONE) == 0)
			return 0;
	}
	return 1;
}

static int rk_edp_channel_eq_ok(const u8 *link_status, int lane_count)
{
	u8 lane_align;
	u8 lane_status;
	int lane;

	lane_align = edp_link_status(link_status,
				    DPCD_LANE_ALIGN_STATUS_UPDATED);
	if ((lane_align & DP_INTERLANE_ALIGN_DONE) == 0)
		return 0;
	for (lane = 0; lane < lane_count; lane++) {
		lane_status = edp_get_lane_status(link_status, lane);
		if ((lane_status & DP_CHANNEL_EQ_BITS) != DP_CHANNEL_EQ_BITS)
			return 0;
	}
	return 1;
}

static u8
rk_edp_get_adjust_request_voltage(const u8 *link_status, int lane)
{
	int i = DPCD_ADJUST_REQUEST_LANE0_1 + (lane >> 1);
	int s = ((lane & 1) ?
		 DP_ADJUST_VOLTAGE_SWING_LANE1_SHIFT :
		 DP_ADJUST_VOLTAGE_SWING_LANE0_SHIFT);
	u8 l = edp_link_status(link_status, i);

	return ((l >> s) & 0x3) << DP_TRAIN_VOLTAGE_SWING_SHIFT;
}

static u8 rk_edp_get_adjust_request_pre_emphasis(const u8 *link_status,
						 int lane)
{
	int i = DPCD_ADJUST_REQUEST_LANE0_1 + (lane >> 1);
	int s = ((lane & 1) ?
		 DP_ADJUST_PRE_EMPHASIS_LANE1_SHIFT :
		 DP_ADJUST_PRE_EMPHASIS_LANE0_SHIFT);
	u8 l = edp_link_status(link_status, i);

	return ((l >> s) & 0x3) << DP_TRAIN_PRE_EMPHASIS_SHIFT;
}

static void edp_get_adjust_train(const u8 *link_status, int lane_count,
				 u8 train_set[])
{
	u8 v = 0;
	u8 p = 0;
	int lane;

	for (lane = 0; lane < lane_count; lane++) {
		u8 this_v =
			rk_edp_get_adjust_request_voltage(link_status, lane);
		u8 this_p =
			rk_edp_get_adjust_request_pre_emphasis(link_status,
								lane);

		printk(BIOS_DEBUG, "requested signal parameters: lane %d "
					"voltage %s pre_emph %s\n", lane,
			 voltage_names[this_v >> DP_TRAIN_VOLTAGE_SWING_SHIFT],
			 pre_emph_names[this_p >> DP_TRAIN_PRE_EMPHASIS_SHIFT]);

		if (this_v > v)
			v = this_v;
		if (this_p > p)
			p = this_p;
	}

	if (v >= DP_VOLTAGE_MAX)
		v |= DP_TRAIN_MAX_SWING_REACHED;

	if (p >= DP_PRE_EMPHASIS_MAX)
		p |= DP_TRAIN_MAX_PRE_EMPHASIS_REACHED;

	printk(BIOS_DEBUG, "using signal parameters: voltage %s pre_emph %s\n",
		  voltage_names[(v & DP_TRAIN_VOLTAGE_SWING_MASK)
		  >> DP_TRAIN_VOLTAGE_SWING_SHIFT],
		  pre_emph_names[(p & DP_TRAIN_PRE_EMPHASIS_MASK)
		  >> DP_TRAIN_PRE_EMPHASIS_SHIFT]);

	for (lane = 0; lane < 4; lane++)
		train_set[lane] = v | p;
}

static int rk_edp_link_train_cr(struct rk_edp *edp)
{
	int clock_recovery;
	u8 voltage, tries = 0;
	u8 status[DP_LINK_STATUS_SIZE];
	int i;
	u8 value;

	value = DP_TRAINING_PATTERN_1;
	write32(&edp->regs->dp_training_ptn_set, value);
	rk_edp_dpcd_write(edp, DPCD_TRAINING_PATTERN_SET, &value, 1);
	memset(edp->train_set, 0, 4);

	/* clock recovery loop */
	clock_recovery = 0;
	tries = 0;
	voltage = 0xff;

	while (1) {
		rk_edp_set_link_training(edp, edp->train_set);
		rk_edp_dpcd_write(edp, DPCD_TRAINING_LANE0_SET,
					edp->train_set,
					edp->link_train.lane_count);

		mdelay(1);

		if (rk_edp_dpcd_read_link_status(edp, status) < 0) {
			printk(BIOS_ERR, "displayport link status failed\n");
			break;
		}

		if (rk_edp_clock_recovery_ok(status,
			edp->link_train.lane_count)) {
			clock_recovery = 1;
			break;
		}

		for (i = 0; i < edp->link_train.lane_count; i++) {
			if ((edp->train_set[i] &
				DP_TRAIN_MAX_SWING_REACHED) == 0)
				break;
		}
		if (i == edp->link_train.lane_count) {
			printk(BIOS_ERR, "clock recovery reached max voltage\n");
			break;
		}

		if ((edp->train_set[0] &
			DP_TRAIN_VOLTAGE_SWING_MASK) == voltage) {
			++tries;
			if (tries == MAX_CR_LOOP) {
				printk(BIOS_ERR, "clock recovery tried 5 times\n");
				break;
			}
		} else
			tries = 0;

		voltage = edp->train_set[0] & DP_TRAIN_VOLTAGE_SWING_MASK;

		/* Compute new train_set as requested by sink */
		edp_get_adjust_train(status, edp->link_train.lane_count,
					edp->train_set);
	}
	if (!clock_recovery) {
		printk(BIOS_ERR, "clock recovery failed\n");
		return -1;
	} else {
		printk(BIOS_DEBUG, "clock recovery at voltage %d pre-emphasis %d\n",
			  edp->train_set[0] & DP_TRAIN_VOLTAGE_SWING_MASK,
			  (edp->train_set[0] & DP_TRAIN_PRE_EMPHASIS_MASK) >>
			  DP_TRAIN_PRE_EMPHASIS_SHIFT);
		return 0;
	}
}

static int rk_edp_link_train_ce(struct rk_edp *edp)
{
	int channel_eq;
	u8 value, tries = 0;
	u8 status[DP_LINK_STATUS_SIZE];

	value = DP_TRAINING_PATTERN_2;
	write32(&edp->regs->dp_training_ptn_set, value);
	rk_edp_dpcd_write(edp, DPCD_TRAINING_PATTERN_SET, &value, 1);

	/* channel equalization loop */
	channel_eq = 0;
	for (tries = 0; tries < 5; tries++) {
		rk_edp_set_link_training(edp, edp->train_set);
		rk_edp_dpcd_write(edp, DPCD_TRAINING_LANE0_SET,
					edp->train_set,
					edp->link_train.lane_count);

		udelay(400);
		if (rk_edp_dpcd_read_link_status(edp, status) < 0) {
			printk(BIOS_ERR, "displayport link status failed\n");
			return -1;
		}

		if (rk_edp_channel_eq_ok(status,
			edp->link_train.lane_count)) {
			channel_eq = 1;
			break;
		}
		edp_get_adjust_train(status,
			edp->link_train.lane_count,
			edp->train_set);
	}

	if (!channel_eq) {
		printk(BIOS_ERR, "channel eq failed\n");
		return -1;
	} else {
		printk(BIOS_DEBUG, "channel eq at voltage %d pre-emphasis %d\n",
			  edp->train_set[0] & DP_TRAIN_VOLTAGE_SWING_MASK,
			  (edp->train_set[0] & DP_TRAIN_PRE_EMPHASIS_MASK)
			  >> DP_TRAIN_PRE_EMPHASIS_SHIFT);
		return 0;
	}
}

static int rk_edp_init_training(struct rk_edp *edp)
{
	u8 values[3];
	int err;

	err = rk_edp_dpcd_read(edp, DPCD_DPCD_REV, values, sizeof(values));
	if (err < 0)
		return err;

	edp->link_train.revision = values[0];
	edp->link_train.link_rate = values[1];
	edp->link_train.lane_count = values[2] & DP_MAX_LANE_COUNT_MASK;

	edp_debug("max link rate:%d.%dGps max number of lanes:%d\n",
			edp->link_train.link_rate * 27 / 100,
			edp->link_train.link_rate * 27 % 100,
			edp->link_train.lane_count);

	if ((edp->link_train.link_rate != LINK_RATE_1_62GBPS) &&
	    (edp->link_train.link_rate != LINK_RATE_2_70GBPS)) {
		edp_debug("Rx Max Link Rate is abnormal :%x\n",
			edp->link_train.link_rate);
		return -1;
	}

	if (edp->link_train.lane_count == 0) {
		edp_debug("Rx Max Lane count is abnormal :%x\n",
			edp->link_train.lane_count);
		return -1;
	}

	rk_edp_link_power_up(edp);
	rk_edp_link_configure(edp);
	return 0;
}

static int rk_edp_hw_link_training(struct rk_edp *edp)
{
	u32 val;
	struct stopwatch sw;

	/* Set link rate and count as you want to establish*/
	write32(&edp->regs->link_bw_set, edp->link_train.link_rate);
	write32(&edp->regs->lane_count_set, edp->link_train.lane_count);

	if (rk_edp_link_train_cr(edp))
		return -1;
	if (rk_edp_link_train_ce(edp))
		return -1;

	write32(&edp->regs->dp_hw_link_training, HW_LT_EN);
	stopwatch_init_msecs_expire(&sw, 10);
	do {
		val = read32(&edp->regs->dp_hw_link_training);
		if (!(val & HW_LT_EN))
			break;
	} while (!stopwatch_expired(&sw));
	if (val & HW_LT_ERR_CODE_MASK) {
		printk(BIOS_ERR, "edp hw link training error: %d\n",
		val >> HW_LT_ERR_CODE_SHIFT);
		return -1;
	}
	return 0;

}

static int rk_edp_select_i2c_device(struct rk_edp *edp,
				    unsigned int device_addr,
				    unsigned int val_addr)
{
	u32 val;
	int retval;

	/* Set EDID device address */
	val = device_addr;
	write32(&edp->regs->aux_addr_7_0, val);
	write32(&edp->regs->aux_addr_15_8, 0x0);
	write32(&edp->regs->aux_addr_19_16, 0x0);

	/* Set offset from base address of EDID device */
	write32(&edp->regs->buf_data[0], val_addr);

	/*
	 * Set I2C transaction and write address
	 * If bit 3 is 1, DisplayPort transaction.
	 * If Bit 3 is 0, I2C transaction.
	 */
	val = AUX_TX_COMM_I2C_TRANSACTION | AUX_TX_COMM_MOT |
		AUX_TX_COMM_WRITE;
	write32(&edp->regs->aux_ch_ctl_1, val);

	/* Start AUX transaction */
	retval = rk_edp_start_aux_transaction(edp);
	if (retval != 0)
		edp_debug("select_i2c_device Aux Transaction fail!\n");

	return retval;
}

static int rk_edp_read_bytes_from_i2c(struct rk_edp *edp,
				      unsigned int device_addr,
				      unsigned int val_addr,
				      unsigned int count,
				      u8 edid[])
{
	u32 val;
	unsigned int i, j;
	unsigned int cur_data_idx;
	unsigned int defer = 0;
	int retval = 0;

	for (i = 0; i < count; i += 16) {
		for (j = 0; j < 10; j++) { /* try 10 times */
			/* Clear AUX CH data buffer */
			val = BUF_CLR;
			write32(&edp->regs->buf_data_ctl, val);

			/* Set normal AUX CH command */
			clrbits32(&edp->regs->aux_ch_ctl_2, ADDR_ONLY);

			/*
			 * If Rx sends defer, Tx sends only reads
			 * request without sending address
			 */
			if (!defer)
				retval = rk_edp_select_i2c_device(edp,
						device_addr, val_addr + i);
			else
				defer = 0;

			/*
			 * Set I2C transaction and write data
			 * If bit 3 is 1, DisplayPort transaction.
			 * If Bit 3 is 0, I2C transaction.
			 */
			val = AUX_LENGTH(16) | AUX_TX_COMM_I2C_TRANSACTION |
				AUX_TX_COMM_READ;
			write32(&edp->regs->aux_ch_ctl_1, val);

			/* Start AUX transaction */
			retval = rk_edp_start_aux_transaction(edp);
			if (retval == 0)
				break;
			else {
				edp_debug("Aux Transaction fail!\n");
				continue;
			}

			/* Check if Rx sends defer */
			val = read32(&edp->regs->aux_rx_comm);
			if (val == AUX_RX_COMM_AUX_DEFER ||
				val == AUX_RX_COMM_I2C_DEFER) {
				edp_debug("Defer: %d\n\n", val);
				defer = 1;
			}
		}

		if (retval)
			return -1;

		for (cur_data_idx = 0; cur_data_idx < 16; cur_data_idx++) {
			val = read32(&edp->regs->buf_data[cur_data_idx]);
			edid[i + cur_data_idx] = (u8)val;
		}
	}

	return retval;
}

static int rk_edp_read_edid(struct rk_edp *edp, struct edid *edid)
{
	u8 buf[EDID_LENGTH * 2];
	u32 edid_size = EDID_LENGTH;
	int retval;

	/* Read EDID data */
	retval = rk_edp_read_bytes_from_i2c(edp, EDID_ADDR,
				EDID_HEADER, EDID_LENGTH,
				&buf[EDID_HEADER]);
	if (retval != 0) {
		printk(BIOS_ERR, "EDID Read failed!\n");
		return -1;
	}

	/* check if edid have extension flag, and read additional EDID data */
	if (buf[EDID_EXTENSION_FLAG]) {
		edid_size += EDID_LENGTH;
		retval = rk_edp_read_bytes_from_i2c(edp, EDID_ADDR,
					EDID_LENGTH, EDID_LENGTH,
					&buf[EDID_LENGTH]);
		if (retval != 0) {
			printk(BIOS_ERR, "EDID Read failed!\n");
			return -1;
		}
	}

	if (decode_edid(buf, edid_size, edid) != EDID_CONFORMANT) {
		printk(BIOS_ERR, "%s: Failed to decode EDID.\n",
		       __func__);
		return -1;
	}

	edp_debug("EDID Read success!\n");
	return 0;
}

static int rk_edp_set_link_train(struct rk_edp *edp)
{
	int retval;

	if (rk_edp_init_training(edp)) {
		printk(BIOS_ERR, "DP LT init failed!\n");
		return -1;
	}

	retval = rk_edp_hw_link_training(edp);

	return retval;
}

static void rk_edp_init_video(struct rk_edp *edp)
{
	u32 val;

	val = VSYNC_DET | VID_FORMAT_CHG | VID_CLK_CHG;
	write32(&edp->regs->common_int_sta_1, val);

	val = CHA_CRI(4) | CHA_CTRL;
	write32(&edp->regs->sys_ctl_2, val);

	val = VID_HRES_TH(2) | VID_VRES_TH(0);
	write32(&edp->regs->video_ctl_8, val);
}

static void rk_edp_config_video_slave_mode(struct rk_edp *edp)
{
	clrbits32(&edp->regs->func_en_1,
			VID_FIFO_FUNC_EN_N | VID_CAP_FUNC_EN_N);
}

static void rk_edp_set_video_cr_mn(struct rk_edp *edp,
				   enum clock_recovery_m_value_type type,
				   u32 m_value,
				   u32 n_value)
{
	u32 val;

	if (type == REGISTER_M) {
		setbits32(&edp->regs->sys_ctl_4, FIX_M_VID);
		val = m_value & 0xff;
		write32(&edp->regs->m_vid_0, val);
		val = (m_value >> 8) & 0xff;
		write32(&edp->regs->m_vid_1, val);
		val = (m_value >> 16) & 0xff;
		write32(&edp->regs->m_vid_2, val);

		val = n_value & 0xff;
		write32(&edp->regs->n_vid_0, val);
		val = (n_value >> 8) & 0xff;
		write32(&edp->regs->n_vid_1, val);
		val = (n_value >> 16) & 0xff;
		write32(&edp->regs->n_vid_2, val);
	} else  {
		clrbits32(&edp->regs->sys_ctl_4, FIX_M_VID);

		write32(&edp->regs->n_vid_0, 0x00);
		write32(&edp->regs->n_vid_1, 0x80);
		write32(&edp->regs->n_vid_2, 0x00);
	}
}

static int rk_edp_is_video_stream_clock_on(struct rk_edp *edp)
{
	u32 val;
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, 100);
	do {
		val = read32(&edp->regs->sys_ctl_1);

		/*must write value to update DET_STA bit status*/
		write32(&edp->regs->sys_ctl_1, val);
		val = read32(&edp->regs->sys_ctl_1);
		if (!(val & DET_STA))
			continue;

		val = read32(&edp->regs->sys_ctl_2);

		/*must write value to update CHA_STA bit status*/
		write32(&edp->regs->sys_ctl_2, val);
		val = read32(&edp->regs->sys_ctl_2);
		if (!(val & CHA_STA))
			return 0;
	} while (!stopwatch_expired(&sw));

	return -1;
}

static int rk_edp_is_video_stream_on(struct rk_edp *edp)
{
	u32 val;
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, 100);
	do {
		val = read32(&edp->regs->sys_ctl_3);

		/*must write value to update STRM_VALID bit status*/
		write32(&edp->regs->sys_ctl_3, val);

		val = read32(&edp->regs->sys_ctl_3);
		if (!(val & STRM_VALID))
			return 0;
	} while (!stopwatch_expired(&sw));

	return -1;
}

static int rk_edp_config_video(struct rk_edp *edp)
{
	rk_edp_config_video_slave_mode(edp);

	if (rk_edp_get_pll_lock_status(edp) == DP_PLL_UNLOCKED) {
		edp_debug("PLL is not locked yet.\n");
		return -1;
	}

	if (rk_edp_is_video_stream_clock_on(edp))
		return -1;

	/* Set to use the register calculated M/N video */
	rk_edp_set_video_cr_mn(edp, CALCULATED_M, 0, 0);

	/* For video bist, Video timing must be generated by register */
	clrbits32(&edp->regs->video_ctl_10, F_SEL);

	/* Disable video mute */
	clrbits32(&edp->regs->video_ctl_1, VIDEO_MUTE);

	return 0;
}

static void rockchip_edp_force_hpd(struct rk_edp *edp)
{
	u32 val;

	val = read32(&edp->regs->sys_ctl_3);
	val |= (F_HPD | HPD_CTRL);
	write32(&edp->regs->sys_ctl_3, val);
}

static int rockchip_edp_get_plug_in_status(struct rk_edp *edp)
{
	u32 val;

	val = read32(&edp->regs->sys_ctl_3);
	if (val & HPD_STATUS)
		return 1;

	return 0;
}

/*
 * support edp HPD function
 * some hardware version do not support edp hdp,
 * we use 360ms to try to get the hpd single now,
 * if we can not get edp hpd single, it will delay 360ms,
 * also meet the edp power timing request, to compatible
 * all of the hardware version
 */
static void rk_edp_wait_hpd(struct rk_edp *edp)
{
	struct stopwatch hpd;

	stopwatch_init_msecs_expire(&hpd, 360);
	do {
		if (rockchip_edp_get_plug_in_status(edp))
			return;
		udelay(100);
	} while (!stopwatch_expired(&hpd));

	printk(BIOS_DEBUG, "do not get hpd single, force hpd\n");
	rockchip_edp_force_hpd(edp);
}

int rk_edp_get_edid(struct edid *edid)
{
	int i;
	int retval;

	/* Read EDID */
	for (i = 0; i < 3; i++) {
		retval = rk_edp_read_edid(&rk_edp, edid);
		if (retval == 0)
			break;
	}

	return retval;
}

int rk_edp_prepare(void)
{
	int ret = 0;

	if (rk_edp_set_link_train(&rk_edp)) {
		printk(BIOS_ERR, "link train failed!\n");
		return -1;
	}

	rk_edp_init_video(&rk_edp);
	ret = rk_edp_config_video(&rk_edp);
	if (ret)
		printk(BIOS_ERR, "config video failed\n");

	return ret;
}

int rk_edp_enable(void)
{
	/* Enable video at next frame */
	setbits32(&rk_edp.regs->video_ctl_1, VIDEO_EN);

	return rk_edp_is_video_stream_on(&rk_edp);
}

void rk_edp_init(void)
{
	rk_edp.regs = (struct rk_edp_regs *)EDP_BASE;

	rk_edp_wait_hpd(&rk_edp);

	rk_edp_init_refclk(&rk_edp);
	rk_edp_init_interrupt(&rk_edp);
	rk_edp_enable_sw_function(&rk_edp);
	rk_edp_init_analog_func(&rk_edp);
	rk_edp_init_aux(&rk_edp);
}
