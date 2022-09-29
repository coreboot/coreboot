/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _PANEL_JDI_LPM102A188A_H_
#define _PANEL_JDI_LPM102A188A_H_

#define LP8557_MAX_BRIGHTNESS				0xFFF

#define LP8557_COMMAND					0x00
#define LP8557_COMMAND_ON				(1 << 0)

#define LP8557_BRIGHTNESS_LOW				0x03
#define LP8557_BRIGHTNESS_LOW_MASK(x)			(((x) & 0xF) << 4)

#define LP8557_BRIGHTNESS_HIGH				0x04
#define LP8557_BRIGHTNESS_HIGH_MASK(x)			(((x) & 0xFF0) >> 4)

enum lp8557_config_brightness_mode {
	LP8557_CONFIG_BRTMODE_PWM			= 0x0,
	LP8557_CONFIG_BRTMODE_REG,
	LP8557_CONFIG_BRTMODE_PWM_REG_SHAPE_PWM,
	LP8557_CONFIG_BRTMODE_PWM_REG_SHAPE_BRIGHTNESS,
	LP8557_CONFIG_BRTMODE_MAX,
};
#define LP8557_CONFIG					0x10
#define LP8557_CONFIG_BRTMODE(x)			(((x) & 0x3) << 0)
#define LP8557_CONFIG_AUTO_DETECT_LED			(1 << 2)
#define LP8557_CONFIG_PWM_STANDBY			(1 << 7)

enum lp8557_current {
	LP8557_CURRENT_5_MA				= 0x0,
	LP8557_CURRENT_10_MA,
	LP8557_CURRENT_13_MA,
	LP8557_CURRENT_15_MA,
	LP8557_CURRENT_18_MA,
	LP8557_CURRENT_20_MA,
	LP8557_CURRENT_23_MA,
	LP8557_CURRENT_25_MA,
	LP8557_CURRENT_MAX,
};
#define LP8557_CURRENT					0x11
#define LP8557_CURRENT_MAXCURR(x)			(((x) & 0x7) << 0)
#define LP8557_CURRENT_ISET				(1 << 7)

enum lp8557_pgen_frequency {
	LP8557_PGEN_FREQ_4_9_KHZ			= 0x0,
	LP8557_PGEN_FREQ_9_8_KHZ,
	LP8557_PGEN_FREQ_14_6_KHZ,
	LP8557_PGEN_FREQ_19_5_KHZ,
	LP8557_PGEN_FREQ_24_4_KHZ,
	LP8557_PGEN_FREQ_29_3_KHZ,
	LP8557_PGEN_FREQ_34_2_KHZ,
	LP8557_PGEN_FREQ_39_1_KHZ,
	LP8557_PGEN_FREQ_MAX,
};
#define LP8557_PGEN					0x12
#define LP8557_PGEN_FREQ(x)				(((x) & 0x7) << 0)
#define LP8557_PGEN_MAGIC				(5 << 3)
#define LP8557_PGEN_FSET				(1 << 7)

enum lp8557_boost_freq {
	LP8557_BOOST_FREQ_500_KHZ			= 0x0,
	LP8557_BOOST_FREQ_1_MHZ,
	LP8557_BOOST_FREQ_MAX,
};
enum lp8557_boost_bcomp {
	LP8557_BOOST_BCOMP_OPTION_0			= 0x0,
	LP8557_BOOST_BCOMP_OPTION_1,
	LP8557_BOOST_BCOMP_MAX,
};
#define LP8557_BOOST					0x13
#define LP8557_BOOST_FREQ(x)				(((x) & 0x1) << 0)
#define LP8557_BOOST_BCOMP(x)				(((x) & 0x1) << 1)
#define LP8557_BOOST_BCSET				(1 << 6)
#define LP8557_BOOST_BFSET				(1 << 7)

#define LP8557_LED_ENABLE				0x14
#define LP8557_LED_ENABLE_SINKS(x)			(((x) & 0x3F) << 0)
#define LP8557_LED_ENABLE_MAGIC				(2 << 6)

enum lp8557_step_ramp {
	LP8557_STEP_RAMP_0_MS				= 0x0,
	LP8557_STEP_RAMP_50_MS,
	LP8557_STEP_RAMP_100_MS,
	LP8557_STEP_RAMP_200_MS,
	LP8557_STEP_RAMP_MAX,
};
enum lp8557_step_smoothing {
	LP8557_STEP_SMOOTHING_NONE			= 0x0,
	LP8557_STEP_SMOOTHING_LIGHT,
	LP8557_STEP_SMOOTHING_MEDIUM,
	LP8557_STEP_SMOOTHING_HEAVY,
	LP8557_STEP_SMOOTHING_MAX,
};
#define LP8557_STEP					0x15
#define LP8557_STEP_RAMP(x)				(((x) & 0x3) << 0)
#define LP8557_STEP_SMOOTHING(x)			(((x) & 0x3) << 6)

struct mipi_dsi_device;
struct soc_nvidia_tegra210_config;

struct panel_jdi {
	struct mipi_dsi_device *dsi;
	const struct soc_nvidia_tegra210_config *mode;

	/* Physical size */
	unsigned int width_mm;
	unsigned int height_mm;

	int enabled;
};

struct panel_jdi *panel_jdi_dsi_probe(struct mipi_dsi_device *dsi);
int panel_jdi_prepare(struct panel_jdi *jdi);

#endif
