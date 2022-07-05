/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <drivers/i2c/designware/dw_i2c.h>
#include <device/pci_ids.h>
#include <intelblocks/cfg.h>
#include <intelblocks/gpio.h>
#include <intelblocks/gspi.h>
#include <intelblocks/power_limit.h>
#include <intelblocks/pcie_rp.h>
#include <intelblocks/tcss.h>
#include <soc/gpe.h>
#include <soc/pci_devs.h>
#include <soc/pmc.h>
#include <soc/serialio.h>
#include <soc/usb.h>
#include <soc/vr_config.h>
#include <stdint.h>

/* Types of different SKUs */
enum soc_intel_alderlake_power_limits {
	ADL_P_142_242_282_15W_CORE,
	ADL_P_282_482_28W_CORE,
	ADL_P_682_28W_CORE,
	ADL_P_442_482_45W_CORE,
	ADL_P_642_682_45W_CORE,
	ADL_M_282_12W_CORE,
	ADL_M_282_15W_CORE,
	ADL_M_242_CORE,
	ADL_P_442_45W_CORE,
	ADL_N_081_15W_CORE,
	ADL_N_041_6W_CORE,
	ADL_N_021_6W_CORE,
	ADL_S_882_35W_CORE,
	ADL_S_882_65W_CORE,
	ADL_S_882_125W_CORE,
	ADL_S_842_35W_CORE,
	ADL_S_842_65W_CORE,
	ADL_S_842_125W_CORE,
	ADL_S_642_125W_CORE,
	ADL_S_602_35W_CORE,
	ADL_S_602_65W_CORE,
	RPL_P_682_642_482_45W_CORE,
	RPL_P_682_482_282_28W_CORE,
	RPL_P_282_242_142_15W_CORE,
	ADL_POWER_LIMITS_COUNT
};

/* TDP values for different SKUs */
enum soc_intel_alderlake_cpu_tdps {
	TDP_6W  = 6,
	TDP_9W  = 9,
	TDP_12W = 12,
	TDP_15W = 15,
	TDP_28W = 28,
	TDP_35W = 35,
	TDP_45W = 45,
	TDP_65W = 65,
	TDP_125W = 125
};

/* Mapping of different SKUs based on CPU ID and TDP values */
static const struct {
	unsigned int cpu_id;
	enum soc_intel_alderlake_power_limits limits;
	enum soc_intel_alderlake_cpu_tdps cpu_tdp;
} cpuid_to_adl[] = {
	{ PCI_DID_INTEL_ADL_P_ID_10, ADL_P_142_242_282_15W_CORE, TDP_15W },
	{ PCI_DID_INTEL_ADL_P_ID_7, ADL_P_142_242_282_15W_CORE, TDP_15W },
	{ PCI_DID_INTEL_ADL_P_ID_6, ADL_P_142_242_282_15W_CORE, TDP_15W },
	{ PCI_DID_INTEL_ADL_P_ID_7, ADL_P_282_482_28W_CORE, TDP_28W },
	{ PCI_DID_INTEL_ADL_P_ID_5, ADL_P_282_482_28W_CORE, TDP_28W },
	{ PCI_DID_INTEL_ADL_P_ID_3, ADL_P_682_28W_CORE, TDP_28W },
	{ PCI_DID_INTEL_ADL_P_ID_5, ADL_P_442_482_45W_CORE, TDP_45W },
	{ PCI_DID_INTEL_ADL_P_ID_4, ADL_P_642_682_45W_CORE, TDP_45W },
	{ PCI_DID_INTEL_ADL_P_ID_3, ADL_P_642_682_45W_CORE, TDP_45W },
	{ PCI_DID_INTEL_ADL_P_ID_1, ADL_P_442_482_45W_CORE, TDP_45W },
	{ PCI_DID_INTEL_ADL_M_ID_1, ADL_M_282_12W_CORE, TDP_12W },
	{ PCI_DID_INTEL_ADL_M_ID_1, ADL_M_282_15W_CORE, TDP_15W },
	{ PCI_DID_INTEL_ADL_M_ID_2, ADL_M_242_CORE, TDP_9W },
	{ PCI_DID_INTEL_ADL_N_ID_1, ADL_N_081_15W_CORE, TDP_15W },
	{ PCI_DID_INTEL_ADL_N_ID_2, ADL_N_041_6W_CORE, TDP_6W },
	{ PCI_DID_INTEL_ADL_N_ID_3, ADL_N_041_6W_CORE, TDP_6W },
	{ PCI_DID_INTEL_ADL_N_ID_4, ADL_N_021_6W_CORE, TDP_6W },
	{ PCI_DID_INTEL_ADL_S_ID_1, ADL_S_882_35W_CORE, TDP_35W },
	{ PCI_DID_INTEL_ADL_S_ID_1, ADL_S_882_65W_CORE, TDP_65W },
	{ PCI_DID_INTEL_ADL_S_ID_1, ADL_S_882_125W_CORE, TDP_125W },
	{ PCI_DID_INTEL_ADL_S_ID_3, ADL_S_842_35W_CORE, TDP_35W },
	{ PCI_DID_INTEL_ADL_S_ID_3, ADL_S_842_65W_CORE, TDP_65W },
	{ PCI_DID_INTEL_ADL_S_ID_3, ADL_S_842_125W_CORE, TDP_125W },
	{ PCI_DID_INTEL_ADL_S_ID_8, ADL_S_642_125W_CORE, TDP_125W },
	{ PCI_DID_INTEL_ADL_S_ID_10, ADL_S_602_35W_CORE, TDP_35W },
	{ PCI_DID_INTEL_ADL_S_ID_10, ADL_S_602_65W_CORE, TDP_65W },
	{ PCI_DID_INTEL_RPL_P_ID_1, RPL_P_682_642_482_45W_CORE, TDP_45W },
	{ PCI_DID_INTEL_RPL_P_ID_2, RPL_P_682_482_282_28W_CORE, TDP_28W },
	{ PCI_DID_INTEL_RPL_P_ID_3, RPL_P_282_242_142_15W_CORE, TDP_15W },
};

/* Types of display ports */
enum ddi_ports {
	DDI_PORT_A,
	DDI_PORT_B,
	DDI_PORT_C,
	DDI_PORT_1,
	DDI_PORT_2,
	DDI_PORT_3,
	DDI_PORT_4,
	DDI_PORT_COUNT,
};

enum ddi_port_flags {
	DDI_ENABLE_DDC = 1 << 0,
	DDI_ENABLE_HPD = 1 << 1,
};

/*
 * Enable External V1P05/Vnn/VnnSx Rail in: BIT0:S0i1/S0i2,
 * BIT1:S0i3, BIT2:S3, BIT3:S4, BIT4:S5, BIT5:S0.
 */
enum fivr_enable_states {
	FIVR_ENABLE_S0i1_S0i2	= BIT(0),
	FIVR_ENABLE_S0i3	= BIT(1),
	FIVR_ENABLE_S3		= BIT(2),
	FIVR_ENABLE_S4		= BIT(3),
	FIVR_ENABLE_S5		= BIT(4),
	FIVR_ENABLE_S0		= BIT(5),
};

/*
 * Enable the following for External V1p05 rail
 * BIT0: Retention active switch support
 * BIT1: Normal Active voltage supported
 * BIT2: Minimum active voltage supported
 * BIT3: Minimum Retention voltage supported
 */
enum fivr_voltage_supported {
	FIVR_RET_ACTIVE_SWITCH_SUPPORT	= BIT(0),
	FIVR_VOLTAGE_NORMAL		= BIT(1),
	FIVR_VOLTAGE_MIN_ACTIVE		= BIT(2),
	FIVR_VOLTAGE_MIN_RETENTION	= BIT(3),
};

#define FIVR_ENABLE_ALL_SX (FIVR_ENABLE_S0i1_S0i2 | FIVR_ENABLE_S0i3 |	\
			    FIVR_ENABLE_S3 | FIVR_ENABLE_S4 | FIVR_ENABLE_S5 | FIVR_ENABLE_S0)
/*
 * The Max Pkg Cstate
 * Values 0 - C0/C1, 1 - C2, 2 - C3, 3 - C6, 4 - C7, 5 - C7S, 6 - C8, 7 - C9, 8 - C10,
 * 254 - CPU Default , 255 - Auto.
 */
enum pkgcstate_limit {
	LIMIT_C0_C1      = 0,
	LIMIT_C2         = 1,
	LIMIT_C3         = 2,
	LIMIT_C6         = 3,
	LIMIT_C7         = 4,
	LIMIT_C7S        = 5,
	LIMIT_C8         = 6,
	LIMIT_C9         = 7,
	LIMIT_C10        = 8,
	LIMIT_CPUDEFAULT = 254,
	LIMIT_AUTO       = 255,
};

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
 * FivrSpreadSpectrum:
 * Values
 *  0 - 0.5%, 3 - 1%, 8 - 1.5%, 18 - 2%, 28 - 3%, 34 - 4%, 39 - 5%, 44 - 6%
 */
enum fivr_spread_spectrum_ratio {
	FIVR_SS_0_5 = 0,
	FIVR_SS_1 = 3,
	FIVR_SS_1_5 = 8,
	FIVR_SS_2 = 18,
	FIVR_SS_3 = 28,
	FIVR_SS_4 = 34,
	FIVR_SS_5 = 39,
	FIVR_SS_6 = 44,
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

struct soc_intel_alderlake_config {

	/* Common struct containing soc config data required by common code */
	struct soc_intel_common_config common_soc_config;

	/* Common struct containing power limits configuration information */
	struct soc_power_limits_config power_limits_config[ADL_POWER_LIMITS_COUNT];

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
	uint8_t tcss_d3_hot_disable;
	/* Support for TBT PCIe root ports and DMA controllers with D3Hot->D3Cold */
	uint8_t tcss_d3_cold_disable;
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
	} sagv;

	/* Rank Margin Tool. 1:Enable, 0:Disable */
	uint8_t RMT;

	/* USB related */
	struct usb2_port_config usb2_ports[16];
	struct usb3_port_config usb3_ports[10];
	/* Wake Enable Bitmap for USB2 ports */
	uint16_t usb2_wake_enable_bitmap;
	/* Wake Enable Bitmap for USB3 ports */
	uint16_t usb3_wake_enable_bitmap;
	/* Program OC pins for TCSS */
	struct tcss_port_config tcss_ports[MAX_TYPE_C_PORTS];

	/* SATA related */
	uint8_t sata_mode;
	uint8_t sata_salp_support;
	uint8_t sata_ports_enable[8];
	uint8_t sata_ports_dev_slp[8];

	/*
	 * Enable(0)/Disable(1) SATA Power Optimizer on PCH side.
	 * Default 0. Setting this to 1 disables the SATA Power Optimizer.
	 */
	uint8_t sata_pwr_optimize_disable;

	/*
	 * SATA Port Enable Dito Config.
	 * Enable DEVSLP Idle Timeout settings (DmVal, DitoVal).
	 */
	uint8_t sata_ports_enable_dito_config[8];

	/* SataPortsDmVal is the DITO multiplier. Default is 15. */
	uint8_t sata_ports_dm_val[8];

	/* SataPortsDitoVal is the DEVSLP Idle Timeout, default is 625ms */
	uint16_t sata_ports_dito_val[8];

	/* Audio related */
	uint8_t pch_hda_dsp_enable;

	/* iDisp-Link T-Mode 0: 2T, 2: 4T, 3: 8T, 4: 16T */
	enum {
		HDA_TMODE_2T = 0,
		HDA_TMODE_4T = 2,
		HDA_TMODE_8T = 3,
		HDA_TMODE_16T = 4,
	} pch_hda_idisp_link_tmode;

	/* iDisp-Link Freq 4: 96MHz, 3: 48MHz. */
	enum {
		HDA_LINKFREQ_48MHZ = 3,
		HDA_LINKFREQ_96MHZ = 4,
	} pch_hda_idisp_link_frequency;

	bool pch_hda_idisp_codec_enable;

	struct pcie_rp_config pch_pcie_rp[CONFIG_MAX_PCH_ROOT_PORTS];
	struct pcie_rp_config cpu_pcie_rp[CONFIG_MAX_CPU_ROOT_PORTS];
	uint8_t pcie_clk_config_flag[CONFIG_MAX_PCIE_CLOCK_SRC];

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
	} igd_dvmt50_pre_alloc;
	uint8_t skip_ext_gfx_scan;

	/* Enable/Disable EIST. 1b:Enabled, 0b:Disabled */
	uint8_t eist_enable;

	/* Enable C6 DRAM */
	uint8_t enable_c6dram;

	/*
	 * SerialIO device mode selection:
	 * PchSerialIoDisabled,
	 * PchSerialIoPci,
	 * PchSerialIoHidden,
	 * PchSerialIoLegacyUart,
	 * PchSerialIoSkipInit
	 */
	uint8_t serial_io_i2c_mode[CONFIG_SOC_INTEL_I2C_DEV_MAX];
	uint8_t serial_io_gspi_mode[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];
	uint8_t serial_io_uart_mode[CONFIG_SOC_INTEL_UART_DEV_MAX];
	/*
	 * GSPIn Default Chip Select Mode:
	 * 0:Hardware Mode,
	 * 1:Software Mode
	 */
	uint8_t serial_io_gspi_cs_mode[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];
	/*
	 * GSPIn Default Chip Select State:
	 * 0: Low,
	 * 1: High
	 */
	uint8_t serial_io_gspi_cs_state[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];

	/* Enable Pch iSCLK */
	uint8_t pch_isclk;

	/* CNVi BT Core Enable/Disable */
	bool cnvi_bt_core;

	/* CNVi BT Audio Offload: Enable/Disable BT Audio Offload. */
	bool cnvi_bt_audio_offload;

	/*
	 * These GPIOs will be programmed by the IOM to handle biasing of the
	 * Type-C aux (SBU) signals when certain alternate modes are used.
	 * `pad_auxn_dc` should be assigned to the GPIO pad providing negative
	 * bias (name usually contains `AUXN_DC` or `AUX_N`); similarly,
	 * `pad_auxp_dc` should be assigned to the GPIO providing positive bias
	 * (name often contains `AUXP_DC` or `_AUX_P`).
	 */
	struct typec_aux_bias_pads typec_aux_bias_pads[MAX_TYPE_C_PORTS];

	/*
	 * SOC Aux orientation override:
	 * This is a bitfield that corresponds to up to 4 TCSS ports on ADL.
	 * Even numbered bits (0, 2, 4, 6) control the retimer being handled by SOC.
	 * Odd numbered bits (1, 3, 5, 7) control the orientation of the physical aux lines
	 * on the motherboard.
	 */
	uint16_t tcss_aux_ori;

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
	uint8_t ddi_portA_config;
	uint8_t ddi_portB_config;

	/* Enable(1)/Disable(0) HPD/DDC */
	uint8_t ddi_ports_config[DDI_PORT_COUNT];

	/* Hybrid storage mode enable (1) / disable (0)
	 * This mode makes FSP detect Optane and NVME and set PCIe lane mode
	 * accordingly */
	uint8_t hybrid_storage_mode;

#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
	/* eMMC HS400 mode */
	uint8_t emmc_enable_hs400_mode;
#endif

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
	uint8_t dmi_power_optimize_disable;

	/*
	 * Used to communicate the power delivery design capability of the board. This
	 * value is an enum of the available power delivery segments that are defined in
	 * the Platform Design Guide.
	 */
	uint8_t vr_power_delivery_design;

	/*
	 * Enable(1)/Disable(0) CPU Replacement check.
	 * Default 0. Setting this to 1 to check CPU replacement.
	 */
	uint8_t cpu_replacement_check;

	/* ISA Serial Base selection. */
	enum {
		ISA_SERIAL_BASE_ADDR_3F8,
		ISA_SERIAL_BASE_ADDR_2F8,
	} isa_serial_uart_base;

	/* structure containing various settings for PCH FIVRs */
	struct {
		bool configure_ext_fivr;
		enum fivr_enable_states v1p05_enable_bitmap;
		enum fivr_enable_states vnn_enable_bitmap;
		enum fivr_enable_states vnn_sx_enable_bitmap;
		enum fivr_voltage_supported v1p05_supported_voltage_bitmap;
		enum fivr_voltage_supported vnn_supported_voltage_bitmap;
		/* V1p05 Rail Voltage in mv  */
		int v1p05_voltage_mv;
		/* Vnn Rail Voltage in mv  */
		int vnn_voltage_mv;
		/* VnnSx Rail Voltage in mv  */
		int vnn_sx_voltage_mv;
		/* External Icc Max for V1p05 rail in mA  */
		int v1p05_icc_max_ma;
		/* External Icc Max for VnnSx rail in mA  */
		int vnn_icc_max_ma;
	} ext_fivr_settings;

	/* VrConfig Settings.
	* 0 = VR_DOMAIN_IA Core 1 = VR_DOMAIN_GT.
	*/
	struct vr_config domain_vr_config[NUM_VR_DOMAINS];

	uint16_t max_dram_speed_mts;

	enum {
		SLP_S3_ASSERTION_DEFAULT,
		SLP_S3_ASSERTION_60_US,
		SLP_S3_ASSERTION_1_MS,
		SLP_S3_ASSERTION_50_MS,
		SLP_S3_ASSERTION_2_S,
	} pch_slp_s3_min_assertion_width;

	enum {
		SLP_S4_ASSERTION_DEFAULT,
		SLP_S4_ASSERTION_1S,
		SLP_S4_ASSERTION_2S,
		SLP_S4_ASSERTION_3S,
		SLP_S4_ASSERTION_4S,
	} pch_slp_s4_min_assertion_width;

	enum {
		SLP_SUS_ASSERTION_DEFAULT,
		SLP_SUS_ASSERTION_0_MS,
		SLP_SUS_ASSERTION_500_MS,
		SLP_SUS_ASSERTION_1_S,
		SLP_SUS_ASSERTION_4_S,
	} pch_slp_sus_min_assertion_width;

	enum {
		SLP_A_ASSERTION_DEFAULT,
		SLP_A_ASSERTION_0_MS,
		SLP_A_ASSERTION_4_S,
		SLP_A_ASSERTION_98_MS,
		SLP_A_ASSERTION_2_S,
	} pch_slp_a_min_assertion_width;

	/*
	 * PCH PM Reset Power Cycle Duration
	 * NOTE: Duration programmed in the PchPmPwrCycDur should never be smaller than the
	 * stretch duration programmed in the following registers:
	 *  - GEN_PMCON_A.SLP_S3_MIN_ASST_WDTH (PchPmSlpS3MinAssert)
	 *  - GEN_PMCON_A.S4MAW (PchPmSlpS4MinAssert)
	 *  - PM_CFG.SLP_A_MIN_ASST_WDTH (PchPmSlpAMinAssert)
	 *  - PM_CFG.SLP_LAN_MIN_ASST_WDTH
	 */
	enum {
		POWER_CYCLE_DURATION_DEFAULT,
		POWER_CYCLE_DURATION_1S,
		POWER_CYCLE_DURATION_2S,
		POWER_CYCLE_DURATION_3S,
		POWER_CYCLE_DURATION_4S,
	} pch_reset_power_cycle_duration;

	/* Platform Power Pmax */
	uint16_t platform_pmax;
	/*
	 * FivrRfiFrequency
	 * PCODE MMIO Mailbox: Set the desired RFI frequency, in increments of 100KHz.
	 * 0: Auto
	 * Range varies based on XTAL clock:
	 *   0-1918*100 KHz (Up to 191.8MHz) for 24MHz clock
	 *   0-1535*100 KHz (Up to 153.5MHz) for 19MHz clock
	 */
	uint32_t fivr_rfi_frequency;
	/*
	 * FivrSpreadSpectrum
	 * Set the Spread Spectrum Range.
	 *   Range: 0.5%, 1%, 1.5%, 2%, 3%, 4%, 5%, 6%.
	 *   Each Range is translated to an encoded value for FIVR register.
	 *   0.5% = 0, 1% = 3, 1.5% = 8, 2% = 18, 3% = 28, 4% = 34, 5% = 39, 6% = 44.
	 */
	uint8_t fivr_spread_spectrum;
	/* Enable or Disable Acoustic Noise Mitigation feature */
	uint8_t acoustic_noise_mitigation;
	/*
	 * Acoustic Noise Mitigation Range. Defines the maximum Pre-Wake
	 * randomization time in micro ticks. This can be programmed only
	 * if AcousticNoiseMitigation is enabled.
	 * Range 0-255
	 */
	uint8_t PreWake;
	/* Disable Fast Slew Rate for Deep Package C States for VR domains */
	uint8_t fast_pkg_c_ramp_disable[NUM_VR_DOMAINS];
	/*
	 * Slew Rate configuration for Deep Package C States for VR domains
	 * 0: Fast/2, 1: Fast/4, 2: Fast/8, 3: Fast/16; see enum slew_rate for values
	 */
	uint8_t slow_slew_rate[NUM_VR_DOMAINS];

	/* Energy-Performance Preference (HWP feature) */
	bool enable_energy_perf_pref;
	uint8_t energy_perf_pref_value;

	/*
	 * Enable or Disable C1 Cstate Demotion.
	 * Default 0. Set this to 1 in order to disable C state demotion.
	 */
	bool disable_c1_state_auto_demotion;

	/*
	 * Enable or Disable PCH USB2 Phy power gating.
	 * Default 0. Set this to 1 in order to disable PCH USB2 Phy Power gating.
	 * Workaround for Intel TA# 723158 to prevent possible display flicker.
	 */
	bool usb2_phy_sus_pg_disable;

	/*
	 * Enable or Disable Package C-state Demotion.
	 * Default is set to 0.
	 * Set this to 1 in order to disable Package C-state demotion.
	 */
	bool disable_package_c_state_demotion;
};

typedef struct soc_intel_alderlake_config config_t;

#endif
