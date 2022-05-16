/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <drivers/i2c/designware/dw_i2c.h>
#include <intelblocks/cfg.h>
#include <intelblocks/gpio.h>
#include <intelblocks/gspi.h>
#include <intelblocks/pcie_rp.h>
#include <intelblocks/power_limit.h>
#include <soc/gpe.h>
#include <soc/gpio.h>
#include <soc/gpio_defs.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/pmc.h>
#include <soc/serialio.h>
#include <soc/usb.h>
#include <stdint.h>

#define MAX_HD_AUDIO_SDI_LINKS	2
#define MAX_HD_AUDIO_DMIC_LINKS	2
#define MAX_HD_AUDIO_SNDW_LINKS	4
#define MAX_HD_AUDIO_SSP_LINKS	6
#define MAX_PSE_TSN_PORTS	2

/* Define config parameters for In-Band ECC (IBECC). */
#define MAX_IBECC_REGIONS	8

enum ibecc_mode {
	IBECC_PER_REGION,
	IBECC_NONE,
	IBECC_ALL
};

struct ehl_ibecc_config {
	bool enable;
	bool parity_en;
	enum ibecc_mode mode;
	bool region_enable[MAX_IBECC_REGIONS];
	uint16_t region_base[MAX_IBECC_REGIONS];
	uint16_t region_mask[MAX_IBECC_REGIONS];
};

/* TSN GBE Link Speed: 0: 2.5Gbps, 1: 1Gbps */
enum tsn_gbe_link_speed {
	Tsn_2_5_Gbps,
	Tsn_1_Gbps,
};

/* TSN Phy Interface Type: 1: RGMII, 2: SGMII, 3:SGMII+ */
enum tsn_phy_type {
	RGMII		= 1,
	SGMII		= 2,
	SGMII_plus	= 3,
};

/*
 * PSE native pins and ownership assignment:-
 * 0: Disable/pins are not owned by PSE/host
 * 1: Pins are muxed to PSE IP, the IO is owned by PSE
 * 2: Pins are muxed to PSE IP, the IO is owned by host
 */
enum pse_device_ownership {
	Device_Disabled,
	PSE_Owned,
	Host_Owned,
};

/*
 * Enable external V1P05 Rail in: BIT0:S0i1/S0i2,
 * BIT1:S0i3, BIT2:S3, BIT3:S4, BIT4:S5
 * However, EHL does not support S0i1 and S0i2,
 * hence removed the option.
 */
enum fivr_states {
	FIVR_ENABLE_S0i3	= BIT(1),
	FIVR_ENABLE_S3		= BIT(2),
	FIVR_ENABLE_S4		= BIT(3),
	FIVR_ENABLE_S5		= BIT(4),
	FIVR_ENABLE_S3_S4_S5	= FIVR_ENABLE_S3 | FIVR_ENABLE_S4 | FIVR_ENABLE_S5,
	FIVR_ENABLE_ALL_SX	= FIVR_ENABLE_S0i3 | FIVR_ENABLE_S3_S4_S5,
};

/*
 * Enable the following for external V1p05 rail
 * BIT1: Normal active voltage supported
 * BIT2: Minimum active voltage supported
 * BIT3: Minimum retention voltage supported
 */
enum fivr_supported_voltage {
	FIVR_VOLTAGE_NORMAL		= BIT(1),
	FIVR_VOLTAGE_MIN_ACTIVE		= BIT(2),
	FIVR_VOLTAGE_MIN_RETENTION	= BIT(3),
	FIVR_ENABLE_ALL_VOLTAGE		= FIVR_VOLTAGE_NORMAL | FIVR_VOLTAGE_MIN_ACTIVE |
					  FIVR_VOLTAGE_MIN_RETENTION,
};

struct soc_intel_elkhartlake_config {

	/* Common struct containing soc config data required by common code */
	struct soc_intel_common_config common_soc_config;

	/* Common struct containing power limits configuration information */
	struct soc_power_limits_config power_limits_config;

	/* Gpio group routed to each dword of the GPE0 block. Values are
	 * of the form PMC_GPP_[A:U] or GPD. */
	uint8_t pmc_gpe0_dw0; /* GPE0_31_0 STS/EN */
	uint8_t pmc_gpe0_dw1; /* GPE0_63_32 STS/EN */
	uint8_t pmc_gpe0_dw2; /* GPE0_95_64 STS/EN */

	/* Generic IO decode ranges */
	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;

	/* Enable S0iX support */
	int s0ix_enable;
	/* Enable DPTF support */
	int dptf_enable;

	/* Deep SX enable for both AC and DC */
	int deep_s3_enable_ac;
	int deep_s3_enable_dc;
	int deep_s5_enable_ac;
	int deep_s5_enable_dc;

	/* Deep Sx Configuration
	 *  DSX_EN_WAKE_PIN       - Enable WAKE# pin
	 *  DSX_EN_LAN_WAKE_PIN   - Enable LAN_WAKE# pin
	 *  DSX_DIS_AC_PRESENT_PD - Disable pull-down on AC_PRESENT pin */
	uint32_t deep_sx_config;

	/* TCC activation offset */
	uint32_t tcc_offset;
	uint32_t tcc_offset_clamp;

	/* Memory Thermal Throttling: Enable - Default (0) / Disable (1) */
	bool MemoryThermalThrottlingDisable;

	/* In-Band ECC (IBECC) configuration */
	struct ehl_ibecc_config ibecc;

	/* FuSa (Functional Safety): Disable - Default (0) / Enable (1) */
	bool FuSaEnable;

	/* System Agent dynamic frequency support.
	 * When enabled memory will be trained at different frequencies.
	 * 0:Disabled, 1:FixedPoint0(low), 2:FixedPoint1(mid), 3:FixedPoint2
	 * (high), 4:Enabled */
	enum {
		SaGv_Disabled,
		SaGv_FixedPoint0,
		SaGv_FixedPoint1,
		SaGv_FixedPoint2,
		SaGv_Enabled,
	} SaGv;

	/* Rank Margin Tool. 1:Enable, 0:Disable */
	uint8_t RMT;

	/* USB related */
	struct usb2_port_config usb2_ports[10];
	struct usb3_port_config usb3_ports[4];
	/* Wake Enable Bitmap for USB2 ports */
	uint16_t usb2_wake_enable_bitmap;
	/* Wake Enable Bitmap for USB3 ports */
	uint16_t usb3_wake_enable_bitmap;

	/* SATA related */
	uint8_t SataMode;
	uint8_t SataSalpSupport;
	uint8_t SataPortsEnable[CONFIG_MAX_SATA_PORTS];
	uint8_t SataPortsDevSlp[CONFIG_MAX_SATA_PORTS];
	/*
	 * Enable(0)/Disable(1) SATA Power Optimizer on PCH side.
	 * Default 0. Setting this to 1 disables the SATA Power Optimizer.
	 */
	uint8_t SataPwrOptimizeDisable;
	/*
	 * SATA Port Enable Dito Config.
	 * Enable DEVSLP Idle Timeout settings (DmVal, DitoVal).
	 */
	uint8_t SataPortsEnableDitoConfig[CONFIG_MAX_SATA_PORTS];
	/* SataPortsDmVal is the DITO multiplier. Default is 15. */
	uint8_t SataPortsDmVal[CONFIG_MAX_SATA_PORTS];
	/* SataPortsDitoVal is the DEVSLP Idle Timeout, default is 625ms */
	uint16_t SataPortsDitoVal[CONFIG_MAX_SATA_PORTS];

	/* Audio related */
	uint8_t PchHdaDspEnable;
	uint8_t PchHdaAudioLinkHdaEnable;
	uint8_t PchHdaSdiEnable[MAX_HD_AUDIO_SDI_LINKS];
	uint8_t PchHdaAudioLinkDmicEnable[MAX_HD_AUDIO_DMIC_LINKS];
	uint8_t PchHdaAudioLinkSspEnable[MAX_HD_AUDIO_SSP_LINKS];
	uint8_t PchHdaAudioLinkSndwEnable[MAX_HD_AUDIO_SNDW_LINKS];

	/* PCIe Root Ports */
	uint8_t PcieRpEnable[CONFIG_MAX_ROOT_PORTS];
	uint8_t PcieRpHotPlug[CONFIG_MAX_ROOT_PORTS];

	/* PCIe output clocks type to PCIe devices.
	 * 0-23: PCH rootport, 0x70: LAN, 0x80: unspecified but in use,
	 * 0xFF: not used */
	uint8_t PcieClkSrcUsage[CONFIG_MAX_PCIE_CLOCK_SRC];

	/* PCIe ClkReq-to-ClkSrc mapping, number of clkreq signal assigned to
	 * clksrc. */
	uint8_t PcieClkSrcClkReq[CONFIG_MAX_PCIE_CLOCK_SRC];

	/* Enable PCIe Precision Time Measurement for Root Ports (disabled by default) */
	uint8_t PciePtm[CONFIG_MAX_ROOT_PORTS];

	/* Probe CLKREQ# signal before enabling CLKREQ# based power management.
	 * Enable - Default (0) / Disable (1) */
	uint8_t PcieRpClkReqDetectDisable[CONFIG_MAX_ROOT_PORTS];

	/* Probe CLKREQ# signal before enabling CLKREQ# based power management.*/
	uint8_t PcieRpAdvancedErrorReportingDisable[CONFIG_MAX_ROOT_PORTS];

	/* PCIe LTR: Enable - Default (0) / Disable (1) */
	uint8_t PcieRpLtrDisable[CONFIG_MAX_ROOT_PORTS];

	/* PCIe RP L1 substate */
	enum L1_substates_control PcieRpL1Substates[CONFIG_MAX_ROOT_PORTS];

	/* eMMC and SD */
	uint8_t ScsEmmcHs400Enabled;
	uint8_t ScsEmmcDdr50Enabled;

	/* Enable if SD Card Power Enable Signal is Active High */
	uint8_t SdCardPowerEnableActiveHigh;

	/* HECI related */
	uint8_t Heci2Enable;
	uint8_t Heci3Enable;

	/* Enable/Disable EIST. 1b:Enabled, 0b:Disabled */
	uint8_t eist_enable;

	/*
	 * SerialIO device mode selection:
	 * PchSerialIoDisabled,
	 * PchSerialIoPci,
	 * PchSerialIoHidden,
	 * PchSerialIoLegacyUart,
	 * PchSerialIoSkipInit
	 */
	uint8_t SerialIoI2cMode[CONFIG_SOC_INTEL_I2C_DEV_MAX];
	uint8_t SerialIoGSpiMode[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];
	uint8_t SerialIoUartMode[CONFIG_SOC_INTEL_UART_DEV_MAX];
	/*
	 * UARTn Default DMA/PIO Mode Enable(1)/Disable(0):
	 */
	uint8_t SerialIoUartDmaEnable[CONFIG_SOC_INTEL_UART_DEV_MAX];
	/*
	 * GSPIn Default Chip Enable(1)/Disable(0):
	 */
	uint8_t SerialIoGSpiCsEnable[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];
	/*
	 * GSPIn Default Chip Select Mode:
	 * 0:Hardware Mode,
	 * 1:Software Mode
	 */
	uint8_t SerialIoGSpiCsMode[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];
	/*
	 * GSPIn Default Chip Select State:
	 * 0: Low,
	 * 1: High
	 */
	uint8_t SerialIoGSpiCsState[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];
	/*
	 * SerialIo I2C Pads Termination Config:
	 * 0x0:Hardware default,
	 * 0x1:None,
	 * 0x13:1kOhm weak pull-up,
	 * 0x15:5kOhm weak pull-up,
	 * 0x19:20kOhm weak pull-up
	 */
	uint8_t SerialIoI2cPadsTermination[CONFIG_SOC_INTEL_I2C_DEV_MAX];

	/*
	 * TraceHubMode config
	 * 0: Disable, 1: Target Debugger Mode, 2: Host Debugger Mode
	 */
	uint8_t TraceHubMode;

	/* Debug interface selection */
	enum {
		DEBUG_INTERFACE_RAM = (1 << 0),
		DEBUG_INTERFACE_UART_8250IO = (1 << 1),
		DEBUG_INTERFACE_USB3 = (1 << 3),
		DEBUG_INTERFACE_LPSS_SERIAL_IO = (1 << 4),
		DEBUG_INTERFACE_TRACEHUB = (1 << 5),
	} debug_interface_flag;

	/* GPIO SD card detect pin */
	unsigned int sdcard_cd_gpio;

	/*
	 * Override GPIO PM configuration:
	 * 0: Use FSP default GPIO PM program,
	 * 1: coreboot to override GPIO PM program
	 */
	uint8_t gpio_override_pm;

	/*
	 * GPIO PM configuration: 0 to disable, 1 to enable power gating
	 * Bit 6-7: Reserved
	 * Bit 5: MISCCFG_GPSIDEDPCGEN
	 * Bit 4: MISCCFG_GPRCOMPCDLCGEN
	 * Bit 3: MISCCFG_GPRTCDLCGEN
	 * Bit 2: MISCCFG_GSXLCGEN
	 * Bit 1: MISCCFG_GPDPCGEN
	 * Bit 0: MISCCFG_GPDLCGEN
	 */
	uint8_t gpio_pm[TOTAL_GPIO_COMM];

	/* DP config */
	/*
	 * Port config
	 * 0:Disabled, 1:eDP, 2:MIPI DSI
	 */
	uint8_t DdiPortAConfig;
	uint8_t DdiPortBConfig;

	/* Enable(1)/Disable(0) HPD */
	uint8_t DdiPortAHpd;
	uint8_t DdiPortBHpd;
	uint8_t DdiPortCHpd;
	uint8_t DdiPort1Hpd;
	uint8_t DdiPort2Hpd;
	uint8_t DdiPort3Hpd;
	uint8_t DdiPort4Hpd;

	/* Enable(1)/Disable(0) DDC */
	uint8_t DdiPortADdc;
	uint8_t DdiPortBDdc;
	uint8_t DdiPortCDdc;
	uint8_t DdiPort1Ddc;
	uint8_t DdiPort2Ddc;
	uint8_t DdiPort3Ddc;
	uint8_t DdiPort4Ddc;

	/* Skip CPU replacement check
	 * 0: disable
	 * 1: enable
	 * Setting this option to skip CPU replacement check to avoid the forced MRC training
	 * for the platforms with soldered down SOC.
	 */
	uint8_t SkipCpuReplacementCheck;

	struct {
		bool fivr_config_en;
		enum fivr_states v1p05_state;
		enum fivr_states vnn_state;
		enum fivr_states vnn_sx_state;
		enum fivr_supported_voltage v1p05_rail;
		enum fivr_supported_voltage vnn_rail;
		/* Icc max for V1p05 rail in mA */
		unsigned int v1p05_icc_max_ma;
		/* Vnn voltage in mV */
		unsigned int vnn_sx_mv;
		/* Transition time in microseconds: */
		/* From low current mode voltage to high current mode voltage */
		unsigned int vcc_low_high_us;
		/* From retention mode voltage to high current mode voltage */
		unsigned int vcc_ret_high_us;
		/* From retention mode voltage to low current mode voltage */
		unsigned int vcc_ret_low_us;
		/* From off(0V) to high current mode voltage */
		unsigned int vcc_off_high_us;
		/* RFI spread spectrum, in 0.1% increment. Range: 0.0% to 10.0% (0-100). */
		unsigned int spread_spectrum;
	} fivr;

	/*
	 * PCH power button override period.
	 * Values: 0x0 - 4s, 0x1 - 6s, 0x2 - 8s, 0x3 - 10s, 0x4 - 12s, 0x5 - 14s
	 */
	u8 PchPmPwrBtnOverridePeriod;

	/* GBE related (PCH & PSE) */
	/* TSN GBE Link Speed: 0: 2.5Gbps, 1: 1Gbps */
	enum tsn_gbe_link_speed PchTsnGbeLinkSpeed;
	enum tsn_gbe_link_speed PseTsnGbeLinkSpeed[MAX_PSE_TSN_PORTS];
	/* TSN GBE SGMII Support: Disable (0) / Enable (1) */
	bool PchTsnGbeSgmiiEnable;
	bool PseTsnGbeSgmiiEnable[MAX_PSE_TSN_PORTS];
	/* TSN GBE Multiple Virtual Channel: Disable (0) / Enable (1) */
	bool PchTsnGbeMultiVcEnable;
	bool PseTsnGbeMultiVcEnable[MAX_PSE_TSN_PORTS];
	/* PSE TSN Phy Interface Type */
	enum tsn_phy_type PseTsnGbePhyType[MAX_PSE_TSN_PORTS];

	/* PSE related */
	/*
	 * PSE (Intel Programmable Services Engine) native pins and ownership
	 * assignment. If the device is configured as 'PSE owned', PSE will have
	 * full control of specific device and it will be hidden from coreboot
	 * and OS. If the device is configured as 'Host owned', the device will
	 * be visible to coreboot and OS as a PCI device, while PSE will still
	 * do some IP initialization and pin assignment works.
	 *
	 * PSE is still required during runtime to ensure any of PSE devices
	 * works properly.
	 */
	enum pse_device_ownership PseDmaOwn[3];
	enum pse_device_ownership PseUartOwn[6];
	enum pse_device_ownership PseHsuartOwn[4];
	enum pse_device_ownership PseQepOwn[4];
	enum pse_device_ownership PseI2cOwn[8];
	enum pse_device_ownership PseI2sOwn[2];
	enum pse_device_ownership PseSpiOwn[4];
	enum pse_device_ownership PseSpiCs0Own[4];
	enum pse_device_ownership PseSpiCs1Own[4];
	enum pse_device_ownership PseCanOwn[2];
	enum pse_device_ownership PsePwmOwn;
	enum pse_device_ownership PseAdcOwn;
	enum pse_device_ownership PseGbeOwn[MAX_PSE_TSN_PORTS];
	/* PSE devices sideband interrupt: Disable (0) / Enable (1) */
	bool PseDmaSbIntEn[3];
	bool PseUartSbIntEn[6];
	bool PseQepSbIntEn[4];
	bool PseI2cSbIntEn[8];
	bool PseI2sSbIntEn[2];
	bool PseSpiSbIntEn[4];
	bool PseCanSbIntEn[2];
	bool PseLh2PseSbIntEn;
	bool PsePwmSbIntEn;
	bool PseAdcSbIntEn;
	/* PSE PWM native function: Disable (0) / Enable (1) */
	bool PsePwmPinEn[16];
	/* PSE Console Shell */
	bool PseShellEn;
};

typedef struct soc_intel_elkhartlake_config config_t;

#endif
