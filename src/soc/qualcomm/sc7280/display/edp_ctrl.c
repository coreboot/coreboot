// SPDX-License-Identifier: GPL-2.0-only

#include <device/mmio.h>
#include <commonlib/rational.h>
#include <console/console.h>
#include <edid.h>
#include <delay.h>
#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <soc/clock.h>
#include <gpio.h>
#include <soc/display/edp_reg.h>
#include <soc/display/edp_aux.h>
#include <soc/display/edp_ctrl.h>
#include <soc/display/edp_phy.h>

#define DPCD_LINK_VOLTAGE_MAX		4
#define DPCD_LINK_PRE_EMPHASIS_MAX	4
#define MAX_LINK_TRAINING_LOOP		5

/* DP_TX Registers */
#define MAX_16BITS_VALUE				((1 << 16) - 1) /* 16 bits value */
#define EDP_INTR_AUX_I2C_DONE				BIT(3)
#define EDP_INTR_WRONG_ADDR				BIT(6)
#define EDP_INTR_TIMEOUT				BIT(9)
#define EDP_INTR_NACK_DEFER				BIT(12)
#define EDP_INTR_WRONG_DATA_CNT				BIT(15)
#define EDP_INTR_I2C_NACK				BIT(18)
#define EDP_INTR_I2C_DEFER				BIT(21)
#define EDP_INTR_PLL_UNLOCKED				BIT(24)
#define EDP_INTR_AUX_ERROR				BIT(27)
#define EDP_INTR_READY_FOR_VIDEO			BIT(0)
#define EDP_INTR_IDLE_PATTERN_SENT			BIT(3)
#define EDP_INTR_FRAME_END				BIT(6)
#define EDP_INTR_CRC_UPDATED				BIT(9)
#define EDP_INTR_SST_FIFO_UNDERFLOW			BIT(28)
#define REG_EDP_DP_HPD_CTRL				(0x00000000)
#define EDP_DP_HPD_CTRL_HPD_EN				(0x00000001)
#define EDP_DP_HPD_PLUG_INT_ACK				(0x00000001)
#define EDP_DP_IRQ_HPD_INT_ACK				(0x00000002)
#define EDP_DP_HPD_REPLUG_INT_ACK			(0x00000004)
#define EDP_DP_HPD_UNPLUG_INT_ACK			(0x00000008)
#define EDP_DP_HPD_STATE_STATUS_BITS_MASK		(0x0000000F)
#define EDP_DP_HPD_STATE_STATUS_BITS_SHIFT		(0x1C)
#define EDP_DP_HPD_PLUG_INT_MASK			(0x00000001)
#define EDP_DP_IRQ_HPD_INT_MASK				(0x00000002)
#define EDP_DP_HPD_REPLUG_INT_MASK			(0x00000004)
#define EDP_DP_HPD_UNPLUG_INT_MASK			(0x00000008)
#define EDP_DP_HPD_INT_MASK				(EDP_DP_HPD_PLUG_INT_MASK | \
							EDP_DP_IRQ_HPD_INT_MASK | \
							EDP_DP_HPD_REPLUG_INT_MASK | \
							EDP_DP_HPD_UNPLUG_INT_MASK)
#define EDP_DP_HPD_STATE_STATUS_CONNECTED		(0x40000000)
#define EDP_DP_HPD_STATE_STATUS_PENDING			(0x20000000)
#define EDP_DP_HPD_STATE_STATUS_DISCONNECTED		(0x00000000)
#define EDP_DP_HPD_STATE_STATUS_MASK			(0xE0000000)
#define EDP_DP_HPD_REFTIMER_ENABLE			(1 << 16)
#define EDP_DP_HPD_EVENT_TIME_0_VAL			(0x3E800FA)
#define EDP_DP_HPD_EVENT_TIME_1_VAL			(0x1F407D0)
#define EDP_INTERRUPT_TRANS_NUM				(0x000000A0)
#define EDP_MAINLINK_CTRL_ENABLE			(0x00000001)
#define EDP_MAINLINK_CTRL_RESET				(0x00000002)
#define EDP_MAINLINK_CTRL_SW_BYPASS_SCRAMBLER		(0x00000010)
#define EDP_MAINLINK_FB_BOUNDARY_SEL			(0x02000000)
#define EDP_CONFIGURATION_CTRL_SYNC_ASYNC_CLK		(0x00000001)
#define EDP_CONFIGURATION_CTRL_STATIC_DYNAMIC_CN	(0x00000002)
#define EDP_CONFIGURATION_CTRL_P_INTERLACED		(0x00000004)
#define EDP_CONFIGURATION_CTRL_INTERLACED_BTF		(0x00000008)
#define EDP_CONFIGURATION_CTRL_NUM_OF_LANES		(0x00000010)
#define EDP_CONFIGURATION_CTRL_ENHANCED_FRAMING		(0x00000040)
#define EDP_CONFIGURATION_CTRL_SEND_VSC			(0x00000080)
#define EDP_CONFIGURATION_CTRL_BPC			(0x00000100)
#define EDP_CONFIGURATION_CTRL_ASSR			(0x00000400)
#define EDP_CONFIGURATION_CTRL_RGB_YUV			(0x00000800)
#define EDP_CONFIGURATION_CTRL_LSCLK_DIV		(0x00002000)
#define EDP_CONFIGURATION_CTRL_NUM_OF_LANES_SHIFT	(0x04)
#define EDP_CONFIGURATION_CTRL_BPC_SHIFT		(0x08)
#define EDP_CONFIGURATION_CTRL_LSCLK_DIV_SHIFT		(0x0D)
#define EDP_TOTAL_HOR_VER_HORIZ__MASK			(0x0000FFFF)
#define EDP_TOTAL_HOR_VER_HORIZ__SHIFT			(0)
#define DP_EDP_CONFIGURATION_CAP			0x00d   /* XXX 1.2? */
#define DP_ALTERNATE_SCRAMBLER_RESET_CAP		(1 << 0)
#define DP_FRAMING_CHANGE_CAP				(1 << 1)
#define DP_DPCD_DISPLAY_CONTROL_CAPABLE			(1 << 3) /* edp v1.2 or higher */
#define EDP_MISC0_SYNCHRONOUS_CLK			(0x00000001)
#define EDP_MISC0_COLORIMETRY_CFG_SHIFT			(0x00000001)
#define EDP_MISC0_TEST_BITS_DEPTH_SHIFT			(0x00000005)
#define LANE0_MAPPING_SHIFT				(0x00000000)
#define LANE1_MAPPING_SHIFT				(0x00000002)
#define LANE2_MAPPING_SHIFT				(0x00000004)
#define LANE3_MAPPING_SHIFT				(0x00000006)
#define EDP_MAINLINK_READY_FOR_VIDEO			(0x00000001)
#define EDP_MAINLINK_READY_TRAIN_PATTERN_1_READY	(0x00000008)
#define EDP_MAINLINK_SAFE_TO_EXIT_LEVEL_2		(0x00000002)
#define EDP_LINK_BW_MAX					DP_LINK_BW_5_4
#define DP_RECEIVER_CAP_SIZE				0xf
#define DP_LINK_STATUS_SIZE				6
#define DP_TRAINING_AUX_RD_MASK				0x7F    /* DP 1.3 */

/* AUX CH addresses */
/* DPCD */
#define DP_DPCD_REV				0x000
#define DP_DPCD_REV_10				0x10
#define DP_DPCD_REV_11				0x11
#define DP_DPCD_REV_12				0x12
#define DP_DPCD_REV_13				0x13
#define DP_DPCD_REV_14				0x14
#define DP_SET_POWER				0x600
#define DP_SET_POWER_D0				0x1
#define DP_SET_POWER_D3				0x2
#define DP_SET_POWER_MASK			0x3
#define DP_MAX_LINK_RATE			0x001
#define DP_MAX_LANE_COUNT			0x002
#define DP_MAX_LANE_COUNT_MASK			0x1f
#define DP_TPS3_SUPPORTED			(1 << 6)
#define DP_ENHANCED_FRAME_CAP			(1 << 7)
#define DP_MAX_DOWNSPREAD			0x003
#define DP_NO_AUX_HANDSHAKE_LINK_TRAINING	(1 << 6)
#define DP_NORP					0x004
#define DP_DOWNSTREAMPORT_PRESENT		0x005
#define DP_DWN_STRM_PORT_PRESENT		(1 << 0)
#define DP_DWN_STRM_PORT_TYPE_MASK		0x06
#define DP_FORMAT_CONVERSION			(1 << 3)
#define DP_MAIN_LINK_CHANNEL_CODING		0x006
#define DP_EDP_CONFIGURATION_CAP		0x00d
#define DP_TRAINING_AUX_RD_INTERVAL		0x00e

/* link configuration */
#define	DP_LINK_BW_SET				0x100
#define DP_LINK_RATE_TABLE			0x00    /* eDP 1.4 */
#define DP_LINK_BW_1_62				0x06
#define DP_LINK_BW_2_7				0x0a
#define DP_LINK_BW_5_4				0x14    /* 1.2 */
#define DP_LINK_BW_8_1				0x1e    /* 1.4 */
#define DP_LANE_COUNT_SET			0x101
#define DP_LANE_COUNT_MASK			0x0f
#define DP_LANE_COUNT_ENHANCED_FRAME_EN		(1 << 7)
#define DP_TRAINING_PATTERN_SET			0x102
#define DP_TRAINING_PATTERN_DISABLE		0
#define DP_TRAINING_PATTERN_1			1
#define DP_TRAINING_PATTERN_2			2
#define DP_TRAINING_PATTERN_3			3
#define DP_TRAINING_PATTERN_MASK		0x3
#define DP_LINK_QUAL_PATTERN_DISABLE		(0 << 2)
#define DP_LINK_QUAL_PATTERN_D10_2		(1 << 2)
#define DP_LINK_QUAL_PATTERN_ERROR_RATE		(2 << 2)
#define DP_LINK_QUAL_PATTERN_PRBS7		(3 << 2)
#define DP_LINK_QUAL_PATTERN_MASK		(3 << 2)
#define DP_RECOVERED_CLOCK_OUT_EN		(1 << 4)
#define DP_LINK_SCRAMBLING_DISABLE		(1 << 5)
#define DP_EDP_CONFIGURATION_SET		0x10a
#define DP_ALTERNATE_SCRAMBLER_RESET_ENABLE	(1 << 0)
#define DP_FRAMING_CHANGE_ENABLE		(1 << 1)
#define DP_PANEL_SELF_TEST_ENABLE		(1 << 7)
#define DP_SYMBOL_ERROR_COUNT_BOTH		(0 << 6)
#define DP_SYMBOL_ERROR_COUNT_DISPARITY		(1 << 6)
#define DP_SYMBOL_ERROR_COUNT_SYMBOL		(2 << 6)
#define DP_SYMBOL_ERROR_COUNT_MASK		(3 << 6)
#define DP_TRAINING_LANE0_SET			0x103
#define DP_TRAINING_LANE1_SET			0x104
#define DP_TRAINING_LANE2_SET			0x105
#define DP_TRAINING_LANE3_SET			0x106

#define DP_TRAIN_VOLTAGE_SWING_MASK		0x3
#define DP_TRAIN_VOLTAGE_SWING_SHIFT		0
#define DP_TRAIN_MAX_SWING_REACHED		(1 << 2)
#define DP_TRAIN_VOLTAGE_SWING_400		(0 << 0)
#define DP_TRAIN_VOLTAGE_SWING_600		(1 << 0)
#define DP_TRAIN_VOLTAGE_SWING_800		(2 << 0)
#define DP_TRAIN_VOLTAGE_SWING_1200		(3 << 0)
#define DP_TRAIN_PRE_EMPHASIS_MASK		(3 << 3)
#define DP_TRAIN_PRE_EMPHASIS_0			(0 << 3)
#define DP_TRAIN_PRE_EMPHASIS_3_5		(1 << 3)
#define DP_TRAIN_PRE_EMPHASIS_6			(2 << 3)
#define DP_TRAIN_PRE_EMPHASIS_9_5		(3 << 3)
#define DP_TRAIN_PRE_EMPHASIS_SHIFT		3
#define DP_TRAIN_MAX_PRE_EMPHASIS_REACHED	(1 << 5)
#define DP_DOWNSPREAD_CTRL			0x107
#define DP_SPREAD_AMP_0_5			(1 << 4)
#define DP_MAIN_LINK_CHANNEL_CODING_SET		0x108
#define DP_SET_ANSI_8B10B			(1 << 0)
#define DP_LANE0_1_STATUS			0x202
#define DP_LANE2_3_STATUS			0x203
#define DP_LANE_CR_DONE				(1 << 0)
#define DP_LANE_CHANNEL_EQ_DONE			(1 << 1)
#define DP_LANE_SYMBOL_LOCKED			(1 << 2)
#define DP_CHANNEL_EQ_BITS			(DP_LANE_CR_DONE | \
						DP_LANE_CHANNEL_EQ_DONE | \
						DP_LANE_SYMBOL_LOCKED)
#define DP_LANE_ALIGN_STATUS_UPDATED		0x204
#define DP_INTERLANE_ALIGN_DONE			(1 << 0)
#define DP_DOWNSTREAM_PORT_STATUS_CHANGED	(1 << 6)
#define DP_LINK_STATUS_UPDATED			(1 << 7)
#define DP_ADJUST_REQUEST_LANE0_1		0x206
#define DP_ADJUST_REQUEST_LANE2_3		0x207
#define DP_ADJUST_VOLTAGE_SWING_LANE0_MASK	0x03
#define DP_ADJUST_VOLTAGE_SWING_LANE0_SHIFT	0
#define DP_ADJUST_PRE_EMPHASIS_LANE0_MASK	0x0c
#define DP_ADJUST_PRE_EMPHASIS_LANE0_SHIFT	2
#define DP_ADJUST_VOLTAGE_SWING_LANE1_MASK	0x30
#define DP_ADJUST_VOLTAGE_SWING_LANE1_SHIFT	4
#define DP_ADJUST_PRE_EMPHASIS_LANE1_MASK	0xc0
#define DP_ADJUST_PRE_EMPHASIS_LANE1_SHIFT	6
#define DP_TEST_REQUEST				0x218
#define DP_TEST_LINK_TRAINING			(1 << 0)
#define DP_TEST_LINK_PATTERN			(1 << 1)
#define DP_TEST_LINK_EDID_READ			(1 << 2)
#define DP_TEST_LINK_PHY_TEST_PATTERN		(1 << 3) /* DPCD >= 1.1 */
#define DP_TEST_LINK_RATE			0x219
#define DP_LINK_RATE_162			(0x6)
#define DP_LINK_RATE_27				(0xa)

#define DP_TEST_LANE_COUNT			0x220
#define DP_TEST_PATTERN				0x221
#define DP_TEST_RESPONSE			0x260
#define DP_TEST_ACK				(1 << 0)
#define DP_TEST_NAK				(1 << 1)
#define DP_TEST_EDID_CHECKSUM_WRITE		(1 << 2)

#define DP_SET_POWER				0x600
#define DP_SET_POWER_D0				0x1
#define DP_SET_POWER_D3				0x2

/* Link training return value */
#define EDP_TRAIN_FAIL		-1
#define EDP_TRAIN_SUCCESS	0
#define EDP_TRAIN_RECONFIG	1

#define EDP_INTERRUPT_STATUS_ACK_SHIFT	1
#define EDP_INTERRUPT_STATUS_MASK_SHIFT	2

#define EDP_INTERRUPT_STATUS1 \
	(EDP_INTR_AUX_I2C_DONE| \
	EDP_INTR_WRONG_ADDR | EDP_INTR_TIMEOUT | \
	EDP_INTR_NACK_DEFER | EDP_INTR_WRONG_DATA_CNT | \
	EDP_INTR_I2C_NACK | EDP_INTR_I2C_DEFER | \
	EDP_INTR_PLL_UNLOCKED | EDP_INTR_AUX_ERROR)

#define EDP_INTERRUPT_STATUS1_ACK \
	(EDP_INTERRUPT_STATUS1 << EDP_INTERRUPT_STATUS_ACK_SHIFT)
#define EDP_INTERRUPT_STATUS1_MASK \
	(EDP_INTERRUPT_STATUS1 << EDP_INTERRUPT_STATUS_MASK_SHIFT)

#define EDP_INTERRUPT_STATUS2 \
	(EDP_INTR_READY_FOR_VIDEO | EDP_INTR_IDLE_PATTERN_SENT | \
	EDP_INTR_FRAME_END | EDP_INTR_CRC_UPDATED | EDP_INTR_SST_FIFO_UNDERFLOW)

#define EDP_INTERRUPT_STATUS2_ACK \
	(EDP_INTERRUPT_STATUS2 << EDP_INTERRUPT_STATUS_ACK_SHIFT)
#define EDP_INTERRUPT_STATUS2_MASK \
	(EDP_INTERRUPT_STATUS2 << EDP_INTERRUPT_STATUS_MASK_SHIFT)

enum edp_color_depth {
	EDP_6BIT = 0,
	EDP_8BIT = 1,
	EDP_10BIT = 2,
	EDP_12BIT = 3,
	EDP_16BIT = 4,
};

struct tu_algo_data {
	int64_t lclk_fp;
	int64_t lclk;
	int64_t pclk;
	int64_t pclk_fp;
	int64_t lwidth;
	int64_t lwidth_fp;
	int64_t hbp_relative_to_pclk;
	int64_t hbp_relative_to_pclk_fp;
	int nlanes;
	int bpp;
	int async_en;
	int bpc;
	int delay_start_link_extra_pclk;
	int extra_buffer_margin;
	int64_t ratio_fp;
	int64_t original_ratio_fp;
	int64_t err_fp;
	int64_t old_err_fp;
	int64_t new_err_fp;
	int tu_size;
	int tu_size_desired;
	int tu_size_minus1;
	int valid_boundary_link;
	int64_t resulting_valid_fp;
	int64_t total_valid_fp;
	int64_t effective_valid_fp;
	int64_t effective_valid_recorded_fp;
	int num_tus;
	int num_tus_per_lane;
	int paired_tus;
	int remainder_tus;
	int remainder_tus_upper;
	int remainder_tus_lower;
	int extra_bytes;
	int filler_size;
	int delay_start_link;
	int extra_pclk_cycles;
	int extra_pclk_cycles_in_link_clk;
	int64_t ratio_by_tu_fp;
	int64_t average_valid2_fp;
	int new_valid_boundary_link;
	int remainder_symbols_exist;
	int n_symbols;
	int64_t n_remainder_symbols_per_lane_fp;
	int64_t last_partial_tu_fp;
	int64_t TU_ratio_err_fp;
	int n_tus_incl_last_incomplete_tu;
	int extra_pclk_cycles_tmp;
	int extra_pclk_cycles_in_link_clk_tmp;
	int extra_required_bytes_new_tmp;
	int filler_size_tmp;
	int lower_filler_size_tmp;
	int delay_start_link_tmp;
	bool boundary_moderation_en;
	int boundary_mod_lower_err;
	int upper_boundary_count;
	int lower_boundary_count;
	int i_upper_boundary_count;
	int i_lower_boundary_count;
	int valid_lower_boundary_link;
	int even_distribution_BF;
	int even_distribution_legacy;
	int even_distribution;
	int min_hblank_violated;
	int64_t delay_start_time_fp;
	int64_t hbp_time_fp;
	int64_t hactive_time_fp;
	int64_t diff_abs_fp;
	int64_t ratio;
};

struct edp_ctrl {
	/* Link status */
	uint32_t link_rate_khz;
	uint8_t link_rate;
	uint32_t lane_cnt;
	uint8_t v_level;
	uint8_t p_level;

	/* Timing status */
	uint32_t pixel_rate; /* in kHz */
	uint32_t color_depth;
};

struct edp_ctrl_tu {
	uint32_t tu_size_minus1;		/* Desired TU Size */
	uint32_t valid_boundary_link;		/* Upper valid boundary */
	uint32_t delay_start_link;		/* # of clock cycles to delay */
	bool boundary_moderation_en;		/* Enable boundary Moderation? */
	uint32_t valid_lower_boundary_link;	/* Valid lower boundary link */
	uint32_t upper_boundary_count;		/* Upper boundary Count */
	uint32_t lower_boundary_count;		/* Lower boundary Count */
};

static uint8_t dp_get_lane_status(const uint8_t link_status[DP_LINK_STATUS_SIZE], int lane)
{
	int i = DP_LANE0_1_STATUS + (lane >> 1);
	int s = (lane & 1) * 4;
	uint8_t l = link_status[i - DP_LANE0_1_STATUS];
	return (l >> s) & 0xf;
}


static uint8_t edp_get_adjust_request_voltage(
				const uint8_t link_status[DP_LINK_STATUS_SIZE],
				int lane)
{
	int i = DP_ADJUST_REQUEST_LANE0_1 + (lane >> 1);
	int s = ((lane & 1) ?
		 DP_ADJUST_VOLTAGE_SWING_LANE1_SHIFT :
		 DP_ADJUST_VOLTAGE_SWING_LANE0_SHIFT);
	uint8_t l = link_status[i - DP_LANE0_1_STATUS];

	return ((l >> s) & 0x3) << DP_TRAIN_VOLTAGE_SWING_SHIFT;
}

static uint8_t edp_get_adjust_request_pre_emphasis(
					const uint8_t link_status[DP_LINK_STATUS_SIZE],
					int lane)
{
	int i = DP_ADJUST_REQUEST_LANE0_1 + (lane >> 1);
	int s = ((lane & 1) ?
		 DP_ADJUST_PRE_EMPHASIS_LANE1_SHIFT :
		 DP_ADJUST_PRE_EMPHASIS_LANE0_SHIFT);
	uint8_t l = link_status[i - DP_LANE0_1_STATUS];

	return ((l >> s) & 0x3) << DP_TRAIN_PRE_EMPHASIS_SHIFT;
}

static void edp_link_train_clock_recovery_delay(const uint8_t dpcd[DP_RECEIVER_CAP_SIZE])
{
	int rd_interval = dpcd[DP_TRAINING_AUX_RD_INTERVAL] & DP_TRAINING_AUX_RD_MASK;

	if (rd_interval > 4)
		printk(BIOS_ERR, "AUX interval %d, out of range (max 4)\n", rd_interval);

	/*
	 * The DPCD stores the AUX read interval in units of 4 ms.
	 * for DP v1.4 and above, clock recovery should use 100 us for AUX read intervals.
	 */
	if (rd_interval == 0 || dpcd[DP_DPCD_REV] >= DP_DPCD_REV_14)
		udelay(100);
	else
		mdelay(rd_interval * 4);
}

static bool edp_clock_recovery_ok(const uint8_t link_status[DP_LINK_STATUS_SIZE],
				  int lane_count)
{
	int lane;
	uint8_t lane_status;

	for (lane = 0; lane < lane_count; lane++) {
		lane_status = dp_get_lane_status(link_status, lane);
		if ((lane_status & DP_LANE_CR_DONE) == 0) {
			printk(BIOS_ERR, "clock recovery ok failed : %x\n", lane_status);
			return false;
		}
	}
	return true;
}

static void edp_link_train_channel_eq_delay(const uint8_t dpcd[DP_RECEIVER_CAP_SIZE])
{
	int rd_interval = dpcd[DP_TRAINING_AUX_RD_INTERVAL] &
			  DP_TRAINING_AUX_RD_MASK;

	if (rd_interval > 4)
		printk(BIOS_INFO, "AUX interval %d, out of range (max 4)\n",
			      rd_interval);

	/*
	 * The DPCD stores the AUX read interval in units of 4 ms.
	 * if the TRAINING_AUX_RD_INTERVAL field is 0, the channel equalization
	 * should use 400 us AUX read intervals.
	 */
	if (rd_interval == 0)
		udelay(400);
	else
		mdelay(rd_interval * 4);
}

static bool edp_channel_eq_ok(const uint8_t link_status[DP_LINK_STATUS_SIZE], int lane_count)
{
	uint8_t lane_align;
	uint8_t lane_status;
	int lane;

	lane_align = link_status[DP_LANE_ALIGN_STATUS_UPDATED - DP_LANE0_1_STATUS];
	if ((lane_align & DP_INTERLANE_ALIGN_DONE) == 0)
		return false;

	for (lane = 0; lane < lane_count; lane++) {
		lane_status = dp_get_lane_status(link_status, lane);
		if ((lane_status & DP_CHANNEL_EQ_BITS) != DP_CHANNEL_EQ_BITS)
			return false;
	}
	return true;
}

static void edp_ctrl_irq_enable(int enable)
{
	if (enable) {
		write32(&edp_ahbclk->interrupt_status, EDP_INTERRUPT_STATUS1_MASK);
		write32(&edp_ahbclk->interrupt_status2,
			EDP_INTERRUPT_STATUS2_MASK);
	} else {
		write32(&edp_ahbclk->interrupt_status,
			EDP_INTERRUPT_STATUS1_ACK);
		write32(&edp_ahbclk->interrupt_status2,
			EDP_INTERRUPT_STATUS2_ACK);
	}
}

static void edp_config_ctrl(struct edp_ctrl *ctrl, uint8_t *dpcd)
{
	uint32_t config = 0, depth = 0;

	/* Default-> LSCLK DIV: 1/4 LCLK  */
	config |= (2 << EDP_CONFIGURATION_CTRL_LSCLK_DIV_SHIFT);

	/* Scrambler reset enable */
	if (dpcd[DP_EDP_CONFIGURATION_CAP] & DP_ALTERNATE_SCRAMBLER_RESET_CAP)
		config |= EDP_CONFIGURATION_CTRL_ASSR;

	if (ctrl->color_depth == 8)
		depth = EDP_8BIT;
	else if (ctrl->color_depth == 10)
		depth = EDP_10BIT;
	else if (ctrl->color_depth == 12)
		depth = EDP_12BIT;
	else if (ctrl->color_depth == 16)
		depth = EDP_16BIT;
	config |= depth << EDP_CONFIGURATION_CTRL_BPC_SHIFT;

	/* Num of Lanes */
	config |= ((ctrl->lane_cnt - 1)
			<< EDP_CONFIGURATION_CTRL_NUM_OF_LANES_SHIFT);

	if (dpcd[DP_DPCD_REV] >= 0x11 &&
		(dpcd[DP_MAX_LANE_COUNT] & DP_ENHANCED_FRAME_CAP))
		config |= EDP_CONFIGURATION_CTRL_ENHANCED_FRAMING;

	config |= EDP_CONFIGURATION_CTRL_P_INTERLACED; /* progressive video */

	/* sync clock & static Mvid */
	config |= EDP_CONFIGURATION_CTRL_STATIC_DYNAMIC_CN;
	config |= EDP_CONFIGURATION_CTRL_SYNC_ASYNC_CLK;

	write32(&edp_lclk->configuration_ctrl, config);
}

static void edp_state_ctrl(uint32_t state)
{
	write32(&edp_lclk->state_ctrl, state);
}

static int edp_lane_set_write(uint8_t voltage_level, uint8_t pre_emphasis_level)
{
	int i;
	uint8_t buf[4];

	if (voltage_level >= DPCD_LINK_VOLTAGE_MAX)
		voltage_level |= 0x04;

	if (pre_emphasis_level >= DPCD_LINK_PRE_EMPHASIS_MAX)
		pre_emphasis_level |= 0x20;

	for (i = 0; i < 4; i++)
		buf[i] = voltage_level | pre_emphasis_level;

	if (edp_aux_transfer(DP_TRAINING_LANE0_SET, DP_AUX_NATIVE_WRITE, buf, 2) < 2) {
		printk(BIOS_ERR, "%s: Set sw/pe to panel failed\n", __func__);
		return -1;
	}

	return 0;
}

static int edp_train_pattern_set_write(uint8_t pattern)
{
	uint8_t p = pattern;

	printk(BIOS_INFO, "pattern=%x\n", p);
	if (edp_aux_transfer(DP_TRAINING_PATTERN_SET, DP_AUX_NATIVE_WRITE, &p, 1) < 1) {
		printk(BIOS_ERR, "%s: Set training pattern to panel failed\n", __func__);
		return -1;
	}

	return 0;
}

static void edp_sink_train_set_adjust(struct edp_ctrl *ctrl,
	const uint8_t *link_status)
{
	int i;
	uint8_t max = 0;
	uint8_t data;

	/* use the max level across lanes */
	for (i = 0; i < ctrl->lane_cnt; i++) {
		data = edp_get_adjust_request_voltage(link_status, i);
		printk(BIOS_INFO, "lane=%d req_voltage_swing=0x%x\n", i, data);
		if (max < data)
			max = data;
	}

	ctrl->v_level = max >> DP_TRAIN_VOLTAGE_SWING_SHIFT;

	/* use the max level across lanes */
	max = 0;
	for (i = 0; i < ctrl->lane_cnt; i++) {
		data = edp_get_adjust_request_pre_emphasis(link_status, i);
		printk(BIOS_INFO, "lane=%d req_pre_emphasis=0x%x\n", i, data);
		if (max < data)
			max = data;
	}

	ctrl->p_level = max >> DP_TRAIN_PRE_EMPHASIS_SHIFT;
	printk(BIOS_INFO, "v_level=%d, p_level=%d\n", ctrl->v_level, ctrl->p_level);
}

static void edp_host_train_set(uint32_t train)
{
	int cnt = 10;
	uint32_t data = 0;
	uint32_t shift = train - 1;

	printk(BIOS_INFO, "train=%d", train);

	edp_state_ctrl(SW_LINK_TRAINING_PATTERN1 << shift);
	while (--cnt) {
		data = read32(&edp_lclk->mainlink_ready);
		if (data & (EDP_MAINLINK_READY_TRAIN_PATTERN_1_READY << shift))
			break;
	}

	if (cnt == 0)
		printk(BIOS_INFO, "%s: set link_train=%u failed\n", __func__, data);
}

static int edp_voltage_pre_emphasis_set(struct edp_ctrl *ctrl)
{
	printk(BIOS_INFO, "v=%d p=%d\n", ctrl->v_level, ctrl->p_level);

	edp_phy_config(ctrl->v_level, ctrl->p_level);
	return edp_lane_set_write(ctrl->v_level, ctrl->p_level);
}

static int edp_start_link_train_1(struct edp_ctrl *ctrl, uint8_t *dpcd)
{
	uint8_t link_status[DP_LINK_STATUS_SIZE];
	uint8_t old_v_level;
	int tries;
	int ret, rlen;

	edp_state_ctrl(0);
	edp_host_train_set(DP_TRAINING_PATTERN_1);

	ret = edp_train_pattern_set_write(DP_TRAINING_PATTERN_1 | DP_LINK_SCRAMBLING_DISABLE);
	if (ret)
		return ret;

	ret = edp_voltage_pre_emphasis_set(ctrl);
	if (ret)
		return ret;

	tries = 0;
	old_v_level = ctrl->v_level;
	while (1) {
		edp_link_train_clock_recovery_delay(dpcd);

		rlen = edp_aux_transfer(DP_LANE0_1_STATUS, DP_AUX_NATIVE_READ,
					&link_status, DP_LINK_STATUS_SIZE);
		if (rlen < DP_LINK_STATUS_SIZE) {
			printk(BIOS_ERR, "%s: read link status failed\n", __func__);
			return -1;
		}

		if (edp_clock_recovery_ok(link_status, ctrl->lane_cnt)) {
			ret = 0;
			break;
		}

		if (ctrl->v_level == DPCD_LINK_VOLTAGE_MAX) {
			ret = -1;
			break;
		}

		if (old_v_level != ctrl->v_level) {
			tries++;
			if (tries >= 5) {
				ret = -1;
				break;
			}
		} else {
			tries = 0;
			old_v_level = ctrl->v_level;
		}

		edp_sink_train_set_adjust(ctrl, link_status);
		ret = edp_voltage_pre_emphasis_set(ctrl);
		if (ret)
			return ret;
	}

	return 0;
}

static int edp_start_link_train_2(struct edp_ctrl *ctrl, uint8_t *dpcd)
{
	uint8_t link_status[DP_LINK_STATUS_SIZE];
	int tries = 0;
	int ret, rlen;

	edp_host_train_set(DP_TRAINING_PATTERN_2);
	ret = edp_voltage_pre_emphasis_set(ctrl);
	if (ret)
		return ret;

	ret = edp_train_pattern_set_write(DP_TRAINING_PATTERN_2 | DP_RECOVERED_CLOCK_OUT_EN);
	if (ret)
		return ret;

	while (1) {
		edp_link_train_channel_eq_delay(dpcd);

		rlen = edp_aux_transfer(DP_LANE0_1_STATUS, DP_AUX_NATIVE_READ,
					&link_status, DP_LINK_STATUS_SIZE);
		if (rlen < DP_LINK_STATUS_SIZE) {
			printk(BIOS_ERR, "%s: read link status failed\n", __func__);
			return -1;
		}

		if (edp_channel_eq_ok(link_status, ctrl->lane_cnt)) {
			ret = 0;
			break;
		}

		tries++;
		if (tries > 10) {
			ret = -1;
			break;
		}

		edp_sink_train_set_adjust(ctrl, link_status);
		ret = edp_voltage_pre_emphasis_set(ctrl);
		if (ret)
			return ret;
	}

	return ret;
}

static int edp_link_rate_down_shift(struct edp_ctrl *ctrl, uint8_t *dpcd)
{
	int ret = 0;
	int link_rate = ctrl->link_rate_khz;

	switch (link_rate) {
	case 810000:
		link_rate = 540000;
		break;
	case 540000:
		link_rate = 270000;
		break;
	case 270000:
		link_rate = 162000;
		break;
	case 162000:
	default:
		ret = -1;
		break;
	}

	if (!ret) {
		ctrl->link_rate_khz = link_rate;
		ctrl->link_rate = link_rate / 27000;
		printk(BIOS_INFO, "new rate=0x%x\n", ctrl->link_rate_khz);
	}

	return ret;
}

static bool edp_clock_recovery_reduced_lanes(
			const uint8_t link_status[DP_LINK_STATUS_SIZE], uint32_t lane_cnt)
{
	int reduced_lanes = 0;

	if (lane_cnt <= 1)
		return -1;

	reduced_lanes = lane_cnt >> 1;

	return edp_clock_recovery_ok(link_status, reduced_lanes);
}

static int edp_link_lane_down_shift(struct edp_ctrl *ctrl, uint8_t *dpcd)
{

	if (ctrl->lane_cnt <= 1)
		return -1;

	ctrl->lane_cnt = ctrl->lane_cnt >> 1;
	ctrl->link_rate_khz = dpcd[DP_MAX_LINK_RATE] * 27000;
	ctrl->link_rate = dpcd[DP_MAX_LINK_RATE];
	ctrl->p_level = 0;
	ctrl->v_level = 0;

	return 0;
}

static int edp_clear_training_pattern(uint8_t *dpcd)
{
	int ret;

	ret = edp_train_pattern_set_write(0);
	edp_link_train_channel_eq_delay(dpcd);

	return ret;
}

static int edp_do_link_train(struct edp_ctrl *ctrl, uint8_t *dpcd)
{
	uint8_t values[2], edp_config = 0;
	int ret;
	int rlen;
	uint8_t link_status[DP_LINK_STATUS_SIZE];

	/*
	 * Set the current link rate and lane cnt to panel. They may have been
	 * adjusted and the values are different from them in DPCD CAP
	 */

	values[0] = ctrl->link_rate;
	values[1] = ctrl->lane_cnt;

	if (dpcd[DP_DPCD_REV] >= 0x11 &&
	    (dpcd[DP_MAX_LANE_COUNT] & DP_ENHANCED_FRAME_CAP))
		values[1] |= DP_LANE_COUNT_ENHANCED_FRAME_EN;

	if (edp_aux_transfer(DP_LINK_BW_SET, DP_AUX_NATIVE_WRITE, &values[0], 1) < 0)
		return EDP_TRAIN_FAIL;

	edp_aux_transfer(DP_LANE_COUNT_SET, DP_AUX_NATIVE_WRITE, &values[1], 1);
	ctrl->v_level = 0; /* start from default level */
	ctrl->p_level = 0;

	values[0] = 0x10;

	if (dpcd[DP_MAX_DOWNSPREAD] & 1)
		values[0] = DP_SPREAD_AMP_0_5;

	values[1] = 1;
	edp_aux_transfer(DP_DOWNSPREAD_CTRL, DP_AUX_NATIVE_WRITE, &values[0], 1);
	edp_aux_transfer(DP_MAIN_LINK_CHANNEL_CODING_SET, DP_AUX_NATIVE_WRITE, &values[1], 1);

	ret = edp_start_link_train_1(ctrl, dpcd);
	if (ret < 0) {
		rlen = edp_aux_transfer(DP_LANE0_1_STATUS, DP_AUX_NATIVE_READ,
					&link_status, DP_LINK_STATUS_SIZE);
		if (rlen < DP_LINK_STATUS_SIZE) {
			printk(BIOS_ERR, "%s: read link status failed\n", __func__);
			return -1;
		}

		ret = edp_link_rate_down_shift(ctrl, dpcd);
		if (!ret) {
			printk(BIOS_ERR, "link reconfig\n");
			ret = EDP_TRAIN_RECONFIG;
		} else if (ret < 0) {
			if (edp_clock_recovery_reduced_lanes(link_status,
							     ctrl->lane_cnt) == 0) {
				if (edp_link_lane_down_shift(ctrl, dpcd) < 0) {
					printk(BIOS_ERR, "%s: Training 1 failed\n", __func__);
					ret = EDP_TRAIN_FAIL;
				} else {
					printk(BIOS_ERR, "link reconfig\n");
					ret = EDP_TRAIN_RECONFIG;
				}
			} else {
				printk(BIOS_ERR, "%s: Training 1 failed\n", __func__);
				ret = EDP_TRAIN_FAIL;
			}
		}
		edp_clear_training_pattern(dpcd);
		return ret;
	}

	printk(BIOS_INFO, "Training 1 completed successfully\n");
	edp_state_ctrl(0);
	if (edp_clear_training_pattern(dpcd))
		return EDP_TRAIN_FAIL;

	ret = edp_start_link_train_2(ctrl, dpcd);
	if (ret < 0) {
		rlen = edp_aux_transfer(DP_LANE0_1_STATUS, DP_AUX_NATIVE_READ,
					&link_status, DP_LINK_STATUS_SIZE);
		if (rlen < DP_LINK_STATUS_SIZE) {
			printk(BIOS_ERR, "%s: read link status failed\n", __func__);
			return -1;
		}

		if (edp_clock_recovery_ok(link_status, ctrl->lane_cnt)) {
			if (edp_link_rate_down_shift(ctrl, dpcd) == 0) {
				printk(BIOS_ERR, "link reconfig\n");
				ret = EDP_TRAIN_RECONFIG;
			} else {
				printk(BIOS_ERR, "%s: Training 2 failed\n", __func__);
				ret = EDP_TRAIN_FAIL;
			}
		} else {
			if (edp_link_lane_down_shift(ctrl, dpcd) < 0) {
				printk(BIOS_ERR, "%s: Training 1 failed\n", __func__);
				ret = EDP_TRAIN_FAIL;
			} else {
				printk(BIOS_ERR, "link reconfig\n");
				ret = EDP_TRAIN_RECONFIG;
			}
		}

		edp_clear_training_pattern(dpcd);
		return ret;
	}

	printk(BIOS_INFO, "Training 2 completed successfully\n");
	edp_config = DP_ALTERNATE_SCRAMBLER_RESET_ENABLE;
	edp_aux_transfer(DP_EDP_CONFIGURATION_SET, DP_AUX_NATIVE_WRITE, &edp_config, 1);

	return ret;
}

static void edp_ctrl_config_misc(struct edp_ctrl *ctrl)
{
	uint32_t misc_val;
	enum edp_color_depth depth = EDP_8BIT;

	misc_val = read32(&edp_lclk->misc1_misc0);
	if (ctrl->color_depth == 8)
		depth = EDP_8BIT;
	else if (ctrl->color_depth == 10)
		depth = EDP_10BIT;
	else if (ctrl->color_depth == 12)
		depth = EDP_12BIT;
	else if (ctrl->color_depth == 16)
		depth = EDP_16BIT;

	/* clear bpp bits */
	misc_val &= ~(0x07 << EDP_MISC0_TEST_BITS_DEPTH_SHIFT);
	misc_val |= depth << EDP_MISC0_TEST_BITS_DEPTH_SHIFT;

	/* Configure clock to synchronous mode */
	misc_val |= EDP_MISC0_SYNCHRONOUS_CLK;
	write32(&edp_lclk->misc1_misc0, misc_val);
}

static int edp_ctrl_pixel_clock_dividers(struct edp_ctrl *ctrl,
					  uint32_t *pixel_m, uint32_t *pixel_n)
{
	uint32_t pixel_div = 0, dispcc_input_rate;
	unsigned long den, num;
	uint8_t rate = ctrl->link_rate;
	uint32_t stream_rate_khz = ctrl->pixel_rate;

	if (rate == DP_LINK_BW_8_1) {
		pixel_div = 6;
	} else if (rate == DP_LINK_BW_1_62 || rate == DP_LINK_BW_2_7) {
		pixel_div = 2;
	} else if (rate == DP_LINK_BW_5_4) {
		pixel_div = 4;
	} else {
		printk(BIOS_ERR, "Invalid pixel mux divider\n");
		return -1;
	}

	dispcc_input_rate = (ctrl->link_rate_khz * 10) / pixel_div;

	rational_best_approximation(dispcc_input_rate, stream_rate_khz,
				    (unsigned long)(1 << 16) - 1,
				    (unsigned long)(1 << 16) - 1, &den, &num);
	*pixel_m = num;
	*pixel_n = den;

	return 0;
}

static int edp_ctrl_config_msa(struct edp_ctrl *ctrl)
{
	uint32_t pixel_m, pixel_n;
	uint32_t mvid, nvid;
	u32 const nvid_fixed = 0x8000;
	uint8_t rate = ctrl->link_rate;

	if (edp_ctrl_pixel_clock_dividers(ctrl, &pixel_m, &pixel_n) < 0)
		return -1;

	pixel_n = ~(pixel_n - pixel_m);
	pixel_n = pixel_n & 0xFFFF;
	mvid = (pixel_m & 0xFFFF) * 5;
	nvid = (0xFFFF & (~pixel_n)) + (pixel_m & 0xFFFF);

	if (nvid < nvid_fixed) {
		u32 temp;

		temp = (nvid_fixed / nvid) * nvid;
		mvid = (nvid_fixed / nvid) * mvid;
		nvid = temp;
	}

	if (rate == DP_LINK_BW_5_4)
		nvid *= 2;

	if (rate == DP_LINK_BW_8_1)
		nvid *= 3;

	printk(BIOS_INFO, "mvid=0x%x, nvid=0x%x\n", mvid, nvid);
	write32(&edp_lclk->software_mvid, mvid);
	write32(&edp_lclk->software_nvid, nvid);
	write32(&edp_p0clk->dsc_dto, 0x0);

	return 0;
}

static void tu_valid_boundary_calc(struct tu_algo_data *tu)
{
	int64_t f = 100000;

	tu->new_valid_boundary_link = DIV_ROUND_UP(tu->tu_size * tu->ratio_fp, f);
	tu->average_valid2_fp = ((tu->i_upper_boundary_count * tu->new_valid_boundary_link +
				 tu->i_lower_boundary_count  *
				 (tu->new_valid_boundary_link - 1)) * f) /
				 (tu->i_upper_boundary_count + tu->i_lower_boundary_count);
	tu->num_tus = ((tu->bpp * tu->lwidth_fp) / 8) / tu->average_valid2_fp;
	tu->n_remainder_symbols_per_lane_fp = ((tu->n_symbols * f) -
					       (tu->num_tus * tu->average_valid2_fp)) /
						tu->nlanes;
	tu->last_partial_tu_fp = (tu->n_remainder_symbols_per_lane_fp / tu->tu_size);
	if (tu->n_remainder_symbols_per_lane_fp != 0)
		tu->remainder_symbols_exist = 1;
	else
		tu->remainder_symbols_exist = 0;

	tu->num_tus_per_lane = tu->num_tus / tu->nlanes;
	tu->paired_tus = (int)((tu->num_tus_per_lane) / (tu->i_upper_boundary_count +
			       tu->i_lower_boundary_count));
	tu->remainder_tus = tu->num_tus_per_lane - tu->paired_tus *
			    (tu->i_upper_boundary_count + tu->i_lower_boundary_count);
	if ((tu->remainder_tus - tu->i_upper_boundary_count) > 0) {
		tu->remainder_tus_upper = tu->i_upper_boundary_count;
		tu->remainder_tus_lower = tu->remainder_tus - tu->i_upper_boundary_count;
	} else {
		tu->remainder_tus_upper = tu->remainder_tus;
		tu->remainder_tus_lower = 0;
	}

	tu->total_valid_fp = (tu->paired_tus * (tu->i_upper_boundary_count *
			      tu->new_valid_boundary_link + tu->i_lower_boundary_count *
			      (tu->new_valid_boundary_link - 1)) + (tu->remainder_tus_upper *
			      tu->new_valid_boundary_link) + (tu->remainder_tus_lower *
			      (tu->new_valid_boundary_link - 1))) * f;
	if (tu->remainder_symbols_exist) {
		tu->effective_valid_fp = (((tu->total_valid_fp +
					  tu->n_remainder_symbols_per_lane_fp) * f) /
					  (tu->num_tus_per_lane * f + tu->last_partial_tu_fp));
	} else {
		tu->effective_valid_fp = (tu->total_valid_fp / tu->num_tus_per_lane);
	}

	tu->new_err_fp = tu->effective_valid_fp - (tu->ratio_fp * tu->tu_size);
	tu->old_err_fp = tu->average_valid2_fp - (tu->ratio_fp * tu->tu_size);
	tu->even_distribution = tu->num_tus % tu->nlanes == 0 ? 1 : 0;
	tu->n_tus_incl_last_incomplete_tu = DIV_ROUND_UP((tu->bpp * tu->lwidth_fp / 8),
							 tu->average_valid2_fp);
	tu->extra_required_bytes_new_tmp = DIV_ROUND_UP((tu->average_valid2_fp - (tu->tu_size *
							tu->original_ratio_fp)) *
							tu->n_tus_incl_last_incomplete_tu, f);
	tu->extra_required_bytes_new_tmp += DIV_ROUND_UP(((tu->i_upper_boundary_count *
							 tu->nlanes) *
							 (tu->new_valid_boundary_link * f -
							 tu->tu_size * tu->original_ratio_fp)),
							 f);
	tu->extra_pclk_cycles_tmp = DIV_ROUND_UP(8 * tu->extra_required_bytes_new_tmp,
						 tu->bpp);
	tu->extra_pclk_cycles_in_link_clk_tmp = DIV_ROUND_UP(tu->extra_pclk_cycles_tmp *
							     tu->lclk_fp, tu->pclk_fp);
	tu->filler_size_tmp = tu->tu_size - tu->new_valid_boundary_link;
	tu->lower_filler_size_tmp = tu->filler_size_tmp + 1;
	tu->delay_start_link_tmp = tu->extra_pclk_cycles_in_link_clk_tmp +
				   tu->lower_filler_size_tmp + tu->extra_buffer_margin;
	tu->delay_start_time_fp = tu->delay_start_link_tmp * f / tu->lclk;
	if (((tu->even_distribution == 1) ||
	    ((tu->even_distribution_BF == 0) &&
	     (tu->even_distribution_legacy == 0))) &&
	      tu->old_err_fp >= 0 && tu->new_err_fp >= 0 &&
	      (tu->new_err_fp < tu->err_fp) &&
	      ((tu->new_err_fp < tu->diff_abs_fp) ||
	       (tu->min_hblank_violated == 1)) &&
	      (tu->new_valid_boundary_link - 1) > 0 &&
	      (tu->hbp_time_fp > tu->delay_start_time_fp) &&
	      (tu->delay_start_link_tmp <= 1023)) {
		tu->upper_boundary_count = tu->i_upper_boundary_count;
		tu->lower_boundary_count = tu->i_lower_boundary_count;
		tu->err_fp = tu->new_err_fp;
		tu->boundary_moderation_en = true;
		tu->tu_size_desired = tu->tu_size;
		tu->valid_boundary_link = tu->new_valid_boundary_link;
		tu->effective_valid_recorded_fp = tu->effective_valid_fp;
		tu->even_distribution_BF = 1;
		tu->delay_start_link = tu->delay_start_link_tmp;
	} else if (tu->boundary_mod_lower_err == 0) {
		if (tu->new_err_fp < tu->diff_abs_fp)
			tu->boundary_mod_lower_err = 1;
	}
}

static void edp_ctrl_calc_tu(struct edp_ctrl *ctrl, struct edid *edid,
			struct edp_ctrl_tu *tu_table)
{
	struct tu_algo_data tu = {0};
	int64_t f = 100000;
	int64_t LCLK_FAST_SKEW_fp = (6 * f) / 1000; /* 0.0006 */
	uint8_t DP_BRUTE_FORCE = 1;
	int64_t BRUTE_FORCE_THRESHOLD_fp = (1 * f) / 10; /* 0.1 */
	int RATIO_SCALE_NUM = 1001;
	int RATIO_SCALE_DEN = 1000;
	int HBLANK_MARGIN = 4;
	int EXTRA_PIXCLK_CYCLE_DELAY = 4;
	int64_t temp = 0;
	int64_t temp_fp = 0;

	tu.lclk_fp = ctrl->link_rate_khz * f;
	tu.lclk = ctrl->link_rate_khz;
	tu.pclk = edid->mode.pixel_clock;
	tu.pclk_fp = edid->mode.pixel_clock * f;
	tu.nlanes = ctrl->lane_cnt;
	tu.bpp = edid->panel_bits_per_pixel;
	tu.lwidth = edid->mode.ha;
	tu.lwidth_fp = tu.lwidth * f;
	tu.hbp_relative_to_pclk = edid->mode.hbl;
	tu.hbp_relative_to_pclk_fp = tu.hbp_relative_to_pclk * f;
	tu.err_fp = 1000 * f;
	tu.extra_buffer_margin = DIV_ROUND_UP(tu.lclk * 4, tu.pclk);
	tu.ratio_fp =  ((int64_t)(tu.pclk_fp * tu.bpp) / 8) / (tu.nlanes * tu.lclk);
	tu.original_ratio_fp = tu.ratio_fp;
	if (((tu.lwidth % tu.nlanes) != 0) && (tu.ratio_fp < f)) {
		tu.ratio_fp = (tu.ratio_fp * RATIO_SCALE_NUM) / RATIO_SCALE_DEN;
		tu.ratio_fp = tu.ratio_fp < f ? tu.ratio_fp : f;
	}

	if (tu.ratio_fp > f)
		tu.ratio_fp = f;

	for (tu.tu_size = 32; tu.tu_size <= 64; tu.tu_size++) {
		tu.new_err_fp = (tu.ratio_fp * tu.tu_size) -
				 (((tu.ratio_fp * tu.tu_size) / f) * f);
		if (tu.new_err_fp > 0)
			tu.new_err_fp = f - tu.new_err_fp;

		if (tu.new_err_fp < tu.err_fp) {
			tu.err_fp = tu.new_err_fp;
			tu.tu_size_desired = tu.tu_size;
		}
	}

	tu.tu_size_minus1 = tu.tu_size_desired - 1;
	tu.valid_boundary_link = DIV_ROUND_UP(tu.tu_size_desired * tu.ratio_fp, f);
	tu.num_tus = ((tu.bpp * tu.lwidth) / 8) / tu.valid_boundary_link;
	tu.even_distribution_legacy = tu.num_tus % tu.nlanes == 0 ? 1 : 0;
	tu.extra_bytes = DIV_ROUND_UP(((tu.num_tus + 1) * (tu.valid_boundary_link * f -
					tu.original_ratio_fp *	tu.tu_size_desired)), f);
	tu.extra_pclk_cycles = DIV_ROUND_UP(tu.extra_bytes * 8, tu.bpp);
	tu.extra_pclk_cycles_in_link_clk = DIV_ROUND_UP(tu.extra_pclk_cycles * tu.lclk,
							 tu.pclk);
	tu.filler_size = tu.tu_size_desired - tu.valid_boundary_link;
	tu.ratio_by_tu_fp = tu.ratio_fp * tu.tu_size_desired;
	tu.delay_start_link = tu.extra_pclk_cycles_in_link_clk + tu.filler_size +
			       tu.extra_buffer_margin;
	tu.resulting_valid_fp = tu.valid_boundary_link * f;
	tu.TU_ratio_err_fp = (tu.resulting_valid_fp / tu.tu_size_desired) -
			       tu.original_ratio_fp;
	tu.hbp_time_fp = (tu.hbp_relative_to_pclk_fp - HBLANK_MARGIN * f) / tu.pclk;
	tu.delay_start_time_fp = (tu.delay_start_link * f) / tu.lclk;
	if (tu.hbp_time_fp < tu.delay_start_time_fp)
		tu.min_hblank_violated = 1;

	tu.hactive_time_fp = (tu.lwidth * f) / tu.pclk;
	if (tu.hactive_time_fp < tu.delay_start_time_fp)
		tu.min_hblank_violated = 1;

	tu.delay_start_time_fp = 0;

	/* brute force */
	tu.delay_start_link_extra_pclk = EXTRA_PIXCLK_CYCLE_DELAY;
	tu.diff_abs_fp = tu.resulting_valid_fp - tu.ratio_by_tu_fp;
	if (tu.diff_abs_fp < 0)
		tu.diff_abs_fp = tu.diff_abs_fp * -1;

	tu.boundary_mod_lower_err = 0;
	if ((tu.diff_abs_fp != 0 &&
	    ((tu.diff_abs_fp > BRUTE_FORCE_THRESHOLD_fp) ||
	     (tu.even_distribution_legacy == 0) ||
	     (DP_BRUTE_FORCE == 1))) ||
	     (tu.min_hblank_violated == 1)) {
		do {
			tu.err_fp = 1000 * f;
			tu.extra_buffer_margin = DIV_ROUND_UP(tu.lclk_fp *
							       tu.delay_start_link_extra_pclk,
							       tu.pclk_fp);
			tu.n_symbols = DIV_ROUND_UP(tu.bpp * tu.lwidth, 8);
			for (tu.tu_size = 32; tu.tu_size <= 64; tu.tu_size++) {
				for (tu.i_upper_boundary_count = 1;
					tu.i_upper_boundary_count <= 15;
					tu.i_upper_boundary_count++) {
					for (tu.i_lower_boundary_count = 1;
					     tu.i_lower_boundary_count <= 15;
					     tu.i_lower_boundary_count++) {
						tu_valid_boundary_calc(&tu);
					}
				}
			}
			tu.delay_start_link_extra_pclk--;
		} while (tu.boundary_moderation_en != true &&
			tu.boundary_mod_lower_err == 1 &&
			tu.delay_start_link_extra_pclk != 0);

		if (tu.boundary_moderation_en == true) {
			tu.resulting_valid_fp = f * (tu.upper_boundary_count *
						      tu.valid_boundary_link +
						      tu.lower_boundary_count *
						      (tu.valid_boundary_link - 1));
			tu.resulting_valid_fp /= (tu.upper_boundary_count +
						   tu.lower_boundary_count);
			tu.ratio_by_tu_fp = tu.original_ratio_fp * tu.tu_size_desired;
			tu.valid_lower_boundary_link = tu.valid_boundary_link - 1;
			tu.num_tus = ((tu.bpp / 8) * tu.lwidth_fp) / tu.resulting_valid_fp;
			tu.tu_size_minus1 = tu.tu_size_desired - 1;
			tu.even_distribution_BF = 1;
			tu.TU_ratio_err_fp = ((tu.tu_size_desired * f /
						tu.resulting_valid_fp) * f);
			tu.TU_ratio_err_fp -= tu.original_ratio_fp;
		}
	}

	temp_fp = LCLK_FAST_SKEW_fp * tu.lwidth;
	temp = DIV_ROUND_UP(temp_fp, f);
	temp_fp = ((tu.bpp * f / 8) / (tu.nlanes * tu.original_ratio_fp)) * f * temp;
	temp = temp_fp / f;

	tu.delay_start_time_fp = (tu.delay_start_link * f) / tu.lclk;

	/* OUTPUTS */
	tu_table->valid_boundary_link       = tu.valid_boundary_link;
	tu_table->delay_start_link          = tu.delay_start_link;
	tu_table->boundary_moderation_en    = tu.boundary_moderation_en;
	tu_table->valid_lower_boundary_link = tu.valid_lower_boundary_link;
	tu_table->upper_boundary_count      = tu.upper_boundary_count;
	tu_table->lower_boundary_count      = tu.lower_boundary_count;
	tu_table->tu_size_minus1            = tu.tu_size_minus1;

	printk(BIOS_INFO, "TU: valid_boundary_link: %d\n",
	       tu_table->valid_boundary_link);
	printk(BIOS_INFO, "TU: delay_start_link: %d\n",
	       tu_table->delay_start_link);
	printk(BIOS_INFO, "TU: boundary_moderation_en: %d\n",
	       tu_table->boundary_moderation_en);
	printk(BIOS_INFO, "TU: valid_lower_boundary_link: %d\n",
	       tu_table->valid_lower_boundary_link);
	printk(BIOS_INFO, "TU: upper_boundary_count: %d\n",
	       tu_table->upper_boundary_count);
	printk(BIOS_INFO, "TU: lower_boundary_count: %d\n",
	       tu_table->lower_boundary_count);
	printk(BIOS_INFO, "TU: tu_size_minus1: %d\n", tu_table->tu_size_minus1);
}

static void edp_ctrl_config_TU(struct edp_ctrl *ctrl, struct edid *edid)
{
	struct edp_ctrl_tu tu_config;

	edp_ctrl_calc_tu(ctrl, edid, &tu_config);
	write32(&edp_lclk->valid_boundary, tu_config.delay_start_link << 16 |
		tu_config.valid_boundary_link);
	write32(&edp_lclk->tu, tu_config.tu_size_minus1);
	write32(&edp_lclk->valid_boundary2, tu_config.boundary_moderation_en |
		tu_config.valid_lower_boundary_link << 1|
		tu_config.upper_boundary_count << 16 |
		tu_config.lower_boundary_count << 20);
}

static void edp_ctrl_timing_cfg(struct edid *edid)
{
	uint32_t hpolarity;
	uint32_t vpolarity;

	hpolarity = (edid->mode.phsync == '+');
	vpolarity = (edid->mode.pvsync == '+');

	/* Configure eDP timing to HW */
	write32(&edp_lclk->total_hor_ver,
		(edid->mode.ha + edid->mode.hbl) |
		(((edid->mode.va + edid->mode.vbl) << 16) & 0xffff0000));

	write32(&edp_lclk->start_hor_ver_from_sync,
		(edid->mode.hbl - edid->mode.hso) |
		(((edid->mode.vbl - edid->mode.vso) << 16) & 0xffff0000));

	write32(&edp_lclk->hysnc_vsync_width_polarity, edid->mode.hspw |
		((hpolarity << 15) & 0x8000) | ((edid->mode.vspw << 16) &
		0x7fff0000) | ((vpolarity << 31) & 0x80000000));

	write32(&edp_lclk->active_hor_ver,
		(edid->mode.ha) |
		((edid->mode.va << 16) & 0xffff0000));

}

static void edp_mainlink_ctrl(int enable)
{
	uint32_t data = 0;

	write32(&edp_lclk->mainlink_ctrl, 0x2);
	if (enable)
		data |= (0x1);

	write32(&edp_lclk->mainlink_ctrl, data);
}

static void edp_ctrl_phy_enable(int enable)
{
	if (enable) {
		write32(&edp_ahbclk->phy_ctrl, 0x4 | 0x1);
		write32(&edp_ahbclk->phy_ctrl, 0x0);
		edp_phy_enable();
	}
}

static void edp_ctrl_phy_aux_enable(int enable)
{
	if (enable) {
		/* regulators are enabled in QCLIB */
		edp_ctrl_phy_enable(1);
		edp_aux_ctrl(1);
	} else {
		edp_aux_ctrl(0);
	}
}

static void edp_ctrl_link_enable(struct edp_ctrl *ctrl,
				struct edid *edid, uint8_t *dpcd, int enable)
{
	int ret = 0;
	uint32_t m = 0, n = 0;

	if (enable) {
		edp_phy_power_on(ctrl->link_rate_khz);
		edp_phy_vm_pe_init();
		mdss_clock_configure(MDSS_CLK_EDP_LINK, 0, 1, 0, 0, 0, 0);
		ret = mdss_clock_enable(MDSS_CLK_EDP_LINK);
		if (ret != 0)
			printk(BIOS_ERR, "failed to enable link clk");

		mdss_clock_configure(MDSS_CLK_EDP_LINK_INTF, 0, 1, 0, 0, 0, 0);
		ret = mdss_clock_enable(MDSS_CLK_EDP_LINK_INTF);
		if (ret != 0)
			printk(BIOS_ERR, "failed to enable link intf clk");

		edp_ctrl_pixel_clock_dividers(ctrl, &m, &n);
		mdss_clock_configure(MDSS_CLK_EDP_PIXEL, 0, 2, 0, m, n, n);
		ret = mdss_clock_enable(MDSS_CLK_EDP_PIXEL);
		if (ret != 0)
			printk(BIOS_ERR, "failed to enable pixel clk");

		edp_mainlink_ctrl(1);
	} else {
		edp_mainlink_ctrl(0);
	}
}

static int edp_ctrl_training(struct edp_ctrl *ctrl, struct edid *edid, uint8_t *dpcd)
{
	int ret;

	/* Do link training only when power is on */
	ret = edp_do_link_train(ctrl, dpcd);
	while (ret == EDP_TRAIN_RECONFIG) {
		/* Re-configure main link */
		edp_ctrl_irq_enable(0);
		edp_ctrl_link_enable(ctrl, edid, dpcd, 0);
		edp_ctrl_phy_enable(1);
		edp_ctrl_irq_enable(1);
		edp_ctrl_link_enable(ctrl, edid, dpcd, 1);
		ret = edp_do_link_train(ctrl, dpcd);
	}

	return ret;
}

static int edp_ctrl_on(struct edp_ctrl *ctrl, struct edid *edid, uint8_t *dpcd)
{
	uint8_t value;
	int ret;

	/*
	 * By default, use the maximum link rate and minimum lane count,
	 * so that we can do rate down shift during link training.
	 */
	ctrl->link_rate_khz = dpcd[DP_MAX_LINK_RATE] * 27000;
	ctrl->link_rate = dpcd[DP_MAX_LINK_RATE];
	ctrl->lane_cnt = dpcd[DP_MAX_LANE_COUNT] & DP_MAX_LANE_COUNT_MASK;
	ctrl->color_depth = edid->panel_bits_per_color;
	ctrl->pixel_rate = edid->mode.pixel_clock;

	/* DP_SET_POWER register is only available on DPCD v1.1 and later */
	if (dpcd[DP_DPCD_REV] >= 0x11) {
		ret = edp_aux_transfer(DP_SET_POWER, DP_AUX_NATIVE_READ, &value, 1);
		if (ret < 0) {
			printk(BIOS_ERR, "edp native read failure\n");
			return -1;
		}

		value &= ~DP_SET_POWER_MASK;
		value |= DP_SET_POWER_D0;

		ret = edp_aux_transfer(DP_SET_POWER, DP_AUX_NATIVE_WRITE, &value, 1);
		if (ret < 0) {
			printk(BIOS_ERR, "edp native read failure\n");
			return -1;
		}

		/*
		 * According to the DP 1.1 specification, a "Sink Device must
		 * exit the power saving state within 1 ms" (Section 2.5.3.1,
		 * Table 5-52, "Sink Control Field" (register 0x600).
		 */
		udelay(1000);
	}

	edp_ctrl_irq_enable(1);
	edp_ctrl_link_enable(ctrl, edid, dpcd, 1);
	/* Start link training */
	ret = edp_ctrl_training(ctrl, edid, dpcd);
	if (ret != EDP_TRAIN_SUCCESS) {
		printk(BIOS_ERR, "edp training failure\n");
		return -1;
	}

	edp_train_pattern_set_write(0);

	write32(&edp_lclk->mainlink_ctrl, 0x2000000);
	write32(&edp_lclk->mainlink_ctrl, 0x2000002);
	write32(&edp_lclk->mainlink_ctrl, 0x2000000);
	write32(&edp_lclk->mainlink_ctrl, 0x2000001);
	edp_config_ctrl(ctrl, dpcd);
	edp_ctrl_config_misc(ctrl);
	edp_ctrl_timing_cfg(edid);

	if (edp_ctrl_config_msa(ctrl) < 0)
		return -1;

	edp_ctrl_config_TU(ctrl, edid);
	edp_state_ctrl(SW_SEND_VIDEO);
	edp_ctrl_irq_enable(0);

	return 0;
}

static bool edp_ctrl_panel_connected(uint8_t *dpcd)
{
	/* enable aux clk */
	mdss_clock_configure(MDSS_CLK_EDP_AUX, 0, 0, 0, 0, 0, 0);
	mdss_clock_enable(MDSS_CLK_EDP_AUX);
	edp_ctrl_phy_aux_enable(1);
	edp_ctrl_irq_enable(1);
	if (edp_aux_transfer(DP_DPCD_REV, DP_AUX_NATIVE_READ, dpcd,
			     DP_RECEIVER_CAP_SIZE) < DP_RECEIVER_CAP_SIZE) {
		printk(BIOS_ERR, "%s: AUX channel is NOT ready\n", __func__);

	} else {
		return true;
	}

	return false;
}

enum cb_err edp_ctrl_init(struct edid *edid)
{
	uint8_t dpcd[DP_RECEIVER_CAP_SIZE];
	struct edp_ctrl ctrl;

	memset(&ctrl, 0, sizeof(struct edp_ctrl));
	mdss_clock_enable(GCC_EDP_CLKREF_EN);

	if (edp_ctrl_panel_connected(dpcd) && edp_read_edid(edid) == 0) {
		if (edp_ctrl_on(&ctrl, edid, dpcd) < 0)
			return CB_ERR;

		return CB_SUCCESS;
	}

	edp_ctrl_irq_enable(0);

	return CB_ERR;
}
