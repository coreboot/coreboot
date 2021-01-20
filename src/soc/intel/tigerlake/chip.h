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
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/pmc.h>
#include <soc/serialio.h>
#include <soc/usb.h>
#include <types.h>

#define MAX_HD_AUDIO_DMIC_LINKS 2
#define MAX_HD_AUDIO_SNDW_LINKS 4
#define MAX_HD_AUDIO_SSP_LINKS  6

/* The first two are for TGL-U */
#define POWER_LIMITS_U_2_CORE	0
#define POWER_LIMITS_U_4_CORE	1
#define POWER_LIMITS_Y_2_CORE	2
#define POWER_LIMITS_Y_4_CORE	3
#define POWER_LIMITS_MAX	4

/*
 * Enable External V1P05 Rail in: BIT0:S0i1/S0i2,
 * BIT1:S0i3, BIT2:S3, BIT3:S4, BIT4:S5
 */
enum fivr_enable_states {
	FIVR_ENABLE_S0i1_S0i2	= BIT(0),
	FIVR_ENABLE_S0i3	= BIT(1),
	FIVR_ENABLE_S3		= BIT(2),
	FIVR_ENABLE_S4		= BIT(3),
	FIVR_ENABLE_S5		= BIT(4),
};

/*
 * Enable the following for External V1p05 rail
 * BIT1: Normal Active voltage supported
 * BIT2: Minimum active voltage supported
 * BIT3: Minimum Retention voltage supported
 */
enum fivr_voltage_supported {
	FIVR_VOLTAGE_NORMAL		= BIT(1),
	FIVR_VOLTAGE_MIN_ACTIVE		= BIT(2),
	FIVR_VOLTAGE_MIN_RETENTION	= BIT(3),
};

#define FIVR_ENABLE_ALL_SX (FIVR_ENABLE_S0i1_S0i2 | FIVR_ENABLE_S0i3 |	\
			    FIVR_ENABLE_S3 | FIVR_ENABLE_S4 | FIVR_ENABLE_S5)

/* Bit values for use in LpmStateEnableMask. */
enum lpm_state_mask {
	LPM_S0i2_0 = BIT(0),
	LPM_S0i2_1 = BIT(1),
	LPM_S0i2_2 = BIT(2),
	LPM_S0i3_0 = BIT(3),
	LPM_S0i3_1 = BIT(4),
	LPM_S0i3_2 = BIT(5),
	LPM_S0i3_3 = BIT(6),
	LPM_S0i3_4 = BIT(7),
	LPM_S0iX_ALL = LPM_S0i2_0 | LPM_S0i2_1 | LPM_S0i2_2
		     | LPM_S0i3_0 | LPM_S0i3_1 | LPM_S0i3_2 | LPM_S0i3_3 | LPM_S0i3_4,
};

/*
 * VR domains. The domains are IA,GT,SA,VLCC and FIVR.
 */
enum vr_domains {
	VR_DOMAIN_IA,
	VR_DOMAIN_GT,
	VR_DOMAIN_SA,
	VR_DOMAIN_VLCC,
	VR_DOMAIN_FIVR,
	VR_DOMAIN_MAX
};

/*
 * Slew Rate configuration for Deep Package C States for VR domain.
 * They are fast time divided by 2.
 * 0 - Fast/2
 * 1 - Fast/4
 * 2 - Fast/8
 * 3 - Fast/16
 */
enum slew_rate {
	SLEW_FAST_2,
	SLEW_FAST_4,
	SLEW_FAST_8,
	SLEW_FAST_16
};

struct soc_intel_tigerlake_config {

	/* Common struct containing soc config data required by common code */
	struct soc_intel_common_config common_soc_config;

	/* Common struct containing power limits configuration information */
	struct soc_power_limits_config power_limits_config[POWER_LIMITS_MAX];

	/* Configuration for boot TDP selection; */
	uint8_t ConfigTdpLevel;

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
	/* S0iX: Selectively disable individual sub-states, by default all are enabled. */
	enum lpm_state_mask LpmStateDisableMask;

	/* Support for TCSS xhci, xdci, TBT PCIe root ports and DMA controllers */
	uint8_t TcssD3HotDisable;
	/* Support for TBT PCIe root ports and DMA controllers with D3Hot->D3Cold */
	uint8_t TcssD3ColdDisable;

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

	/* System Agent dynamic frequency support. Only effects ULX/ULT CPUs.
	 * When enabled memory will be training at two different frequencies.
	 * 0:Disabled, 1:FixedPoint0, 2:FixedPoint1, 3:FixedPoint2,
	 * 4:FixedPoint3, 5:Enabled */
	enum {
		SaGv_Disabled,
		SaGv_FixedPoint0,
		SaGv_FixedPoint1,
		SaGv_FixedPoint2,
		SaGv_FixedPoint3,
		SaGv_Enabled,
	} SaGv;

	/* Rank Margin Tool. 1:Enable, 0:Disable */
	uint8_t RMT;

	/* Command Pins Mirrored */
	uint32_t CmdMirror;

	/* USB related */
	struct usb2_port_config usb2_ports[16];
	struct usb3_port_config usb3_ports[10];
	/* Wake Enable Bitmap for USB2 ports */
	uint16_t usb2_wake_enable_bitmap;
	/* Wake Enable Bitmap for USB3 ports */
	uint16_t usb3_wake_enable_bitmap;

	/*
	 * Acoustic Noise Mitigation
	 * 0 - Disable
	 * 1 - Enable noise mitigation
	 */
	uint8_t AcousticNoiseMitigation;

	/*
	 * Offset 0x054B - Disable Fast Slew Rate for Deep Package
	 * C States for VR domains. Disable Fast Slew Rate for Deep
	 * Package C States based on Acoustic Noise Mitigation feature
	 * enabled. The domains are IA,GT,SA,VLCC and FIVR.
	 * 0 - False
	 * 1 - True
	 */
	uint8_t FastPkgCRampDisable[VR_DOMAIN_MAX];

	/*
	 * Offset 0x0550 - Slew Rate configuration for Deep Package
	 * C States for VR domains. Slew Rate configuration for Deep
	 * Package C States for VR domains based on Acoustic Noise
	 * Mitigation feature enabled. The domains are IA,GT,SA,VLCC and FIVR.
	 * Slew rates are defined as enum slew_rate.
	 */
	uint8_t SlowSlewRate[VR_DOMAIN_MAX];

	/* SATA related */
	uint8_t SataEnable;
	uint8_t SataMode;
	uint8_t SataSalpSupport;
	uint8_t SataPortsEnable[8];
	uint8_t SataPortsDevSlp[8];

	/*
	 * Enable(0)/Disable(1) SATA Power Optimizer on PCH side.
	 * Default 0. Setting this to 1 disables the SATA Power Optimizer.
	 */
	uint8_t SataPwrOptimizeDisable;

	/*
	 * SATA Port Enable Dito Config.
	 * Enable DEVSLP Idle Timeout settings (DmVal, DitoVal).
	 */
	uint8_t SataPortsEnableDitoConfig[8];

	/* SataPortsDmVal is the DITO multiplier. Default is 15. */
	uint8_t SataPortsDmVal[8];
	/* SataPortsDitoVal is the DEVSLP Idle Timeout, default is 625ms */
	uint16_t SataPortsDitoVal[8];

	/* Audio related */
	uint8_t PchHdaDspEnable;
	uint8_t PchHdaAudioLinkHdaEnable;
	uint8_t PchHdaAudioLinkDmicEnable[MAX_HD_AUDIO_DMIC_LINKS];
	uint8_t PchHdaAudioLinkSspEnable[MAX_HD_AUDIO_SSP_LINKS];
	uint8_t PchHdaAudioLinkSndwEnable[MAX_HD_AUDIO_SNDW_LINKS];
	uint8_t PchHdaIDispCodecDisconnect;

	/* PCIe Root Ports */
	uint8_t PcieRpEnable[CONFIG_MAX_ROOT_PORTS];
	uint8_t PcieRpHotPlug[CONFIG_MAX_ROOT_PORTS];
	/* PCIe output clocks type to PCIe devices.
	 * 0-23: PCH rootport, 0x70: LAN, 0x80: unspecified but in use,
	 * 0xFF: not used */
	uint8_t PcieClkSrcUsage[CONFIG_MAX_PCIE_CLOCKS];
	/* PCIe ClkReq-to-ClkSrc mapping, number of clkreq signal assigned to
	 * clksrc. */
	uint8_t PcieClkSrcClkReq[CONFIG_MAX_PCIE_CLOCKS];

	/* Probe CLKREQ# signal before enabling CLKREQ# based power management.*/
	uint8_t PcieRpClkReqDetect[CONFIG_MAX_ROOT_PORTS];

	/* Enable PCIe Precision Time Measurement for Root Ports (disabled by default) */
	uint8_t PciePtm[CONFIG_MAX_ROOT_PORTS];

	/* PCIe RP L1 substate */
	enum L1_substates_control PcieRpL1Substates[CONFIG_MAX_ROOT_PORTS];

	/* PCIe LTR: Enable (1) / Disable (0) */
	uint8_t PcieRpLtrEnable[CONFIG_MAX_ROOT_PORTS];

	/* PCIE RP Advanced Error Report: Enable (1) / Disable (0) */
	uint8_t PcieRpAdvancedErrorReporting[CONFIG_MAX_ROOT_PORTS];

	/* SMBus */
	uint8_t SmbusEnable;

	/* Gfx related */
	uint8_t SkipExtGfxScan;

	uint8_t Device4Enable;

	/* HeciEnabled decides the state of Heci1 at end of boot
	 * Setting to 0 (default) disables Heci1 and hides the device from OS */
	uint8_t HeciEnabled;

	/* Enable/Disable EIST. 1b:Enabled, 0b:Disabled */
	uint8_t eist_enable;

	/* Enable C6 DRAM */
	uint8_t enable_c6dram;
	uint8_t PmTimerDisabled;
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

	/* CNVi BT Audio Offload: Enable/Disable BT Audio Offload. */
	enum {
		FORCE_DISABLE,
		FORCE_ENABLE,
	} CnviBtAudioOffload;

	/* TCSS USB */
	uint8_t TcssXhciEn;
	uint8_t TcssXdciEn;

	/*
	 * Specifies which Type-C Ports are enabled on the system
	 * each bit represents a port starting at 0
	 * Example: set value to 0x3 for ports 0 and 1 to be enabled
	 */
	uint8_t UsbTcPortEn;

	/*
	 * IOM Port Config
	 * If a port orientation needs to be controlled by the SOC this setting must be
	 * updated to reflect the correct GPIOs being used for the SOC port flipping.
	 * There are 4 ports each with a pair of GPIOs for Pull Up and Pull Down
	 * 0,1 are pull up and pull down for port 0
	 * 2,3 are pull up and pull down for port 1
	 * 4,5 are pull up and pull down for port 2
	 * 6,7 are pull up and pull down for port 3
	 * values to be programmed correspond to the GPIO family and offsets
	 */
	uint32_t IomTypeCPortPadCfg[8];

	/*
	 * SOC Aux orientation override:
	 * This is a bitfield that corresponds to up to 4 TCSS ports on TGL.
	 * Even numbered bits (0, 2, 4, 6) control the retimer being handled by SOC.
	 * Odd numbered bits (1, 3, 5, 7) control the orientation of the physical aux lines
	 * on the motherboard.
	 */
	uint16_t TcssAuxOri;

	/* Connect Topology Command timeout value */
	uint16_t ITbtConnectTopologyTimeoutInMs;

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

	/* Hybrid storage mode enable (1) / disable (0)
	 * This mode makes FSP detect Optane and NVME and set PCIe lane mode
	 * accordingly */
	uint8_t HybridStorageMode;

	/*
	 * Override CPU flex ratio value:
	 * CPU ratio value controls the maximum processor non-turbo ratio.
	 * Valid Range 0 to 63.
	 * In general descriptor provides option to set default cpu flex ratio.
	 * Default cpu flex ratio 0 ensures booting with non-turbo max frequency.
	 * That's the reason FSP skips cpu_ratio override if cpu_ratio is 0.
	 * Only override CPU flex ratio to not boot with non-turbo max.
	 */
	uint8_t cpu_ratio_override;

	/* HyperThreadingDisable : Yes (1) / No (0) */
	uint8_t HyperThreadingDisable;

	/*
	 * Enable(0)/Disable(1) DMI Power Optimizer on PCH side.
	 * Default 0. Setting this to 1 disables the DMI Power Optimizer.
	 */
	uint8_t DmiPwrOptimizeDisable;

	/* structure containing various settings for PCH FIVRs */
	struct {
		bool configure_ext_fivr;
		enum fivr_enable_states v1p05_enable_bitmap;
		enum fivr_enable_states vnn_enable_bitmap;
		enum fivr_voltage_supported v1p05_supported_voltage_bitmap;
		enum fivr_voltage_supported vnn_supported_voltage_bitmap;
		/* External Icc Max for V1p05 rail in mA  */
		int v1p05_icc_max_ma;
		/* External Vnn Voltage in mV */
		int vnn_sx_voltage_mv;
	} ext_fivr_settings;

	/*
	 * Enable(1)/Disable(0) CPU Replacement check.
	 * Default 0. Setting this to 1 to check CPU replacement.
	 */
	uint8_t CpuReplacementCheck;

	/*
	 * SLP_S3 Minimum Assertion Width Policy
	 *  1 = 60us
	 *  2 = 1ms
	 *  3 = 50ms (default)
	 *  4 = 2s
	 */
	uint8_t PchPmSlpS3MinAssert;

	/*
	 * SLP_S4 Minimum Assertion Width Policy
	 *  1 = 1s (default)
	 *  2 = 2s
	 *  3 = 3s
	 *  4 = 4s
	 */
	uint8_t PchPmSlpS4MinAssert;

	/*
	 * SLP_SUS Minimum Assertion Width Policy
	 *  1 = 0ms
	 *  2 = 500ms
	 *  3 = 1s
	 *  4 = 4s (default)
	 */
	uint8_t PchPmSlpSusMinAssert;

	/*
	 * SLP_A Minimum Assertion Width Policy
	 *  1 = 0ms
	 *  2 = 4s
	 *  3 = 98ms
	 *  4 = 2s (default)
	 */
	uint8_t PchPmSlpAMinAssert;

	/*
	 * PCH PM Reset Power Cycle Duration
	 *  0 = 4s (default)
	 *  1 = 1s
	 *  2 = 2s
	 *  3 = 3s
	 *  4 = 4s
	 *
	 * NOTE: Duration programmed in the PchPmPwrCycDur should never be smaller than the
	 * stretch duration programmed in the following registers:
	 *  - GEN_PMCON_A.SLP_S3_MIN_ASST_WDTH (PchPmSlpS3MinAssert)
	 *  - GEN_PMCON_A.S4MAW (PchPmSlpS4MinAssert)
	 *  - PM_CFG.SLP_A_MIN_ASST_WDTH (PchPmSlpAMinAssert)
	 *  - PM_CFG.SLP_LAN_MIN_ASST_WDTH
	 */
	uint8_t PchPmPwrCycDur;

	/*
	 * External Clock Gate
	 * true = Mainboard design uses external clock gating
	 * false = Mainboard design does not use external clock gating
	 *
	 */
	bool external_clk_gated;

	/*
	 * External PHY Gate
	 * true = Mainboard design uses external phy gating
	 * false = Mainboard design does not use external phy gating
	 *
	 */
	bool external_phy_gated;

	/*
	 * External Bypass Enable
	 * true = Mainboard design uses external bypass rail
	 * false = Mainboard design does not use external bypass rail
	 *
	 */
	bool external_bypass;
};

typedef struct soc_intel_tigerlake_config config_t;

#endif
