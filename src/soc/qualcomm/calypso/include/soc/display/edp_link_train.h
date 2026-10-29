/* SPDX-License-Identifier: GPL-2.0-or-later */
/* eDP link training register and helper definitions for calypso */

#ifndef __SOC_QUALCOMM_CALYPSO_EDP_LINK_TRAIN_H__
#define __SOC_QUALCOMM_CALYPSO_EDP_LINK_TRAIN_H__

#include <types.h>

#define DPCD_LINK_VOLTAGE_MAX      4
#define DPCD_LINK_PRE_EMPHASIS_MAX 4
#define MAX_LINK_TRAINING_LOOP     5
#define DP_LINK_RATE_UNIT_KHZ 27000

/* DP_TX Registers */
#define MAX_16BITS_VALUE                   ((1 << 16) - 1) /* 16 bits value */
#define EDP_INTR_AUX_I2C_DONE              BIT(3)
#define EDP_INTR_WRONG_ADDR                BIT(6)
#define EDP_INTR_TIMEOUT                   BIT(9)
#define EDP_INTR_NACK_DEFER                BIT(12)
#define EDP_INTR_WRONG_DATA_CNT            BIT(15)
#define EDP_INTR_I2C_NACK                  BIT(18)
#define EDP_INTR_I2C_DEFER                 BIT(21)
#define EDP_INTR_PLL_UNLOCKED              BIT(24)
#define EDP_INTR_AUX_ERROR                 BIT(27)
#define EDP_INTR_READY_FOR_VIDEO           BIT(0)
#define EDP_INTR_IDLE_PATTERN_SENT         BIT(3)
#define EDP_INTR_FRAME_END                 BIT(6)
#define EDP_INTR_CRC_UPDATED               BIT(9)
#define EDP_INTR_SST_FIFO_UNDERFLOW        BIT(28)
#define REG_EDP_DP_HPD_CTRL                (0x00000000)
#define EDP_DP_HPD_CTRL_HPD_EN             (0x00000001)
#define EDP_DP_HPD_PLUG_INT_ACK            (0x00000001)
#define EDP_DP_IRQ_HPD_INT_ACK             (0x00000002)
#define EDP_DP_HPD_REPLUG_INT_ACK          (0x00000004)
#define EDP_DP_HPD_UNPLUG_INT_ACK          (0x00000008)
#define EDP_DP_HPD_STATE_STATUS_BITS_MASK  (0x0000000F)
#define EDP_DP_HPD_STATE_STATUS_BITS_SHIFT (0x1C)
#define EDP_DP_HPD_PLUG_INT_MASK           (0x00000001)
#define EDP_DP_IRQ_HPD_INT_MASK            (0x00000002)
#define EDP_DP_HPD_REPLUG_INT_MASK         (0x00000004)
#define EDP_DP_HPD_UNPLUG_INT_MASK         (0x00000008)
#define EDP_DP_HPD_INT_MASK                                                                \
	(EDP_DP_HPD_PLUG_INT_MASK | EDP_DP_IRQ_HPD_INT_MASK | EDP_DP_HPD_REPLUG_INT_MASK | \
	 EDP_DP_HPD_UNPLUG_INT_MASK)
#define EDP_DP_HPD_STATE_STATUS_CONNECTED         (0x40000000)
#define EDP_DP_HPD_STATE_STATUS_PENDING           (0x20000000)
#define EDP_DP_HPD_STATE_STATUS_DISCONNECTED      (0x00000000)
#define EDP_DP_HPD_STATE_STATUS_MASK              (0xE0000000)
#define EDP_DP_HPD_REFTIMER_ENABLE                (1 << 16)
#define EDP_DP_HPD_EVENT_TIME_0_VAL               (0x3E800FA)
#define EDP_DP_HPD_EVENT_TIME_1_VAL               (0x1F407D0)
#define EDP_INTERRUPT_TRANS_NUM                   (0x000000A0)
#define EDP_MAINLINK_CTRL_ENABLE                  (0x00000001)
#define EDP_MAINLINK_CTRL_RESET                   (0x00000002)
#define EDP_MAINLINK_CTRL_SW_BYPASS_SCRAMBLER     (0x00000010)
#define EDP_MAINLINK_FB_BOUNDARY_SEL              (0x02000000)
#define EDP_CONFIGURATION_CTRL_SYNC_ASYNC_CLK     (0x00000001)
#define EDP_CONFIGURATION_CTRL_STATIC_DYNAMIC_CN  (0x00000002)
#define EDP_CONFIGURATION_CTRL_P_INTERLACED       (0x00000004)
#define EDP_CONFIGURATION_CTRL_INTERLACED_BTF     (0x00000008)
#define EDP_CONFIGURATION_CTRL_NUM_OF_LANES       (0x00000010)
#define EDP_CONFIGURATION_CTRL_ENHANCED_FRAMING   (0x00000040)
#define EDP_CONFIGURATION_CTRL_SEND_VSC           (0x00000080)
#define EDP_CONFIGURATION_CTRL_BPC                (0x00000100)
#define EDP_CONFIGURATION_CTRL_ASSR               (0x00000400)
#define EDP_CONFIGURATION_CTRL_RGB_YUV            (0x00000800)
#define EDP_CONFIGURATION_CTRL_LSCLK_DIV          (0x00002000)
#define EDP_CONFIGURATION_CTRL_NUM_OF_LANES_SHIFT (0x04)
#define EDP_CONFIGURATION_CTRL_BPC_SHIFT          (0x08)
#define EDP_CONFIGURATION_CTRL_LSCLK_DIV_SHIFT    (0x0D)
#define EDP_TOTAL_HOR_VER_HORIZ__MASK             (0x0000FFFF)
#define EDP_TOTAL_HOR_VER_HORIZ__SHIFT            (0)
#define DP_EDP_CONFIGURATION_CAP                  0x00d /* XXX 1.2? */
#define DP_ALTERNATE_SCRAMBLER_RESET_CAP          (1 << 0)
#define DP_FRAMING_CHANGE_CAP                     (1 << 1)
#define DP_DPCD_DISPLAY_CONTROL_CAPABLE           (1 << 3) /* edp v1.2 or higher */
#define EDP_MISC0_SYNCHRONOUS_CLK                 (0x00000001)
#define EDP_MISC0_COLORIMETRY_CFG_SHIFT           (0x00000001)
#define EDP_MISC0_TEST_BITS_DEPTH_SHIFT           (0x00000005)
#define LANE0_MAPPING_SHIFT                       (0x00000000)
#define LANE1_MAPPING_SHIFT                       (0x00000002)
#define LANE2_MAPPING_SHIFT                       (0x00000004)
#define LANE3_MAPPING_SHIFT                       (0x00000006)
#define EDP_MAINLINK_READY_FOR_VIDEO              (0x00000001)
#define EDP_MAINLINK_READY_TRAIN_PATTERN_1_READY  (0x00000008)
#define EDP_MAINLINK_SAFE_TO_EXIT_LEVEL_2         (0x00000002)
#define EDP_LINK_BW_MAX                           DP_LINK_BW_5_4
#define DP_RECEIVER_CAP_SIZE                      0x0f
#define DP_LINK_STATUS_SIZE                       6
#define DP_TRAINING_AUX_RD_MASK                   0x7F /* DP 1.3 */

/* AUX CH addresses */
/* DPCD */
#define DP_DPCD_REV                       0x000
#define DP_DPCD_REV_10                    0x10
#define DP_DPCD_REV_11                    0x11
#define DP_DPCD_REV_12                    0x12
#define DP_DPCD_REV_13                    0x13
#define DP_DPCD_REV_14                    0x14
#define DP_SET_POWER                      0x600
#define DP_SET_POWER_D0                   0x1
#define DP_SET_POWER_D3                   0x2
#define DP_SET_POWER_MASK                 0x3
#define DP_MAX_LINK_RATE                  0x001
#define DP_MAX_LANE_COUNT                 0x002
#define DP_MAX_LANE_COUNT_MASK            0x1f
#define DP_TPS3_SUPPORTED                 (1 << 6)
#define DP_ENHANCED_FRAME_CAP             (1 << 7)
#define DP_MAX_DOWNSPREAD                 0x003
#define DP_NO_AUX_HANDSHAKE_LINK_TRAINING (1 << 6)
#define DP_NORP                           0x004
#define DP_DOWNSTREAMPORT_PRESENT         0x005
#define DP_DWN_STRM_PORT_PRESENT          (1 << 0)
#define DP_DWN_STRM_PORT_TYPE_MASK        0x06
#define DP_FORMAT_CONVERSION              (1 << 3)
#define DP_MAIN_LINK_CHANNEL_CODING       0x006
#define DP_EDP_CONFIGURATION_CAP          0x00d
#define DP_TRAINING_AUX_RD_INTERVAL       0x00e

/*
 * eDP-specific DPCD revision.
 *
 * This register reports the eDP specification revision implemented by
 * the sink. It is separate from DP_DPCD_REV at DPCD address 0x000.
 */
#define DP_EDP_DPCD_REV           0x700

#define DP_EDP_11                 0x00
#define DP_EDP_12                 0x01
#define DP_EDP_13                 0x02
#define DP_EDP_14                 0x03
#define DP_EDP_14A                0x04
#define DP_EDP_14B                0x05
#define DP_EDP_15                 0x06

/*
 * eDP DPCD AUX backlight control (VESA eDP spec, "DPCD Backlight Control").
 * Mirrors the real DPCD addresses/bit layout used by the Linux msm/dp
 * driver's edp_set_dpcd_brightness()/dp_panel_read_edp_backlight_caps().
 */
#define DP_EDP_GENERAL_CAP_1                     0x701
#define DP_EDP_TCON_BACKLIGHT_ADJUSTMENT_CAP     (1 << 0)
#define DP_EDP_BACKLIGHT_AUX_ENABLE_CAP          (1 << 2)

#define DP_EDP_BACKLIGHT_ADJUSTMENT_CAP          0x702
#define DP_EDP_BACKLIGHT_BRIGHTNESS_AUX_SET_CAP  (1 << 1)
#define DP_EDP_BACKLIGHT_BRIGHTNESS_BYTE_COUNT   (1 << 2)

#define DP_EDP_BACKLIGHT_MODE_SET_REGISTER       0x721
#define DP_EDP_BACKLIGHT_CONTROL_MODE_MASK       0x3
#define DP_EDP_BACKLIGHT_CONTROL_MODE_PWM        (0 << 0)
#define DP_EDP_BACKLIGHT_CONTROL_MODE_PRESET     (1 << 0)
#define DP_EDP_BACKLIGHT_CONTROL_MODE_DPCD       (2 << 0)
#define DP_EDP_BACKLIGHT_CONTROL_MODE_PRODUCT    (3 << 0)

#define DP_EDP_DISPLAY_CONTROL_REGISTER          0x720
#define DP_EDP_BACKLIGHT_ENABLE                  (1 << 0)

#define DP_EDP_BACKLIGHT_BRIGHTNESS_MSB          0x722
#define DP_EDP_BACKLIGHT_BRIGHTNESS_LSB          0x723

//======================================================================================================================

#define DP_SUPPORTED_LINK_RATES 0x0010
#define DP_EDP_RATE_TABLE_SIZE  16 /* 0x10..0x1F */
#define DP_LINK_RATE_SET        0x0115
#define DP_LINK_RATE_SET_MASK   0x07

/* link configuration */
#define DP_LINK_BW_SET     0x100 // Sets link bit rate (e.g. 1.62/2.7/5.4/8.1 Gbps)
#define DP_LINK_RATE_TABLE 0x00  // Selects link rate from eDP 1.4 rate table instead of BW code
#define DP_LINK_BW_1_62    0x06  // Code for 1.62 Gbps link rate
#define DP_LINK_BW_2_7     0x0a  // Code for 2.7  Gbps link rate
#define DP_LINK_BW_5_4     0x14  // Code for 5.4  Gbps link rate
#define DP_LINK_BW_8_1     0x1e  // Code for 8.1  Gbps link rate

#define DP_LANE_COUNT_SET               0x101 // Sets number of active lanes and enhanced framing
#define DP_LANE_COUNT_MASK              0x0f  // Mask to extract lane-count bits
#define DP_LANE_COUNT_ENHANCED_FRAME_EN (1 << 7) // Enable enhanced framing

#define DP_TRAINING_PATTERN_SET         0x102    // Selects link-training_link-quality pattern
#define DP_TRAINING_PATTERN_DISABLE     0        // Disable training pattern (normal operation)
#define DP_TRAINING_PATTERN_1           1        // Enable Training Pattern 1 (CR phase)
#define DP_TRAINING_PATTERN_2           2        // Enable Training Pattern 2 (EQ phase)
#define DP_TRAINING_PATTERN_3           3        // Enable Training Pattern 3 (HBR2/HBR3 EQ)
#define DP_TRAINING_PATTERN_MASK        0x3      // Mask for training pattern bits
#define DP_LINK_QUAL_PATTERN_DISABLE    (0 << 2) // Disable link-quality pattern
#define DP_LINK_QUAL_PATTERN_D10_2      (1 << 2) // D10.2 compliance / test pattern
#define DP_LINK_QUAL_PATTERN_ERROR_RATE (2 << 2) // Error-rate measurement pattern
#define DP_LINK_QUAL_PATTERN_PRBS7      (3 << 2) // PRBS7 test pattern
#define DP_LINK_QUAL_PATTERN_MASK       (3 << 2) // Mask for link-quality pattern bits
#define DP_RECOVERED_CLOCK_OUT_EN       (1 << 4) // Enable recovered-clock output
#define DP_LINK_SCRAMBLING_DISABLE      (1 << 5) // Disable data scrambling on link

#define DP_EDP_CONFIGURATION_SET            0x10A    // eDP-specific feature configuration
#define DP_ALTERNATE_SCRAMBLER_RESET_ENABLE (1 << 0) // Enable alt scrambler reset
#define DP_FRAMING_CHANGE_ENABLE            (1 << 1) // Allow dynamic framing changes
#define DP_PANEL_SELF_TEST_ENABLE           (1 << 7) // Enable panel self-test mode
#define DP_SYMBOL_ERROR_COUNT_BOTH          (0 << 6) // Count both disparity & symbol errors
#define DP_SYMBOL_ERROR_COUNT_DISPARITY     (1 << 6) // Count disparity errors
#define DP_SYMBOL_ERROR_COUNT_SYMBOL        (2 << 6) // Count symbol errors
#define DP_SYMBOL_ERROR_COUNT_MASK          (3 << 6) // Mask for symbol-error count mode

#define DP_TRAINING_LANE0_SET 0x103 // Lane 0 voltage swing pre-emphasis request
#define DP_TRAINING_LANE1_SET 0x104 // Lane 1 voltage swing pre-emphasis request
#define DP_TRAINING_LANE2_SET 0x105 // Lane 2 voltage swing pre-emphasis request
#define DP_TRAINING_LANE3_SET 0x106 // Lane 3 voltage swing pre-emphasis request

#define DP_TRAIN_VOLTAGE_SWING_MASK         0x3
#define DP_TRAIN_VOLTAGE_SWING_SHIFT        0
#define DP_TRAIN_MAX_SWING_REACHED          (1 << 2)
#define DP_TRAIN_VOLTAGE_SWING_400          (0 << 0)
#define DP_TRAIN_VOLTAGE_SWING_600          (1 << 0)
#define DP_TRAIN_VOLTAGE_SWING_800          (2 << 0)
#define DP_TRAIN_VOLTAGE_SWING_1200         (3 << 0)
#define DP_TRAIN_PRE_EMPHASIS_MASK          (3 << 3)
#define DP_TRAIN_PRE_EMPHASIS_0             (0 << 3)
#define DP_TRAIN_PRE_EMPHASIS_3_5           (1 << 3)
#define DP_TRAIN_PRE_EMPHASIS_6             (2 << 3)
#define DP_TRAIN_PRE_EMPHASIS_9_5           (3 << 3)
#define DP_TRAIN_PRE_EMPHASIS_SHIFT         3
#define DP_TRAIN_MAX_PRE_EMPHASIS_REACHED   (1 << 5)
#define DP_DOWNSPREAD_CTRL                  0x107
#define DP_SPREAD_AMP_0_5                   (1 << 4)
#define DP_MAIN_LINK_CHANNEL_CODING_SET     0x108
#define DP_SET_ANSI_8B10B                   (1 << 0)
#define DP_LANE0_1_STATUS                   0x202
#define DP_LANE2_3_STATUS                   0x203
#define DP_LANE_CR_DONE                     (1 << 0)
#define DP_LANE_CHANNEL_EQ_DONE             (1 << 1)
#define DP_LANE_SYMBOL_LOCKED               (1 << 2)
#define DP_CHANNEL_EQ_BITS                  (DP_LANE_CR_DONE | DP_LANE_CHANNEL_EQ_DONE | DP_LANE_SYMBOL_LOCKED)
#define DP_LANE_ALIGN_STATUS_UPDATED        0x204
#define DP_INTERLANE_ALIGN_DONE             (1 << 0)
#define DP_DOWNSTREAM_PORT_STATUS_CHANGED   (1 << 6)
#define DP_LINK_STATUS_UPDATED              (1 << 7)
#define DP_ADJUST_REQUEST_LANE0_1           0x206
#define DP_ADJUST_REQUEST_LANE2_3           0x207
#define DP_ADJUST_VOLTAGE_SWING_LANE0_MASK  0x03
#define DP_ADJUST_VOLTAGE_SWING_LANE0_SHIFT 0
#define DP_ADJUST_PRE_EMPHASIS_LANE0_MASK   0x0c
#define DP_ADJUST_PRE_EMPHASIS_LANE0_SHIFT  2
#define DP_ADJUST_VOLTAGE_SWING_LANE1_MASK  0x30
#define DP_ADJUST_VOLTAGE_SWING_LANE1_SHIFT 4
#define DP_ADJUST_PRE_EMPHASIS_LANE1_MASK   0xc0
#define DP_ADJUST_PRE_EMPHASIS_LANE1_SHIFT  6
#define DP_TEST_REQUEST                     0x218
#define DP_TEST_LINK_TRAINING               (1 << 0)
#define DP_TEST_LINK_PATTERN                (1 << 1)
#define DP_TEST_LINK_EDID_READ              (1 << 2)
#define DP_TEST_LINK_PHY_TEST_PATTERN       (1 << 3) /* DPCD >= 1.1 */
#define DP_TEST_LINK_RATE                   0x219
#define DP_LINK_RATE_162                    (0x6)
#define DP_LINK_RATE_27                     (0xa)

#define DP_TEST_LANE_COUNT          0x220
#define DP_TEST_PATTERN             0x221
#define DP_TEST_RESPONSE            0x260
#define DP_TEST_ACK                 (1 << 0)
#define DP_TEST_NAK                 (1 << 1)
#define DP_TEST_EDID_CHECKSUM_WRITE (1 << 2)

#define DP_SET_POWER    0x600
#define DP_SET_POWER_D0 0x1
#define DP_SET_POWER_D3 0x2

/* Link training return value */
#define EDP_TRAIN_FAIL     -1
#define EDP_TRAIN_SUCCESS  0
#define EDP_TRAIN_RECONFIG 1

#define EDP_INTERRUPT_STATUS_ACK_SHIFT  1
#define EDP_INTERRUPT_STATUS_MASK_SHIFT 2

#define EDP_INTERRUPT_STATUS1                                                \
	(EDP_INTR_AUX_I2C_DONE | EDP_INTR_WRONG_ADDR | EDP_INTR_TIMEOUT |    \
	 EDP_INTR_NACK_DEFER | EDP_INTR_WRONG_DATA_CNT | EDP_INTR_I2C_NACK | \
	 EDP_INTR_I2C_DEFER | EDP_INTR_PLL_UNLOCKED | EDP_INTR_AUX_ERROR)

#define EDP_INTERRUPT_STATUS1_ACK  (EDP_INTERRUPT_STATUS1 << EDP_INTERRUPT_STATUS_ACK_SHIFT)
#define EDP_INTERRUPT_STATUS1_MASK (EDP_INTERRUPT_STATUS1 << EDP_INTERRUPT_STATUS_MASK_SHIFT)

#define EDP_INTERRUPT_STATUS2                                                         \
	(EDP_INTR_READY_FOR_VIDEO | EDP_INTR_IDLE_PATTERN_SENT | EDP_INTR_FRAME_END | \
	 EDP_INTR_CRC_UPDATED | EDP_INTR_SST_FIFO_UNDERFLOW)

#define EDP_INTERRUPT_STATUS2_ACK  (EDP_INTERRUPT_STATUS2 << EDP_INTERRUPT_STATUS_ACK_SHIFT)
#define EDP_INTERRUPT_STATUS2_MASK (EDP_INTERRUPT_STATUS2 << EDP_INTERRUPT_STATUS_MASK_SHIFT)

enum edp_color_depth {
	EDP_6BIT = 0,
	EDP_8BIT = 1,
	EDP_10BIT = 2,
	EDP_12BIT = 3,
	EDP_16BIT = 4,
};

struct dp_tu_calc_input {
	u64 lclk;              /* 162, 270, 540 and 810 MHz*/
	u64 pclk_khz;          /* in KHz */
	u64 hactive;           /* active h-width */
	u64 hporch;            /* bp + fp + pulse */
	int nlanes;            /* no.of.lanes */
	int bpp;               /* bits */
	int pixel_enc;         /* 444, 420, 422 */
	int dsc_en;            /* dsc on/off */
	int async_en;          /* async mode */
	int fec_en;            /* fec */
	int compress_ratio;    /* 2:1 = 200, 3:1 = 300, 3.75:1 = 375 */
	int num_of_dsc_slices; /* number of slices per line */
	s64 comp_bpp;          /* compressed bpp = uncomp_bpp / compression_ratio */
	int ppc_div_factor;    /* pass in ppc mode 2/4 */
};

struct dp_tu_calc_output {
	u32 valid_boundary_link;
	u32 delay_start_link;
	bool boundary_moderation_en;
	u32 valid_lower_boundary_link;
	u32 upper_boundary_count;
	u32 lower_boundary_count;
	u32 tu_size_minus1;
};

struct tu_algo_data {
	s64 lclk_fp;
	s64 orig_lclk_fp;

	s64 pclk_fp;
	s64 orig_pclk_fp;
	s64 lwidth;
	s64 lwidth_fp;
	int orig_lwidth;
	s64 hbp_relative_to_pclk;
	s64 hbp_relative_to_pclk_fp;
	int orig_hbp;
	int nlanes;
	int bpp;
	int pixelEnc;
	int dsc_en;
	int async_en;
	int fec_en;
	int bpc;

	int rb2;
	unsigned int delay_start_link_extra_pixclk;
	int extra_buffer_margin;
	s64 ratio_fp;
	s64 original_ratio_fp;

	s64 err_fp;
	s64 n_err_fp;
	s64 n_n_err_fp;
	int tu_size;
	int tu_size_desired;
	int tu_size_minus1;

	int valid_boundary_link;
	s64 resulting_valid_fp;
	s64 total_valid_fp;
	s64 effective_valid_fp;
	s64 effective_valid_recorded_fp;
	int n_tus;
	int n_tus_per_lane;
	int paired_tus;
	int remainder_tus;
	int remainder_tus_upper;
	int remainder_tus_lower;
	int extra_bytes;
	int filler_size;
	int delay_start_link;

	int extra_pclk_cycles;
	int extra_pclk_cycles_in_link_clk;
	s64 ratio_by_tu_fp;
	s64 average_valid2_fp;
	int new_valid_boundary_link;
	int remainder_symbols_exist;
	int n_symbols;
	s64 n_remainder_symbols_per_lane_fp;
	s64 last_partial_tu_fp;
	s64 TU_ratio_err_fp;

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
	int hbp_delayStartCheck;
	int pre_tu_hw_pipe_delay;
	int post_tu_hw_pipe_delay;
	int link_config_hactive_time;
	int delay_start_link_lclk;
	int tu_active_cycles;
	s64 parity_symbols;
	int resolution_line_time;
	int last_partial_lclk;
	int min_hblank_violated;
	s64 delay_start_time_fp;
	s64 hbp_time_fp;
	s64 hactive_time_fp;
	s64 diff_abs_fp;
	int second_loop_set;
	s64 ratio;
	s64 comp_bpp;
	int ppc_div_factor;
};

struct edp_ctrl {
	/* Link status */
	uint32_t link_rate_khz;
	uint8_t link_rate;
	uint32_t lane_cnt;
	uint8_t v_level;
	uint8_t p_level;

	/* eDP rate-table mode ORBs */
	bool use_rate_select;
	uint8_t rate_select_idx; /* 0..7 -> DPCD 0x115[2:0] */

	/* Timing status */
	uint32_t pixel_rate; /* in kHz */
	uint32_t color_depth;

	/*============================================*/
	uint8_t edp_rev;
};


struct edp_ctrl_tu {
	uint32_t tu_size_minus1;            /* Desired TU Size */
	uint32_t valid_boundary_link;       /* Upper valid boundary */
	uint32_t delay_start_link;          /* # of clock cycles to delay */
	bool boundary_moderation_en;        /* Enable boundary Moderation? */
	uint32_t valid_lower_boundary_link; /* Valid lower boundary link */
	uint32_t upper_boundary_count;      /* Upper boundary Count */
	uint32_t lower_boundary_count;      /* Lower boundary Count */
};

int edp_ctrl_on(struct edp_ctrl *ctrl, struct edid *edid, uint8_t *dpcd);
void edp_ctrl_config_TU(struct edp_ctrl *ctrl, struct edid *edid);

#endif
