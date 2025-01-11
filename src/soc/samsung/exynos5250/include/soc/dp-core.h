/* SPDX-License-Identifier: GPL-2.0-only */

/* Header file for Samsung DP (Display Port) interface driver. */

#ifndef CPU_SAMSUNG_EXYNOS5250_DP_CORE_H
#define CPU_SAMSUNG_EXYNOS5250_DP_CORE_H

#define STREAM_ON_TIMEOUT 100
#define PLL_LOCK_TIMEOUT 10
#define DP_INIT_TRIES 10
#define MAX_CR_LOOP 5
#define MAX_EQ_LOOP 4

/* Link rate type */
enum link_rate {
	LINK_RATE_1_62GBPS = 0x06,
	LINK_RATE_2_70GBPS = 0x0a
};

/* Number of lanes supported */
enum link_lane_count {
	LANE_COUNT1 = 1,
	LANE_COUNT2 = 2,
	LANE_COUNT4 = 4
};

/* Pre emphasis level */
enum pre_emphasis_level {
	PRE_EMPHASIS_LEVEL_0,
	PRE_EMPHASIS_LEVEL_1,
	PRE_EMPHASIS_LEVEL_2,
	PRE_EMPHASIS_LEVEL_3,
};

/* Type of color space */
enum color_space {
	COLOR_RGB,
	COLOR_YCBCR422,
	COLOR_YCBCR444
};

/* Video input Bit Per Color */
enum color_depth {
	COLOR_6,
	COLOR_8,
	COLOR_10,
	COLOR_12
};

/* Type of YCbCr coefficient */
enum color_coefficient {
	COLOR_YCBCR601,
	COLOR_YCBCR709
};

/* Color range */
enum dynamic_range {
	VESA,
	CEA
};

/* Status of PLL clock */
enum pll_status {
	PLL_UNLOCKED,
	PLL_LOCKED
};

/* To choose type of m_value */
enum clock_recovery_m_value_type {
	CALCULATED_M,
	REGISTER_M
};

struct video_info {
	enum color_space color_space;
	enum dynamic_range dynamic_range;
	enum color_coefficient ycbcr_coeff;
	enum color_depth color_depth;

	enum link_rate link_rate;
	enum link_lane_count lane_count;

	char *name;

	unsigned int h_sync_polarity:1;
	unsigned int v_sync_polarity:1;
	unsigned int interlaced:1;
};

struct link_train {
	u8 link_rate;
	u8 lane_count;
};

struct s5p_dp_device {
	unsigned int		irq;
	struct exynos5_dp	*base;
	struct video_info	*video_info;
	struct link_train	link_train;
};

/* s5p_dp_reg.c */

/*
 * Reset DP module
 *
 * param dp	pointer to main s5p-dp structure
 */
void s5p_dp_reset(struct s5p_dp_device *dp);
/*
 * Initialize DP to receive video stream
 *
 * param dp	pointer to main s5p-dp structure
 */
void s5p_dp_init_video(struct s5p_dp_device *dp);
/*
 * Check whether PLL is locked
 *
 * param dp	pointer to main s5p-dp structure
 * return	Lock status
 */
unsigned int s5p_dp_get_pll_lock_status(struct s5p_dp_device *dp);
/*
 * Initialize analog functions of DP
 *
 * param dp	pointer to main s5p-dp structure
 * return	0 on success
 */
int s5p_dp_init_analog_func(struct s5p_dp_device *dp);
/*
 * Initialize DP for AUX transaction
 *
 * param dp	pointer to main s5p-dp structure
 */
void s5p_dp_init_aux(struct s5p_dp_device *dp);

/*
 * Start an AUX transaction.
 *
 * param dp	pointer to main s5p-dp structure
 */
int s5p_dp_start_aux_transaction(struct s5p_dp_device *dp);

/*
 * Write a byte to DPCD register
 *
 * param dp		pointer to main s5p-dp structure
 * param reg_addr	DPCD register to be written
 * param data		byte data to be written
 * return		write status
 */
int s5p_dp_write_byte_to_dpcd(struct s5p_dp_device *dp,
				unsigned int reg_addr,
				unsigned char data);
/*
 * Read a byte from DPCD register
 *
 * param dp		pointer to main s5p-dp structure
 * param reg_addr	DPCD register to read
 * param data		read byte data
 * return		read status
 */
int s5p_dp_read_byte_from_dpcd(struct s5p_dp_device *dp,
				unsigned int reg_addr,
				unsigned char *data);
/*
 * Initialize DP video functions
 *
 * param dp	pointer to main s5p-dp structure
 */
//void s5p_dp_init_video(struct s5p_dp_device *dp);

/*
 * Set color parameters for display
 *
 * param dp		pointer to main s5p-dp structure
 * param color_depth	Video input Bit Per Color
 * param color_space	Colorimetric format of input video
 * param dynamic_range	VESA range or CEA range
 * param coeff		YCbCr Coefficients of input video
 */
void s5p_dp_set_video_color_format(struct s5p_dp_device *dp,
				   unsigned int color_depth,
				   unsigned int color_space,
				   unsigned int dynamic_range,
				   unsigned int coeff);
/*
 * Check whether video clock is on
 *
 * param dp	pointer to main s5p-dp structure
 * return	clock status
 */
int s5p_dp_is_slave_video_stream_clock_on(struct s5p_dp_device *dp);
/*
 * Check whether video clock is on
 *
 * param dp		pointer to main s5p-dp structure
 * param type		clock_recovery_m_value_type
 * param m_value	to calculate m_vid value
 * param n_value	to calculate n_vid value
 */
void s5p_dp_set_video_cr_mn(struct s5p_dp_device *dp,
			enum clock_recovery_m_value_type type,
			unsigned int m_value,
			unsigned int n_value);
/*
 * Set DP to video slave mode thereby enabling video master
 *
 * param dp	pointer to main s5p-dp structure
 */
void s5p_dp_enable_video_master(struct s5p_dp_device *dp);
/*
 * Check whether video stream is on
 *
 * param dp	pointer to main s5p-dp structure
 * return	video stream status
 */
int s5p_dp_is_video_stream_on(struct s5p_dp_device *dp);
/*
 * Configure DP in slave mode
 *
 * param dp		pointer to main s5p-dp structure
 * param video_info	pointer to main video_info structure.
 */
void s5p_dp_config_video_slave_mode(struct s5p_dp_device *dp,
			struct video_info *video_info);

/*
 * Wait unitl HW link training done
 *
 * param dp		pointer to main s5p-dp structure
 */
void s5p_dp_wait_hw_link_training_done(struct s5p_dp_device *dp);

/* startup and init */
struct exynos5_fimd_panel;
void fb_init(unsigned long fb_size, void *lcdbase,
	     struct exynos5_fimd_panel *pd);
int dp_controller_init(struct s5p_dp_device *dp_device);
int lcd_ctrl_init(unsigned long fb_size,
		  struct exynos5_fimd_panel *panel_data, void *lcdbase);
#endif /* CPU_SAMSUNG_EXYNOS5250_DP_CORE_H */
