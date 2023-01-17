/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <acpi/acpi_device.h>
#include <device/i2c_simple.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <drivers/intel/gma/gma.h>
#include <gpio.h>
#include <intelblocks/cfg.h>
#include <intelblocks/gspi.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/power_limit.h>
#include <stdint.h>
#include <soc/gpe.h>
#include <soc/irq.h>
#include <soc/pci_devs.h>
#include <soc/pmc.h>
#include <soc/serialio.h>
#include <soc/usb.h>
#include <soc/vr_config.h>

#define MAX_PEG_PORTS	3

enum skylake_i2c_voltage {
	I2C_VOLTAGE_3V3,
	I2C_VOLTAGE_1V8
};

struct soc_intel_skylake_config {

	/* Common struct containing soc config data required by common code */
	struct soc_intel_common_config common_soc_config;

	/* Common struct containing power limits configuration information */
	struct soc_power_limits_config power_limits_config;

	/* IGD panel configuration */
	struct i915_gpu_panel_config panel_cfg;

	/* Gpio group routed to each dword of the GPE0 block. Values are
	 * of the form GPP_[A:G] or GPD. */
	uint8_t gpe0_dw0; /* GPE0_31_0 STS/EN */
	uint8_t gpe0_dw1; /* GPE0_63_32 STS/EN */
	uint8_t gpe0_dw2; /* GPE0_95_64 STS/EN */

	/* LPC fixed enables and ranges */
	uint16_t lpc_iod;
	uint16_t lpc_ioe;

	/* Generic IO decode ranges */
	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;

	/* Enable S0iX support */
	int s0ix_enable;

	/* Enable DPTF support */
	int dptf_enable;

	/* Deep SX enables */
	int deep_s3_enable_ac;
	int deep_s3_enable_dc;
	int deep_s5_enable_ac;
	int deep_s5_enable_dc;

	/*
	 * Deep Sx Configuration
	 *  DSX_EN_WAKE_PIN       - Enable WAKE# pin
	 *  DSX_EN_LAN_WAKE_PIN   - Enable LAN_WAKE# pin
	 *  DSX_DIS_AC_PRESENT_PD - Disable pull-down on AC_PRESENT pin
	 */
	uint32_t deep_sx_config;

	/* TCC activation offset */
	uint32_t tcc_offset;

	/*
	 * System Agent dynamic frequency configuration
	 * When enabled memory will be trained at two different frequencies.
	 * 0 = Disabled
	 * 1 = FixedLow
	 * 2 = FixedHigh
	 * 3 = Enabled
	*/
	enum {
		SaGv_Disabled,
		SaGv_FixedLow,
		SaGv_FixedHigh,
		SaGv_Enabled,
	} SaGv;

	/* Enable/disable Rank Margin Tool */
	u8 RMT;

	/* Disable Command TriState */
	u8 CmdTriStateDis;

	/* Lan */
	u8 EnableLanLtr;
	u8 EnableLanK1Off;
	u8 LanClkReqSupported;
	u8 LanClkReqNumber;

	/* SATA related */
	enum {
		SATA_AHCI    = 0,
		SATA_RAID    = 1,
	} SataMode;
	u8 SataSalpSupport;
	u8 SataPortsEnable[8];
	u8 SataPortsDevSlp[8];
	u8 SataPortsSpinUp[8];
	u8 SataPortsHotPlug[8];
	u8 SataSpeedLimit;

	/* Audio related */
	u8 DspEnable;

	/* HDA Virtual Channel Type Select */
	enum {
		Vc0,
		Vc1,
	} PchHdaVcType;

	/*
	 * I/O Buffer Ownership:
	 * 0: HD-A Link
	 * 1 Shared, HD-A Link and I2S Port
	 * 3: I2S Ports
	 */
	u8 IoBufferOwnership;

	/* Trace Hub function */
	u32 TraceHubMemReg0Size;
	u32 TraceHubMemReg1Size;

	/* DCI Enable/Disable */
	u8 PchDciEn;

	/*
	 * PCIe Root Port configuration:
	 * each element of array corresponds to
	 * respective PCIe root port.
	 */

	/* PEG Max Link Width */
	enum {
		Peg0_x16,
		Peg0_x1,
		Peg0_x2,
		Peg0_x4,
		Peg0_x8,
	} Peg0MaxLinkWidth;

	enum {
		Peg1_x8,
		Peg1_x1,
		Peg1_x2,
		Peg1_x4,
	} Peg1MaxLinkWidth;

	enum {
		Peg2_x4,
		Peg2_x1,
		Peg2_x2,
	} Peg2MaxLinkWidth;

	/*
	 * Enable/Disable Root Port
	 * 0: Disable Root Port
	 * 1: Enable Root Port
	 */
	u8 PcieRpEnable[CONFIG_MAX_ROOT_PORTS];

	/*
	 * Enable/Disable Clk-req support for Root Port
	 * 0: Disable Clk-Req
	 * 1: Enable Clk-req
	 */
	u8 PcieRpClkReqSupport[CONFIG_MAX_ROOT_PORTS];

	/*
	 * Clk-req source for Root Port
	 */
	u8 PcieRpClkReqNumber[CONFIG_MAX_ROOT_PORTS];

	/*
	 * Clk source number for Root Port
	 */
	u8 PcieRpClkSrcNumber[CONFIG_MAX_ROOT_PORTS];

	/*
	 * Enable/Disable AER (Advanced Error Reporting) for Root Port
	 * 0: Disable AER
	 * 1: Enable AER
	 */
	u8 PcieRpAdvancedErrorReporting[CONFIG_MAX_ROOT_PORTS];

	/*
	 * Enable/Disable Latency Tolerance Reporting for Root Port
	 * 0: Disable LTR
	 * 1: Enable LTR
	 */
	u8 PcieRpLtrEnable[CONFIG_MAX_ROOT_PORTS];

	/* Enable/Disable HotPlug support for Root Port */
	u8 PcieRpHotPlug[CONFIG_MAX_ROOT_PORTS];

	/* PCIE RP Max Payload, Max Payload Size supported */
	enum {
		RpMaxPayload_128,
		RpMaxPayload_256,
	} PcieRpMaxPayload[CONFIG_MAX_ROOT_PORTS];

	/* PCIE RP ASPM, ASPM support for the root port */
	enum {
		AspmDefault,
		AspmDisabled,
		AspmL0s,
		AspmL1,
		AspmL0sL1,
		AspmAutoConfig,
	} pcie_rp_aspm[CONFIG_MAX_ROOT_PORTS];

	/* PCIe RP L1 substate */
	enum {
		L1SS_Default,
		L1SS_Disabled,
		L1SS_L1_1,
		L1SS_L1_2,
	} pcie_rp_l1substates[CONFIG_MAX_ROOT_PORTS];

	/* USB related */
	struct usb2_port_config usb2_ports[16];
	struct usb3_port_config usb3_ports[10];
	u8 SsicPortEnable;

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
	 * PchSerialIoIndexI2C6
	 * PchSerialIoIndexSpi0
	 * PchSerialIoIndexSpi1
	 * PchSerialIoIndexUart0
	 * PchSerialIoIndexUart1
	 * PchSerialIoIndexUart2
	 *
	 * Mode select:
	 * PchSerialIoDisabled
	 * PchSerialIoAcpi
	 * PchSerialIoPci
	 * PchSerialIoAcpiHidden
	 * PchSerialIoLegacyUart
	 */
	u8 SerialIoDevMode[PchSerialIoIndexMax];

	/* I2C */
	/* Bus voltage level, default is 3.3V */
	enum skylake_i2c_voltage i2c_voltage[CONFIG_SOC_INTEL_I2C_DEV_MAX];

	/* eMMC and SD */
	u8 ScsEmmcHs400Enabled;
	u8 EmmcHs400DllNeed;
	u8 ScsEmmcHs400RxStrobeDll1;
	u8 ScsEmmcHs400TxDataDll;

	enum {
		Display_iGFX,
		Display_PEG,
		Display_PCH_PCIe,
		Display_Auto,
		Display_Switchable,
	} PrimaryDisplay;
	u8 SkipExtGfxScan;

	/* GPIO IRQ Route  The valid values is 14 or 15*/
	u8 GpioIrqSelect;
	/* SCI IRQ Select  The valid values is 9, 10, 11 and 20 21, 22, 23*/
	u8 SciIrqSelect;
	/* TCO IRQ Select  The valid values is 9, 10, 11, 20 21, 22, 23*/
	u8 TcoIrqSelect;
	u8 TcoIrqEnable;
	/* Enable SMI_LOCK bit to prevent writes to the Global SMI Enable bit.*/
	u8 LockDownConfigGlobalSmi;
	/*
	 * Enable RTC lower and upper 128 byte Lock bits to lock Bytes 38h-3Fh
	 * in the upper and lower 128-byte bank of RTC RAM.
	 */
	u8 LockDownConfigRtcLock;

	/*
	 * Determine if WLAN wake from Sx, corresponds to the
	 * HOST_WLAN_PP_EN bit in the PWRM_CFG3 register.
	 */
	u8 PchPmWoWlanEnable;

	/*
	 * Determine if WLAN wake from DeepSx, corresponds to
	 * the DSX_WLAN_PP_EN bit in the PWRM_CFG3 register.
	 */
	u8 PchPmWoWlanDeepSxEnable;

	/*
	 * Corresponds to the "WOL Enable Override" bit in the General PM
	 * Configuration B (GEN_PMCON_B) register
	 */
	u8 WakeConfigWolEnableOverride;
	/* Determine if enable PCIe to wake from deep Sx*/
	u8 WakeConfigPcieWakeFromDeepSx;
	/* Deep Sx Policy. Values 0: PchDeepSxPolDisable,
	 * 1: PchDpS5BatteryEn, 2: PchDpS5AlwaysEn, 3: PchDpS4S5BatteryEn,
	 * 4: PchDpS4S5AlwaysEn, 5: PchDpS3S4S5BatteryEn, 6: PchDpS3S4S5AlwaysEn
	 */
	u8 PmConfigDeepSxPol;

	enum {
		SLP_S3_MIN_ASSERT_60US = 0,
		SLP_S3_MIN_ASSERT_1MS  = 1,
		SLP_S3_MIN_ASSERT_50MS = 2,
		SLP_S3_MIN_ASSERT_2S   = 3,
	} PmConfigSlpS3MinAssert;

	enum {
		SLP_S4_MIN_ASSERT_PCH = 0,
		SLP_S4_MIN_ASSERT_1S  = 1,
		SLP_S4_MIN_ASSERT_2S  = 2,
		SLP_S4_MIN_ASSERT_3S  = 3,
		SLP_S4_MIN_ASSERT_4S  = 4,
	} PmConfigSlpS4MinAssert;

	/* When deep Sx enabled: Must be greater than or equal to
	                         all other minimum assertion widths. */
	enum {
		SLP_SUS_MIN_ASSERT_0MS   = 0,
		SLP_SUS_MIN_ASSERT_500MS = 1,
		SLP_SUS_MIN_ASSERT_1S    = 2,
		SLP_SUS_MIN_ASSERT_4S    = 3,
	} PmConfigSlpSusMinAssert;

	enum {
		SLP_A_MIN_ASSERT_0MS  = 0,
		SLP_A_MIN_ASSERT_4S   = 1,
		SLP_A_MIN_ASSERT_98MS = 2,
		SLP_A_MIN_ASSERT_2S   = 3,
	} PmConfigSlpAMinAssert;

	/*
	 * SLP_X Stretching After SUS Well Power Up. Values 0: Disabled,
	 * 1: Enabled
	 */
	u8 PmConfigSlpStrchSusUp;
	/*
	 * PCH power button override period.
	 * Values: 0x0 - 4s, 0x1 - 6s, 0x2 - 8s, 0x3 - 10s, 0x4 - 12s, 0x5 - 14s
	 */
	u8 PmConfigPwrBtnOverridePeriod;

	/*
	 * PCH Pm Slp S0 Voltage Margining Enable
	 * Indicates platform supports VCCPrim_Core Voltage Margining
	 * in SLP_S0# asserted state.
	 */
	u8 PchPmSlpS0VmEnable;

	enum {
		RESET_POWER_CYCLE_DEFAULT = 0,
		RESET_POWER_CYCLE_1S      = 1,
		RESET_POWER_CYCLE_2S      = 2,
		RESET_POWER_CYCLE_3S      = 3,
		RESET_POWER_CYCLE_4S      = 4,
	} PmConfigPwrCycDur;

	enum serirq_mode serirq_mode;

	enum {
		SERIAL_IRQ_FRAME_PULSE_4CLK = 0,
		SERIAL_IRQ_FRAME_PULSE_6CLK = 1,
		SERIAL_IRQ_FRAME_PULSE_8CLK = 2,
	} SerialIrqConfigStartFramePulse;

	/*
	 * VrConfig Settings for 5 domains
	 * 0 = System Agent, 1 = IA Core, 2 = Ring,
	 * 3 = GT unsliced,  4 = GT sliced
	 */
	struct vr_config domain_vr_config[NUM_VR_DOMAINS];

	/*
	 * Enable VR specific mailbox command
	 * 000b - Don't Send any VR command
	 * 001b - VR command specifically for the MPS IMPV8 VR will be sent
	 * 010b - VR specific command sent for PS4 exit issue
	 * 011b - VR specific command sent for both MPS IMPV8 & PS4 exit issue
	*/
	u8 SendVrMbxCmd;

	/* Enable/Disable host reads to PMC XRAM registers */
	u8 PchPmPmcReadDisable;

	/*
	 * Use SD card detect GPIO with default config:
	 * - Edge triggered
	 * - No internal pull
	 * - Active both (high + low)
	 * - Can wake device from D3
	 * - 100ms debounce timeout
	 *
	 * GpioInt (Edge, ActiveBoth, SharedAndWake, PullNone, 10000,
	 *          "\\_SB.PCI0.GPIO", 0, ResourceConsumer)
	 *          { sdcard_cd_gpio }
	 */
	unsigned int sdcard_cd_gpio;

	/* Wake Enable Bitmap for USB2 ports */
	u16 usb2_wake_enable_bitmap;

	/* Wake Enable Bitmap for USB3 ports */
	u8 usb3_wake_enable_bitmap;

	/*
	 * Acoustic Noise Mitigation
	 * 0b - Disable
	 * 1b - Enable noise mitigation
	 */
	u8 AcousticNoiseMitigation;

	/*
	 * Disable Fast Package C-state ramping
	 * Need to set AcousticNoiseMitigation = '1' first
	 * 0b - Enabled
	 * 1b - Disabled
	 */
	u8 FastPkgCRampDisableIa;
	u8 FastPkgCRampDisableGt;
	u8 FastPkgCRampDisableSa;

	/*
	 * Adjust the VR slew rates
	 * Need to set AcousticNoiseMitigation = '1' first
	 * 000b - Fast/2
	 * 001b - Fast/4
	 * 010b - Fast/8
	 * 011b - Fast/16
	 */
	u8 SlowSlewRateForIa;
	u8 SlowSlewRateForGt;
	u8 SlowSlewRateForSa;

	/* Enable/Disable EIST
	 * 1b - Enabled
	 * 0b - Disabled
	 */
	u8 eist_enable;

	/*
	 * Activates VR mailbox command for Intersil VR C-state issues.
	 * 0 - no mailbox command sent.
	 * 1 - VR mailbox command sent for IA/GT rails only.
	 * 2 - VR mailbox command sent for IA/GT/SA rails.
	 */
	u8 IslVrCmd;

	/* i915 struct for GMA backlight control */
	struct i915_gpu_controller_info gfx;
};

typedef struct soc_intel_skylake_config config_t;

#endif
