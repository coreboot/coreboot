/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PICASSO_CHIP_H__
#define __PICASSO_CHIP_H__

#include <amdblocks/chip.h>
#include <commonlib/helpers.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <gpio.h>
#include <soc/i2c.h>
#include <soc/iomap.h>
#include <soc/southbridge.h>
#include <arch/x86/include/arch/smp/mpspec.h> /* point from top level */
#include <types.h>

/*
  USB 2.0 PHY Parameters
*/
struct __packed usb2_phy_tune {
	/* Disconnect Threshold Adjustment. Range 0 - 0x7 */
	uint8_t	com_pds_tune;
	/* Squelch Threshold Adjustment. Range 0 - 0x7 */
	uint8_t	sq_rx_tune;
	/* FS/LS Source Impedance Adjustment. Range 0 - 0xF */
	uint8_t	tx_fsls_tune;
	/* HS Transmitter Pre-Emphasis Current Control. Range 0 - 0x3 */
	uint8_t	tx_pre_emp_amp_tune;
	/* HS Transmitter Pre-Emphasis Duration Control. Range: 0 - 0x1 */
	uint8_t	tx_pre_emp_pulse_tune;
	/* HS Transmitter Rise/Fall Time Adjustment. Range: 0 - 0x3 */
	uint8_t	tx_rise_tune;
	/* HS DC Voltage Level Adjustment. Range 0 - 0xF */
	uint8_t	tx_vref_tune;
	/* Transmitter High-Speed Crossover Adjustment. Range 0 - 0x3 */
	uint8_t	tx_hsxv_tune;
	/* USB Source Impedance Adjustment. Range 0 - 0x3. */
	uint8_t	tx_res_tune;
};

/* force USB3 port to gen1, bit0 - controller0 Port0, bit1 - Port1, etc */
union __packed usb3_force_gen1 {
		struct {
			uint8_t xhci0_port0:1;
			uint8_t xhci0_port1:1;
			uint8_t xhci0_port2:1;
			uint8_t xhci0_port3:1;
		} ports;
		uint8_t usb3_port_force_gen1_en;
};

enum rfmux_configuration_setting {
	USB_PD_RFMUX_SAFE_STATE = 0x0,
	USB_PD_RFMUX_USB31_MODE = 0x1,
	USB_PD_RFMUX_USB31_MODE_FLIP = 0x2,
	USB_PD_RFMUX_ATE_MODE = 0x3,
	USB_PD_RFMUX_DP_X2_MODE = 0x4,
	USB_PD_RFMUX_MF_MODE_ALT_D_F = 0x6,
	USB_PD_RFMUX_DP_X2_MODE_FLIP = 0x8,
	USB_PD_RFMUX_MF_MODE_ALT_D_F_FLIP = 0x9,
	USB_PD_RFMUX_DP_X4_MODE = 0xc,
};

struct usb_pd_control {
		uint8_t rfmux_override_en;
		uint32_t rfmux_config;
};

#define USB_PORT_COUNT	6

struct __packed usb3_phy_tune {
	uint8_t rx_eq_delta_iq_ovrd_val;
	uint8_t rx_eq_delta_iq_ovrd_en;
};
/* the RV2 USB3 port count */
#define RV2_USB3_PORT_COUNT 4
#define USB_PD_PORT_COUNT 2

enum sd_emmc_driver_strength {
	SD_EMMC_DRIVE_STRENGTH_B,
	SD_EMMC_DRIVE_STRENGTH_A,
	SD_EMMC_DRIVE_STRENGTH_C,
	SD_EMMC_DRIVE_STRENGTH_D,
};

/* dpphy_override */
enum sysinfo_dpphy_override {
	ENABLE_DVI_TUNINGSET = 0x01,
	ENABLE_HDMI_TUNINGSET = 0x02,
	ENABLE_HDMI6G_TUNINGSET = 0x04,
	ENABLE_DP_TUNINGSET = 0x08,
	ENABLE_DP_HBR3_TUNINGSET = 0x10,
	ENABLE_DP_HBR_TUNINGSET = 0x20,
	ENABLE_DP_HBR2_TUNINGSET = 0x40,
	ENABLE_EDP_TUNINGSET = 0x80,
};

struct soc_amd_picasso_config {
	struct soc_amd_common_config common_config;
	/*
	 * If sb_reset_i2c_peripherals() is called, this devicetree register
	 * defines which I2C SCL will be toggled 9 times at 100 KHz.
	 * For example, should we need I2C0 and  I2C3 have their peripheral
	 * devices reset by toggling SCL, use:
	 *
	 * register i2c_scl_reset = (GPIO_I2C0_SCL | GPIO_I2C3_SCL)
	 */
	u8 i2c_scl_reset;
	struct dw_i2c_bus_config i2c[I2C_CTRLR_COUNT];

	/* System config index */
	uint8_t system_config;

	/* STAPM Configuration */
	uint32_t fast_ppt_limit_mW;
	uint32_t slow_ppt_limit_mW;
	uint32_t slow_ppt_time_constant_s;
	uint32_t stapm_time_constant_s;
	uint32_t sustained_power_limit_mW;

	/* Enable dptc for tablet mode */
	bool dptc_enable;

	/* STAPM Configuration for tablet mode (need enable dptc_enable first) */
	uint32_t fast_ppt_limit_tablet_mode_mW;
	uint32_t slow_ppt_limit_tablet_mode_mW;
	uint32_t sustained_power_limit_tablet_mode_mW;

	/* PROCHOT_L de-assertion Ramp Time */
	uint32_t prochot_l_deassertion_ramp_time_ms;

	enum {
		DOWNCORE_AUTO = 0,
		DOWNCORE_1 = 1, /* Run with 1 physical core */
		DOWNCORE_2 = 3, /* Run with 2 physical cores */
		DOWNCORE_3 = 4, /* Run with 3 physical cores */
	} downcore_mode;
	bool smt_disable; /* true=disable SMT on all physical cores */

	/* Lower die temperature limit */
	uint32_t thermctl_limit_degreeC;
	uint32_t thermctl_limit_tablet_mode_degreeC;

	/* FP5 Processor Voltage Supply PSI Currents. 0 indicates use SOC default */
	uint32_t psi0_current_limit_mA;
	uint32_t psi0_soc_current_limit_mA;
	uint32_t vddcr_soc_voltage_margin_mV;
	uint32_t vddcr_vdd_voltage_margin_mV;

	/* VRM Limits. 0 indicates use SOC default */
	uint32_t vrm_maximum_current_limit_mA;
	uint32_t vrm_soc_maximum_current_limit_mA;
	uint32_t vrm_current_limit_mA;
	uint32_t vrm_soc_current_limit_mA;

	/* Misc SMU settings */
	uint8_t sb_tsi_alert_comparator_mode_en;
	uint8_t core_dldo_bypass;
	uint8_t min_soc_vid_offset;
	uint8_t aclk_dpm0_freq_400MHz;
	uint32_t telemetry_vddcr_vdd_slope_mA;
	uint32_t telemetry_vddcr_vdd_offset;
	uint32_t telemetry_vddcr_soc_slope_mA;
	uint32_t telemetry_vddcr_soc_offset;

	/*
	 * HDMI 2.0 disable setting
	 * bit0~3: disable HDMI 2.0 DDI0~3
	*/
	uint8_t hdmi2_disable;

	struct {
		/*
		 * SDHCI doesn't directly support eMMC. There is an implicit mapping between
		 * eMMC timing modes and SDHCI UHS-I timing modes defined in the linux
		 * kernel.
		 *
		 *  HS    -> UHS_SDR12 (0x00)
		 *  DDR52 -> UHS_DDR50 (0x04)
		 *  HS200 -> UHS_SDR104 (0x03)
		 *  HS400 -> NONE (0x05)
		 *
		 * The kernel driver uses a heuristic to determine if HS400 is supported.
		*/
		enum {
			SD_EMMC_DISABLE,
			SD_EMMC_SD_LOW_SPEED,
			SD_EMMC_SD_HIGH_SPEED,
			SD_EMMC_SD_UHS_I_SDR_50,
			SD_EMMC_SD_UHS_I_DDR_50,
			SD_EMMC_SD_UHS_I_SDR_104,
			SD_EMMC_EMMC_SDR_26,
			SD_EMMC_EMMC_SDR_52,
			SD_EMMC_EMMC_DDR_104,
			SD_EMMC_EMMC_HS200,
			SD_EMMC_EMMC_HS400,
			SD_EMMC_EMMC_HS300,
		} timing;

		/*
		 * Sets the driver strength reflected in the SDHCI Preset Value Registers.
		 *
		 * According to the SDHCI spec:
		 *   The host should select the weakest drive strength that meets rise /
		 *   fall time requirement at system operating frequency.
		 */
		enum sd_emmc_driver_strength sdr104_hs400_driver_strength;
		enum sd_emmc_driver_strength ddr50_driver_strength;
		enum sd_emmc_driver_strength sdr50_driver_strength;

		/*
		 * Sets the frequency in kHz reflected in the Initialization Preset Value
		 * Register.
		 *
		 * This value is used while in open-drain mode, and has a maximum value of
		 * 400 kHz.
		 */
		uint16_t init_khz_preset;
	} emmc_config;

	/* Force USB3 port to gen1, bit0 - controller0 Port0, bit1 - Port1 */
	union usb3_force_gen1 usb3_port_force_gen1;

	uint8_t has_usb2_phy_tune_params;
	struct usb2_phy_tune usb_2_port_tune_params[USB_PORT_COUNT];
	enum {
		USB_OC_PIN_0	= 0x0,
		USB_OC_PIN_1	= 0x1,
		USB_OC_PIN_2	= 0x2,
		USB_OC_PIN_3	= 0x3,
		USB_OC_PIN_4	= 0x4,
		USB_OC_PIN_5	= 0x5,
		USB_OC_NONE	= 0xf,
	} usb_port_overcurrent_pin[USB_PORT_COUNT];

	/* RV2 SOC Usb 3.1 PHY Parameters */
	uint8_t usb3_phy_override;
	/*
	 * 1,RX_EQ_DELTA_IQ_OVRD_VAL- Override value for rx_eq_delta_iq. Range 0-0xF
	 * 2,RX_EQ_DELTA_IQ_OVRD_EN - Enable override value for rx_eq_delta_iq. Range 0-0x1
	 */
	struct usb3_phy_tune usb3_phy_tune_params[RV2_USB3_PORT_COUNT];
	/* Override value for rx_vref_ctrl. Range 0 - 0x1F */
	uint8_t usb3_rx_vref_ctrl;
	/* Enable override value for rx_vref_ctrl. Range 0 - 0x1 */
	uint8_t usb3_rx_vref_ctrl_en;
	/* Override value for tx_vboost_lvl: 0 - 0x7. */
	uint8_t usb_3_tx_vboost_lvl;
	/* Enable override value for tx_vboost_lvl. Range: 0 - 0x1 */
	uint8_t usb_3_tx_vboost_lvl_en;
	/* Override value for rx_vref_ctrl. Range 0 - 0x1F.*/
	uint8_t usb_3_rx_vref_ctrl_x;
	/* Enable override value for rx_vref_ctrl. Range 0 - 0x1. */
	uint8_t usb_3_rx_vref_ctrl_en_x;
	/* Override value for tx_vboost_lvl: 0 - 0x7. */
	uint8_t usb_3_tx_vboost_lvl_x;
	/* Enable override value for tx_vboost_lvl. Range: 0 - 0x1. */
	uint8_t usb_3_tx_vboost_lvl_en_x;

	/* The array index is the general purpose PCIe clock output number. Values in here
	   aren't the values written to the register to have the default to be always on. */
	enum {
		GPP_CLK_ON,	/* GPP clock always on; default */
		GPP_CLK_REQ,	/* GPP clock controlled by corresponding #CLK_REQx pin */
		GPP_CLK_OFF,	/* GPP clk off */
	} gpp_clk_config[GPP_CLK_OUTPUT_COUNT];

	/* performance policy for the PCIe links: power consumption vs. link speed */
	enum {
		DXIO_PSPP_DISABLED = 0,
		DXIO_PSPP_PERFORMANCE,
		DXIO_PSPP_BALANCED,
		DXIO_PSPP_POWERSAVE,
	} pspp_policy;

	/* If using an external 48MHz OSC for codec, will disable internal X48M_OSC */
	bool acp_i2s_use_external_48mhz_osc;

	/* eDP phy tuning settings */
	uint16_t edp_phy_override;
	/* bit vector of phy, bit0=1: DP0, bit1=1: DP1, bit2=1: DP2 bit3=1: DP3 */
	uint8_t edp_physel;

	struct {
		uint8_t dp_vs_pemph_level;
		uint8_t deemph_6db4;
		uint8_t boostadj;
		uint16_t margin_deemph;
	} edp_tuningset;

	/*
	 * eDP panel power sequence control
	 * all pwr sequence numbers below are in uint of 4ms and "0" as default value
	 */
	uint8_t edp_pwr_adjust_enable;
	uint8_t pwron_digon_to_de;
	uint8_t pwron_de_to_varybl;
	uint8_t pwrdown_varybloff_to_de;
	uint8_t pwrdown_de_to_digoff;
	uint8_t pwroff_delay;
	uint8_t pwron_varybl_to_blon;
	uint8_t pwrdown_bloff_to_varybloff;
	uint8_t min_allowed_bl_level;

	/* allow USB PD port setting override */
	struct usb_pd_control usb_pd_config_override[USB_PD_PORT_COUNT];
};

#endif /* __PICASSO_CHIP_H__ */
