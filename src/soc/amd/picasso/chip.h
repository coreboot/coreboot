/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PICASSO_CHIP_H__
#define __PICASSO_CHIP_H__

#include <stddef.h>
#include <stdint.h>
#include <amdblocks/chip.h>
#include <commonlib/helpers.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <soc/i2c.h>
#include <soc/iomap.h>
#include <soc/southbridge.h>
#include <arch/x86/include/arch/smp/mpspec.h> /* point from top level */

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
	/* HS Transmitter Pre-Emphasis Curent Control. Range 0 - 0x3 */
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

#define USB_PORT_COUNT	6

enum sd_emmc_driver_strength {
	SD_EMMC_DRIVE_STRENGTH_B,
	SD_EMMC_DRIVE_STRENGTH_A,
	SD_EMMC_DRIVE_STRENGTH_C,
	SD_EMMC_DRIVE_STRENGTH_D,
};

struct soc_amd_picasso_config {
	struct soc_amd_common_config common_config;
	/*
	 * If sb_reset_i2c_slaves() is called, this devicetree register
	 * defines which I2C SCL will be toggled 9 times at 100 KHz.
	 * For example, should we need I2C0 and  I2C3 have their slave
	 * devices reseted by toggling SCL, use:
	 *
	 * register i2c_scl_reset = (GPIO_I2C0_SCL | GPIO_I2C3_SCL)
	 */
	u8 i2c_scl_reset;
	struct dw_i2c_bus_config i2c[I2C_MASTER_DEV_COUNT];
	enum {
		I2S_PINS_MAX_HDA = 0,	/* HDA w/reset  3xSDI, SW w/Data0 */
		I2S_PINS_MAX_MHDA = 1,	/* HDA no reset 3xSDI, SW w/Data0-1 */
		I2S_PINS_MIN_HDA = 2,	/* HDA w/reset  1xSDI, SW w/Data0-2 */
		I2S_PINS_MIN_MHDA = 3,	/* HDA no reset 1xSDI, SW w/Data0-3 */
		I2S_PINS_I2S_TDM = 4,
		I2S_PINS_UNCONF = 7,	/* All pads will be input mode */
	} acp_pin_cfg;

	/* Enable ACP I2S wake feature (0 = disable, 1 = enable) */
	u8 acp_i2s_wake_enable;
	/* Enable ACP PME (0 = disable, 1 = enable) */
	u8 acp_pme_enable;

	/**
	 * IRQ 0 - 15 have a default trigger of edge and default polarity of high.
	 * If you have a device that requires a different configuration you can override the
	 * settings here.
	 */
	struct {
		uint8_t irq;
		/* See MP_IRQ_* from mpspec.h */
		uint8_t flags;
	} irq_override[16];

	/* Options for these are in src/arch/x86/include/acpi/acpi.h */
	uint16_t fadt_boot_arch;
	uint32_t fadt_flags;

	/* System config index */
	uint8_t system_config;

	/* STAPM Configuration */
	uint32_t fast_ppt_limit_mW;
	uint32_t slow_ppt_limit_mW;
	uint32_t slow_ppt_time_constant_s;
	uint32_t stapm_time_constant_s;
	uint32_t sustained_power_limit_mW;

	/* Enable dptc for tablet mode (0 = disable, 1 = enable) */
	uint8_t dptc_enable;

	/* STAPM Configuration for tablet mode (need enable dptc_enable first) */
	uint32_t fast_ppt_limit_tablet_mode_mW;
	uint32_t slow_ppt_limit_tablet_mode_mW;
	uint32_t sustained_power_limit_tablet_mode_mW;

	/* PROCHOT_L de-assertion Ramp Time */
	uint32_t prochot_l_deassertion_ramp_time_ms;

	enum {
		DOWNCORE_AUTO = 0,
		DOWNCORE_1 = 1, /* Run with single core */
		DOWNCORE_2 = 3, /* Run with two cores */
		DOWNCORE_3 = 4, /* Run with three cores */
	} downcore_mode;
	uint8_t smt_disable; /* 1=disable SMT, 0=enable SMT */

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
			SD_EMMC_EMMC_DDR_52,
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

	/* The array index is the general purpose PCIe clock output number. */
	enum gpp_clk_req_setting gpp_clk_config[GPP_CLK_OUTPUT_COUNT];
};

typedef struct soc_amd_picasso_config config_t;

extern struct device_operations pci_domain_ops;

#endif /* __PICASSO_CHIP_H__ */
