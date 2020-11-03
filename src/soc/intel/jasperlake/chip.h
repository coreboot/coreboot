/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <drivers/i2c/designware/dw_i2c.h>
#include <intelblocks/cfg.h>
#include <intelblocks/gpio.h>
#include <intelblocks/gspi.h>
#include <intelblocks/power_limit.h>
#include <soc/gpe.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/pmc.h>
#include <soc/serialio.h>
#include <soc/usb.h>
#include <stdint.h>

#define MAX_HD_AUDIO_DMIC_LINKS 2
#define MAX_HD_AUDIO_SNDW_LINKS 4
#define MAX_HD_AUDIO_SSP_LINKS  6

struct soc_intel_jasperlake_config {

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

	/* System Agent dynamic frequency support.
	 * When enabled memory will be training at different frequencies.
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
	struct usb2_port_config usb2_ports[16];
	struct usb3_port_config usb3_ports[10];
	/* Wake Enable Bitmap for USB2 ports */
	uint16_t usb2_wake_enable_bitmap;
	/* Wake Enable Bitmap for USB3 ports */
	uint16_t usb3_wake_enable_bitmap;

	/* SATA related */
	uint8_t SataMode;
	uint8_t SataSalpSupport;
	uint8_t SataPortsEnable[8];
	uint8_t SataPortsDevSlp[8];

	/* Audio related */
	uint8_t PchHdaDspEnable;
	uint8_t PchHdaAudioLinkHdaEnable;
	uint8_t PchHdaAudioLinkDmicEnable[MAX_HD_AUDIO_DMIC_LINKS];
	uint8_t PchHdaAudioLinkSspEnable[MAX_HD_AUDIO_SSP_LINKS];
	uint8_t PchHdaAudioLinkSndwEnable[MAX_HD_AUDIO_SNDW_LINKS];
	uint8_t PchHdaIDispLinkTmode;
	uint8_t PchHdaIDispLinkFrequency;
	uint8_t PchHdaIDispCodecDisconnect;

	/* PCIe Root Ports */
	uint8_t PcieRpEnable[CONFIG_MAX_ROOT_PORTS];
	/* PCIe output clocks type to PCIe devices.
	 * 0-23: PCH rootport, 0x70: LAN, 0x80: unspecified but in use,
	 * 0xFF: not used */
	uint8_t PcieClkSrcUsage[CONFIG_MAX_PCIE_CLOCKS];
	/* PCIe ClkReq-to-ClkSrc mapping, number of clkreq signal assigned to
	 * clksrc. */
	uint8_t PcieClkSrcClkReq[CONFIG_MAX_PCIE_CLOCKS];

	/* Probe CLKREQ# signal before enabling CLKREQ# based power management.*/
	uint8_t PcieRpClkReqDetect[CONFIG_MAX_ROOT_PORTS];

	/* PCIe RP L1 substate */
	enum L1_substates_control {
		L1_SS_FSP_DEFAULT,
		L1_SS_DISABLED,
		L1_SS_L1_1,
		L1_SS_L1_2,
	} PcieRpL1Substates[CONFIG_MAX_ROOT_PORTS];

	/* SMBus */
	uint8_t SmbusEnable;

	/* eMMC and SD */
	uint8_t ScsEmmcHs400Enabled;

	/* Enable if SD Card Power Enable Signal is Active High */
	uint8_t SdCardPowerEnableActiveHigh;

	/* Integrated Sensor */
	uint8_t PchIshEnable;

	/* Heci related */
	uint8_t Heci3Enabled;

	/* VR Config Settings for IA Core */
	uint16_t ImonSlope;
	uint16_t ImonOffset;

	/* Gfx related */
	uint8_t IgdDvmt50PreAlloc;
	uint8_t SkipExtGfxScan;

	uint32_t GraphicsConfigPtr;

	/* HeciEnabled decides the state of Heci1 at end of boot
	 * Setting to 0 (default) disables Heci1 and hides the device from OS */
	uint8_t HeciEnabled;

	/* Enable/Disable EIST. 1b:Enabled, 0b:Disabled */
	uint8_t eist_enable;

	/* Enable C6 DRAM */
	uint8_t enable_c6dram;
	/*
	 * PRMRR size setting with below options
	 * Disable: 0x0
	 * 32MB: 0x2000000
	 * 64MB: 0x4000000
	 * 128 MB: 0x8000000
	 * 256 MB: 0x10000000
	 * 512 MB: 0x20000000
	 */
	uint32_t PrmrrSize;
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

	/* GPIO SD card detect pin */
	unsigned int sdcard_cd_gpio;

	/* Enable Pch iSCLK */
	uint8_t pch_isclk;

	/* CNVi BT Audio Offload: Enable/Disable BT Audio Offload. */
	enum {
		FORCE_DISABLE,
		FORCE_ENABLE,
	} CnviBtAudioOffload;

	/* Tcss */
	uint8_t TcssXhciEn;
	uint8_t TcssXdciEn;

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

	/* Skip CPU replacement check
	 * 0: disable
	 * 1: enable
	 * Setting this option to skip CPU replacement check to avoid the forced MRC training
	 * for the platforms with soldered down SOC.
	 */
	uint8_t SkipCpuReplacementCheck;

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
};

typedef struct soc_intel_jasperlake_config config_t;

#endif
