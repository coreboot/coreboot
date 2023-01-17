/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <drivers/i2c/designware/dw_i2c.h>
#include <drivers/intel/gma/gma.h>
#include <gpio.h>
#include <intelblocks/cfg.h>
#include <intelblocks/gspi.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/power_limit.h>
#include <stdint.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/pmc.h>
#include <soc/sata.h>
#include <soc/serialio.h>
#include <soc/usb.h>
#include <soc/vr_config.h>
#if CONFIG(SOC_INTEL_CANNONLAKE_PCH_H)
#include <soc/gpio_defs_cnp_h.h>
#else
#include <soc/gpio_defs.h>
#endif

#define SOC_INTEL_CML_UART_DEV_MAX 3
#define SOC_INTEL_CML_SATA_DEV_MAX 8

enum chip_pl2_4_cfg {
	baseline,
	performance,
	value_not_set /* vr_config internal use only */
};

struct soc_intel_cannonlake_config {

	/* Common struct containing soc config data required by common code */
	struct soc_intel_common_config common_soc_config;

	/* Common struct containing power limits configuration information */
	struct soc_power_limits_config power_limits_config;

	/* Gpio group routed to each dword of the GPE0 block. Values are
	 * of the form GPP_[A:G] or GPD. */
	uint8_t gpe0_dw0; /* GPE0_31_0 STS/EN */
	uint8_t gpe0_dw1; /* GPE0_63_32 STS/EN */
	uint8_t gpe0_dw2; /* GPE0_95_64 STS/EN */

	/* Generic IO decode ranges */
	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;

	/* S0ix configuration */

	/* Enable S0iX support */
	int s0ix_enable;
	/* Enable Audio DSP oscillator qualification for S0ix */
	uint8_t cppmvric2_adsposcdis;

	/* Enable DPTF support */
	int dptf_enable;

	enum {
		MAX_PC_DEFAULT	= 0,
		MAX_PC0_1	= 1,
		MAX_PC2		= 2,
		MAX_PC3		= 3,
		MAX_PC6		= 4,
		MAX_PC7		= 5,
		MAX_PC7S	= 6,
		MAX_PC8		= 7,
		MAX_PC9		= 8,
		MAX_PC10	= 9,
	} max_package_c_state;

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
	 * For CNL, options are as following
	 * When enabled, memory will be training at three different frequencies.
	 * 0:Disabled, 1:FixedLow, 2:FixedMid, 3:FixedHigh, 4:Enabled
	 * For WHL/CFL/CML options are as following
	 * When enabled, memory will be training at two different frequencies.
	 * 0:Disabled, 1:FixedLow, 2:FixedHigh, 3:Enabled*/
	enum {
		SaGv_Disabled,
		SaGv_FixedLow,
		SaGv_FixedHigh,
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
	/* USB2 PHY power gating */
	uint8_t PchUsb2PhySusPgDisable;

	/* SATA related */
	enum {
		SATA_AHCI,
		SATA_RAID,
	} SataMode;

	/* SATA devslp pad reset configuration */
	enum {
		SataDevSlpResumeReset = 1,
		SataDevSlpHostDeepReset = 3,
		SataDevSlpPlatformReset = 5,
		SataDevSlpDswReset = 7
	} SataDevSlpRstConfig;

	uint8_t SataSalpSupport;
	uint8_t SataPortsEnable[8];
	uint8_t SataPortsDevSlp[8];
	uint8_t SataPortsDevSlpResetConfig[8];
	uint8_t SataPortsHotPlug[8];

	/* Enable/Disable SLP_S0 with GBE Support. 0: disable, 1: enable */
	uint8_t SlpS0WithGbeSupport;
	/* SLP_S0 Voltage Margining Policy. 0: disable, 1: enable */
	uint8_t PchPmSlpS0VmRuntimeControl;
	/* SLP_S0 Voltage Margining  0.70V Policy. 0: disable, 1: enable */
	uint8_t PchPmSlpS0Vm070VSupport;
	/* SLP_S0 Voltage Margining  0.75V Policy. 0: disable, 1: enable */
	uint8_t PchPmSlpS0Vm075VSupport;

	/* Audio related */
	uint8_t PchHdaDspEnable;

	/* Enable/Disable HD Audio Link. Muxed with SSP0/SSP1/SNDW1 */
	uint8_t PchHdaAudioLinkHda;
	uint8_t PchHdaIDispCodecDisconnect;
	uint8_t PchHdaAudioLinkDmic0;
	uint8_t PchHdaAudioLinkDmic1;
	uint8_t PchHdaAudioLinkSsp0;
	uint8_t PchHdaAudioLinkSsp1;
	uint8_t PchHdaAudioLinkSsp2;
	uint8_t PchHdaAudioLinkSndw1;
	uint8_t PchHdaAudioLinkSndw2;
	uint8_t PchHdaAudioLinkSndw3;
	uint8_t PchHdaAudioLinkSndw4;

	/* PCIe Root Ports */
	uint8_t PcieRpEnable[CONFIG_MAX_ROOT_PORTS];
	/* PCIe output clocks type to PCIe devices.
	 * 0-23: PCH rootport, 0x70: LAN, 0x80: unspecified but in use,
	 * 0xFF: not used */
	uint8_t PcieClkSrcUsage[CONFIG_MAX_PCIE_CLOCK_SRC];
	/* PCIe ClkReq-to-ClkSrc mapping, number of clkreq signal assigned to
	 * clksrc. */
	uint8_t PcieClkSrcClkReq[CONFIG_MAX_PCIE_CLOCK_SRC];
	/* PCIe LTR(Latency Tolerance Reporting) mechanism */
	uint8_t PcieRpLtrEnable[CONFIG_MAX_ROOT_PORTS];
	/* Implemented as slot or built-in? */
	uint8_t PcieRpSlotImplemented[CONFIG_MAX_ROOT_PORTS];
	/* Enable/Disable HotPlug support for Root Port */
	uint8_t PcieRpHotPlug[CONFIG_MAX_ROOT_PORTS];

	/*
	 * Enable/Disable AER (Advanced Error Reporting) for Root Port
	 * 0: Disable AER
	 * 1: Enable AER
	 */
	uint8_t PcieRpAdvancedErrorReporting[CONFIG_MAX_ROOT_PORTS];

	/* PCIE RP ASPM, ASPM support for the root port */
	enum {
		AspmDefault,
		AspmDisabled,
		AspmL0s,
		AspmL1,
		AspmL0sL1,
		AspmAutoConfig,
	} PcieRpAspm[CONFIG_MAX_ROOT_PORTS];

	/* PCIE RP Max Payload, Max Payload Size supported */
	enum {
		RpMaxPayload_128,
		RpMaxPayload_256,
	} PcieRpMaxPayload[CONFIG_MAX_ROOT_PORTS];

	/* eMMC and SD */
	uint8_t ScsEmmcHs400Enabled;
	/* Need to update DLL setting to get Emmc running at HS400 speed */
	uint8_t EmmcHs400DllNeed;
	/* 0-39: number of active delay for RX strobe, unit is 125 psec */
	uint8_t EmmcHs400RxStrobeDll1;
	/* 0-78: number of active delay for TX data, unit is 125 psec */
	uint8_t EmmcHs400TxDataDll;
	/* Enable/disable SD card write protect pin configuration on CML */
	uint8_t ScsSdCardWpPinEnabled;

	/* Heci related */
	uint8_t DisableHeciRetry;

	/* Gfx related */
	uint8_t SkipExtGfxScan;

	uint8_t Device4Enable;

	/* CPU PL2/4 Config
	 * Performance: Maximum PLs for maximum performance.
	 * Baseline: Baseline PLs for balanced performance at lower power.
	 */
	enum chip_pl2_4_cfg cpu_pl2_4_cfg;

	/* VrConfig Settings for 5 domains
	 * 0 = System Agent, 1 = IA Core, 2 = Ring,
	 * 3 = GT unsliced,  4 = GT sliced */
	struct vr_config domain_vr_config[NUM_VR_DOMAINS];

	/* Enables support for Teton Glacier hybrid storage device */
	uint8_t TetonGlacierMode;

	/* Enable/Disable EIST. 1b:Enabled, 0b:Disabled */
	uint8_t eist_enable;

	/* Enable C6 DRAM */
	uint8_t enable_c6dram;

	/*
	 * SLP_S3 Minimum Assertion Width Policy
	 *  1 = 60us
	 *  2 = 1ms (default)
	 *  3 = 50ms
	 *  4 = 2s
	 */
	uint8_t PchPmSlpS3MinAssert;

	/*
	 * SLP_S4 Minimum Assertion Width Policy
	 *  1 = 1s
	 *  2 = 2s
	 *  3 = 3s
	 *  4 = 4s (default)
	 */
	uint8_t PchPmSlpS4MinAssert;

	/*
	 * SLP_SUS Minimum Assertion Width Policy
	 *  1 = 0ms
	 *  2 = 500ms
	 *  3 = 1s (default)
	 *  4 = 4s
	 */
	uint8_t PchPmSlpSusMinAssert;

	/*
	 * SLP_A Minimum Assertion Width Policy
	 *  1 = 0ms
	 *  2 = 4s
	 *  3 = 98ms (default)
	 *  4 = 2s
	 */
	uint8_t PchPmSlpAMinAssert;

	/*
	 * PCH PM Reset Power Cycle Duration
	 *  0 = 4s
	 *  1 = 1s
	 *  2 = 2s
	 *  3 = 3s
	 *  4 = 4s (default)
	 *
	 * NOTE: Duration programmed in the PchPmPwrCycDur should never be smaller than the
	 * stretch duration programmed in the following registers -
	 *  - GEN_PMCON_A.SLP_S3_MIN_ASST_WDTH (PchPmSlpS3MinAssert)
	 *  - GEN_PMCON_A.S4MAW (PchPmSlpS4MinAssert)
	 *  - PM_CFG.SLP_A_MIN_ASST_WDTH (PchPmSlpAMinAssert)
	 *  - PM_CFG.SLP_LAN_MIN_ASST_WDTH
	 */
	uint8_t PchPmPwrCycDur;

	/*
	 * SerialIO device mode selection:
	 *
	 * Device index:
	 * PchSerialIoIndexI2C0
	 * PchSerialIoIndexI2C1
	 * PchSerialIoIndexI2C2
	 * PchSerialIoIndexI2C3
	 * PchSerialIoIndexI2C4
	 * PchSerialIoIndexI2C5
	 * PchSerialIoIndexSPI0
	 * PchSerialIoIndexSPI1
	 * PchSerialIoIndexSPI2
	 * PchSerialIoIndexUART0
	 * PchSerialIoIndexUART1
	 * PchSerialIoIndexUART2
	 *
	 * Mode select:
	 * For Cannonlake PCH following values are supported:
	 * PchSerialIoNotInitialized
	 * PchSerialIoDisabled
	 * PchSerialIoPci
	 * PchSerialIoAcpi
	 * PchSerialIoHidden
	 * PchSerialIoMax
	 *
	 * For Cometlake following values are supported:
	 * PchSerialIoNotInitialized
	 * PchSerialIoDisabled,
	 * PchSerialIoPci,
	 * PchSerialIoHidden,
	 * PchSerialIoLegacyUart,
	 * PchSerialIoSkipInit,
	 * PchSerialIoMax
	 *
	 * NOTE:
	 * PchSerialIoNotInitialized is not an option provided by FSP, this
	 * option is default selected in case devicetree doesn't fill this param
	 * In case PchSerialIoNotInitialized is selected or an invalid value is
	 * provided from devicetree, coreboot will configure device into PCI
	 * mode by default.
	 *
	 */
	uint8_t SerialIoDevMode[PchSerialIoIndexMAX];

	enum serirq_mode serirq_mode;

	/* GPIO SD card detect pin */
	unsigned int sdcard_cd_gpio;

	/* Enable Pch iSCLK */
	uint8_t pch_isclk;

	/*
	 * Acoustic Noise Mitigation
	 * 0b - Disable
	 * 1b - Enable noise mitigation
	 */
	uint8_t AcousticNoiseMitigation;

	/*
	 * Disable Fast Package C-state ramping
	 * Need to set AcousticNoiseMitigation = '1' first
	 * 0b - Enabled
	 * 1b - Disabled
	 */
	uint8_t FastPkgCRampDisableIa;
	uint8_t FastPkgCRampDisableGt;
	uint8_t FastPkgCRampDisableSa;
	uint8_t FastPkgCRampDisableFivr;

	/*
	 * Adjust the VR slew rates
	 * Need to set AcousticNoiseMitigation = '1' first
	 * 000b - Fast/2
	 * 001b - Fast/4
	 * 010b - Fast/8
	 * 011b - Fast/16
	 */
	uint8_t SlowSlewRateForIa;
	uint8_t SlowSlewRateForGt;
	uint8_t SlowSlewRateForSa;
	uint8_t SlowSlewRateForFivr;

	/* SATA Power Optimizer */
	uint8_t satapwroptimize;

	/* SATA Gen3 Strength */
	struct sata_port_config sata_port[SOC_INTEL_CML_SATA_DEV_MAX];

	/* Enable or disable eDP device */
	uint8_t DdiPortEdp;

	/* Enable or disable HPD of DDI port B/C/D/F */
	uint8_t DdiPortBHpd;
	uint8_t DdiPortCHpd;
	uint8_t DdiPortDHpd;
	uint8_t DdiPortFHpd;

	/* Enable or disable DDC of DDI port B/C/D/F  */
	uint8_t DdiPortBDdc;
	uint8_t DdiPortCDdc;
	uint8_t DdiPortDDdc;
	uint8_t DdiPortFDdc;

	/* Unlock all GPIO Pads */
	uint8_t PchUnlockGpioPads;

	/* Enable GBE wakeup */
	uint8_t LanWakeFromDeepSx;
	uint8_t WolEnableOverride;

#if !CONFIG(SOC_INTEL_COMETLAKE)
	uint32_t VrPowerDeliveryDesign;
#endif

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

	/*
	 * Override CPU flex ratio value:
	 * CPU ratio value controls the maximum processor non-turbo ratio.
	 * Valid Range 0 to 63.
	 *
	 * In general descriptor provides option to set default cpu flex ratio.
	 * Default cpu flex ratio is 0 ensures booting with non-turbo max frequency.
	 * That's the reason FSP skips cpu_ratio override if cpu_ratio is 0.
	 *
	 * Only override CPU flex ratio if don't want to boot with non-turbo max.
	 */
	uint8_t cpu_ratio_override;

	struct i915_gpu_panel_config panel_cfg;

	struct i915_gpu_controller_info gfx;

	/* Disable CPU Turbo in IA32_MISC_ENABLE */
	bool cpu_turbo_disable;

	bool disable_vmx;
};

typedef struct soc_intel_cannonlake_config config_t;

#endif
