/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <drivers/i2c/designware/dw_i2c.h>
#include <intelblocks/cfg.h>
#include <intelblocks/gpio.h>
#include <intelblocks/gspi.h>
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

struct soc_intel_alderlake_config {

	/* Common struct containing soc config data required by common code */
	struct soc_intel_common_config common_soc_config;

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

	/* USB related */
	struct usb2_port_config usb2_ports[16];
	struct usb3_port_config usb3_ports[10];
	/* Wake Enable Bitmap for USB2 ports */
	uint16_t usb2_wake_enable_bitmap;
	/* Wake Enable Bitmap for USB3 ports */
	uint16_t usb3_wake_enable_bitmap;

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
	uint8_t PchHdaIDispLinkTmode;
	uint8_t PchHdaIDispLinkFrequency;
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

	/* PCIe RP L1 substate */
	enum L1_substates_control {
		L1_SS_FSP_DEFAULT,
		L1_SS_DISABLED,
		L1_SS_L1_1,
		L1_SS_L1_2,
	} PcieRpL1Substates[CONFIG_MAX_ROOT_PORTS];

	/* PCIe LTR: Enable (1) / Disable (0) */
	uint8_t PcieRpLtrEnable[CONFIG_MAX_ROOT_PORTS];

	/* PCIE RP Advanced Error Report: Enable (1) / Disable (0) */
	uint8_t PcieRpAdvancedErrorReporting[CONFIG_MAX_ROOT_PORTS];

	/* Integrated Sensor */
	uint8_t PchIshEnable;

	/* Heci related */
	uint8_t Heci3Enabled;

	/* Gfx related */
	enum {
		IGD_SM_0MB = 0x00,
		IGD_SM_32MB = 0x01,
		IGD_SM_64MB = 0x02,
		IGD_SM_96MB = 0x03,
		IGD_SM_128MB = 0x04,
		IGD_SM_160MB = 0x05,
		IGD_SM_4MB = 0xF0,
		IGD_SM_8MB = 0xF1,
		IGD_SM_12MB = 0xF2,
		IGD_SM_16MB = 0xF3,
		IGD_SM_20MB = 0xF4,
		IGD_SM_24MB = 0xF5,
		IGD_SM_28MB = 0xF6,
		IGD_SM_36MB = 0xF8,
		IGD_SM_40MB = 0xF9,
		IGD_SM_44MB = 0xFA,
		IGD_SM_48MB = 0xFB,
		IGD_SM_52MB = 0xFC,
		IGD_SM_56MB = 0xFD,
		IGD_SM_60MB = 0xFE,
	} IgdDvmt50PreAlloc;
	uint8_t InternalGfx;
	uint8_t SkipExtGfxScan;

	uint32_t GraphicsConfigPtr;

	/* HeciEnabled decides the state of Heci1 at end of boot
	 * Setting to 0 (default) disables Heci1 and hides the device from OS */
	uint8_t HeciEnabled;
	/* PL2 Override value in Watts */
	uint32_t tdp_pl2_override;
	/* Intel Speed Shift Technology */
	uint8_t speed_shift_enable;

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

	/* Debug interface selection */
	enum {
		DEBUG_INTERFACE_RAM = (1 << 0),
		DEBUG_INTERFACE_UART_8250IO = (1 << 1),
		DEBUG_INTERFACE_USB3 = (1 << 3),
		DEBUG_INTERFACE_LPSS_SERIAL_IO = (1 << 4),
		DEBUG_INTERFACE_TRACEHUB = (1 << 5),
	} debug_interface_flag;

	/* Enable Pch iSCLK */
	uint8_t pch_isclk;

	/* CNVi BT Audio Offload: Enable/Disable BT Audio Offload. */
	enum {
		FORCE_DISABLE,
		FORCE_ENABLE,
	} CnviBtAudioOffload;

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
	 * This is a bitfield that corresponds to up to 4 TCSS ports on ADL.
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
	 *
	 * In general descriptor provides option to set default cpu flex ratio.
	 * Default cpu flex ratio is 0 ensures booting with non-turbo max frequency.
	 * That's the reason FSP skips cpu_ratio override if cpu_ratio is 0.
	 *
	 * Only override CPU flex ratio if don't want to boot with non-turbo max.
	 */
	uint8_t cpu_ratio_override;

	/*
	 * Enable(0)/Disable(1) DMI Power Optimizer on PCH side.
	 * Default 0. Setting this to 1 disables the DMI Power Optimizer.
	 */
	uint8_t DmiPwrOptimizeDisable;

	/*
	 * Enable(1)/Disable(0) CPU Replacement check.
	 * Default 0. Setting this to 1 to check CPU replacement.
	 */
	uint8_t CpuReplacementCheck;

	/* ISA Serial Base selection. */
	enum {
		ISA_SERIAL_BASE_ADDR_3F8,
		ISA_SERIAL_BASE_ADDR_2F8,
	} IsaSerialUartBase;
};

typedef struct soc_intel_alderlake_config config_t;

#endif
