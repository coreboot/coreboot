/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <arch/acpi_device.h>
#include <device/i2c_simple.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <intelblocks/gspi.h>
#include <stdint.h>
#include <soc/gpe.h>
#include <soc/gpio.h>
#include <soc/irq.h>
#include <soc/pci_devs.h>
#include <soc/pmc.h>
#include <soc/serialio.h>
#include <soc/usb.h>
#include <soc/vr_config.h>

#define SKYLAKE_I2C_DEV_MAX 6

enum skylake_i2c_voltage {
	I2C_VOLTAGE_3V3,
	I2C_VOLTAGE_1V8
};

struct soc_intel_skylake_config {
	/*
	 * Interrupt Routing configuration
	 * If bit7 is 1, the interrupt is disabled.
	 */
	uint8_t pirqa_routing;
	uint8_t pirqb_routing;
	uint8_t pirqc_routing;
	uint8_t pirqd_routing;
	uint8_t pirqe_routing;
	uint8_t pirqf_routing;
	uint8_t pirqg_routing;
	uint8_t pirqh_routing;

	/* GPE configuration */
	uint32_t gpe0_en_1; /* GPE0_EN_31_0 */
	uint32_t gpe0_en_2; /* GPE0_EN_63_32 */
	uint32_t gpe0_en_3; /* GPE0_EN_95_64 */
	uint32_t gpe0_en_4; /* GPE0_EN_127_96 / GPE_STD */
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
	int tcc_offset;

	/* PL2 Override value in Watts */
	u32 tdp_pl2_override;

	/* SysPL2 Value in Watts */
	u32 tdp_psyspl2;

	/* SysPL3 Value in Watts */
	u32 tdp_psyspl3;
	/* SysPL3 window size */
	u32 tdp_psyspl3_time;
	/* SysPL3 duty cycle */
	u32 tdp_psyspl3_dutycycle;

	/* PL4 Value in Watts */
	u32 tdp_pl4;

	/* Estimated maximum platform power in Watts */
	u16 psys_pmax;

	/*
	 * The following fields come from FspUpdVpd.h.
	 * These are configuration values that are passed to FSP during
	 * MemoryInit.
	 */
	u64 PlatformMemorySize;
	u8 SmramMask;
	u8 MrcFastBoot;
	u32 TsegSize;
	u16 MmioSize;

	/*
	 * DDR Frequency Limit
	 * 0(Auto), 1067, 1333, 1600, 1867, 2133, 2400
	 */
	u16 DdrFreqLimit;

	/* Probeless Trace function */
	u8 ProbelessTrace;

	/*
	 * System Agent dynamic frequency configuration
	 * When enabled memory will be trained at two different frequencies.
	 * 0 = Disabled
	 * 1 = FixedLow
	 * 2 = FixedHigh
	 * 3 = Enabled
	*/
	u8 SaGv;

	/* Enable/disable Rank Margin Tool */
	u8 Rmt;

	/* Lan */
	u8 EnableLan;
	u8 EnableLanLtr;
	u8 EnableLanK1Off;
	u8 LanClkReqSupported;
	u8 LanClkReqNumber;

	/* SATA related */
	u8 EnableSata;
	enum {
		/* Documentation and header files of Skylake FSP disagree on
		   the values, Kaby Lake FSP (KabylakeFsp0001 on github) uses
		   these: */
		KBLFSP_SATA_MODE_AHCI    = 0,
		KBLFSP_SATA_MODE_RAID    = 1,
	} SataMode;
	u8 SataSalpSupport;
	u8 SataPortsEnable[8];
	u8 SataPortsDevSlp[8];
	u8 SataSpeedLimit;

	/* Audio related */
	u8 EnableAzalia;
	u8 DspEnable;

	/*
	 * I/O Buffer Ownership:
	 * 0: HD-A Link
	 * 1 Shared, HD-A Link and I2S Port
	 * 3: I2S Ports
	 */
	u8 IoBufferOwnership;

	/* Trace Hub function */
	u8 EnableTraceHub;
	u32 TraceHubMemReg0Size;
	u32 TraceHubMemReg1Size;

	/* DCI Enable/Disable */
	u8 PchDciEn;

	/*
	 * Pcie Root Port configuration:
	 * each element of array corresponds to
	 * respective PCIe root port.
	 */

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

	/* USB related */
	struct usb2_port_config usb2_ports[16];
	struct usb3_port_config usb3_ports[10];
	u8 SsicPortEnable;

	/* SMBus */
	u8 SmbusEnable;

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
	enum skylake_i2c_voltage i2c_voltage[SKYLAKE_I2C_DEV_MAX];
	struct dw_i2c_bus_config i2c[SKYLAKE_I2C_DEV_MAX];

	/* GSPI */
	struct gspi_cfg gspi[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];

	/* Camera */
	u8 Cio2Enable;
	u8 SaImguEnable;

	/* eMMC and SD */
	u8 ScsEmmcEnabled;
	u8 ScsEmmcHs400Enabled;
	u8 ScsSdCardEnabled;

	/* Integrated Sensor */
	u8 IshEnable;

	u8 PttSwitch;
	u8 HeciTimeouts;
	u8 HsioMessaging;
	u8 Heci3Enabled;

	/* Gfx related */
	u8 IgdDvmt50PreAlloc;
	u8 PrimaryDisplay;
	u8 InternalGfx;
	u8 ApertureSize;
	u8 SkipExtGfxScan;
	u8 ScanExtGfxForLegacyOpRom;

	/*
	 * The following fields come from fsp_vpd.h
	 * These are configuration values that are passed to FSP during
	 * SiliconInit.
	 */
	u32 LogoPtr;
	u32 LogoSize;
	u32 GraphicsConfigPtr;
	u8 Device4Enable;
	u8 RtcLock;
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
	 * in the upper and and lower 128-byte bank of RTC RAM.
	 */
	u8 LockDownConfigRtcLock;
	/* Subsystem Vendor ID of the PCH devices*/
	u16 PchConfigSubSystemVendorId;
	/* Subsystem ID of the PCH devices*/
	u16 PchConfigSubSystemId;

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
	 * This member describes whether or not the PCI ClockRun feature of PCH
	 * should be enabled. Values 0: Disabled, 1: Enabled
	 */
	u8 PmConfigPciClockRun;
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

	/* Determines if enable Serial IRQ. Values 0: Disabled, 1: Enabled.*/
	u8 SerialIrqConfigSirqEnable;

	enum {
		SERIAL_IRQ_QUIET_MODE      = 0,
		SERIAL_IRQ_CONTINUOUS_MODE = 1,
	} SerialIrqConfigSirqMode;

	enum {
		SERIAL_IRQ_FRAME_PULSE_4CLK = 0,
		SERIAL_IRQ_FRAME_PULSE_6CLK = 1,
		SERIAL_IRQ_FRAME_PULSE_8CLK = 2,
	} SerialIrqConfigStartFramePulse;

	u8 FspSkipMpInit;

	/*
	 * VrConfig Settings for 5 domains
	 * 0 = System Agent, 1 = IA Core, 2 = Ring,
	 * 3 = GT unsliced,  4 = GT sliced
	 */
	struct vr_config domain_vr_config[NUM_VR_DOMAINS];
	/*
	 * HeciEnabled decides the state of Heci1 at end of boot
	 * Setting to 0 (default) disables Heci1 and hides the device from OS
	 */
	u8 HeciEnabled;
	u8 PmTimerDisabled;
	/* Intel Speed Shift Technology */
	u8 speed_shift_enable;
	/*
	 * Enable VR specific mailbox command
	 * 000b - Don't Send any VR command
	 * 001b - VR command specifically for the MPS IMPV8 VR will be sent
	 * 010b - VR specific command sent for PS4 exit issue
	 * 011b - VR specific command sent for both MPS IMPV8 & PS4 exit issue
	*/
	u8 SendVrMbxCmd;

	/* Enable/Disable VMX feature */
	u8 VmxEnable;

	/*
	 * PRMRR size setting with three options
	 * 0x02000000 - 32MiB
	 * 0x04000000 - 64MiB
	 * 0x08000000 - 128MiB
	 */
	u32 PrmrrSize;

	/* Enable/Disable host reads to PMC XRAM registers */
	u8 PchPmPmcReadDisable;

	/* Statically clock gate 8254 PIT. */
	u8 clock_gate_8254;

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
	 *          { sdcard_cd_gpio_default }
	 */
	unsigned int sdcard_cd_gpio_default;

	/* Use custom SD card detect GPIO configuration */
	struct acpi_gpio sdcard_cd_gpio;

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
	/* FSP 1.1 */
	u8 FastPkgCRampDisable;
	/* FSP 2.0 */
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

	/* Enable SGX feature */
	u8 sgx_enable;

	/* Enable/Disable EIST
	 * 1b - Enabled
	 * 0b - Disabled
	 */
	u8 eist_enable;
	/* Chipset (LPC and SPI)  Lock Down
	 * 1b - coreboot to handle lockdown
	 * 0b - FSP to handle lockdown
	 */
	enum {
		/* lock according to binary UPD settings */
		CHIPSET_LOCKDOWN_FSP,
		/* coreboot handles locking */
		CHIPSET_LOCKDOWN_COREBOOT,
	} chipset_lockdown;

	/*
	 * Activates VR mailbox command for Intersil VR C-state issues.
	 * 0 - no mailbox command sent.
	 * 1 - VR mailbox command sent for IA/GT rails only.
	 * 2 - VR mailbox command sent for IA/GT/SA rails.
	 */
	u8 IslVrCmd;

	/* PCH Trip Temperature */
	u8 pch_trip_temp;

	/* Enable/Disable Sata power optimization */
	u8 SataPwrOptEnable;
};

typedef struct soc_intel_skylake_config config_t;

extern struct chip_operations soc_ops;

#endif
