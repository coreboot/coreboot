/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <baseboard/variants.h>

/*
 * Pad configuration in ramstage. The order largely follows the 'GPIO Muxing'
 * table found in EDS vol 1, but some pins aren't grouped functionally in
 * the table so those were moved for more logical grouping.
 */
static const struct pad_config gpio_table[] = {
	/* Southwest Community */

	/* PCIE_WAKE[0:3]_N - unused */
	PAD_CFG_GPI(GPIO_205, UP_20K, DEEP),
	PAD_CFG_GPI(GPIO_206, UP_20K, DEEP),
	PAD_CFG_GPI(GPIO_207, UP_20K, DEEP),
	PAD_CFG_GPI(GPIO_208, UP_20K, DEEP),

	/* EMMC interface. */
	PAD_CFG_NF(GPIO_156, DN_20K, DEEP, NF1),	/* EMMC_CLK */
	PAD_CFG_NF(GPIO_157, UP_20K, DEEP, NF1),	/* EMMC_D0 */
	PAD_CFG_NF(GPIO_158, UP_20K, DEEP, NF1),	/* EMMC_D1 */
	PAD_CFG_NF(GPIO_159, UP_20K, DEEP, NF1),	/* EMMC_D2 */
	PAD_CFG_NF(GPIO_160, UP_20K, DEEP, NF1),	/* EMMC_D3 */
	PAD_CFG_NF(GPIO_161, UP_20K, DEEP, NF1),	/* EMMC_D4 */
	PAD_CFG_NF(GPIO_162, UP_20K, DEEP, NF1),	/* EMMC_D5 */
	PAD_CFG_NF(GPIO_163, UP_20K, DEEP, NF1),	/* EMMC_D6 */
	PAD_CFG_NF(GPIO_164, UP_20K, DEEP, NF1),	/* EMMC_D7 */
	PAD_CFG_NF(GPIO_165, UP_20K, DEEP, NF1),	/* EMMC_CMD */
	PAD_CFG_NF(GPIO_182, DN_20K, DEEP, NF1),	/* EMMC_RCLK */

	/* SDIO - unused */
	PAD_CFG_GPI(GPIO_166, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_167, DN_20K, DEEP),
	PAD_CFG_NF(GPIO_168, UP_20K, DEEP, NF1),
	PAD_CFG_GPI(GPIO_169, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_170, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_171, DN_20K, DEEP),

	/* SDCARD */
	PAD_CFG_NF(GPIO_172, DN_20K, DEEP, NF1),	/* SDCARD_CLK */
	PAD_CFG_NF(GPIO_173, UP_20K, DEEP, NF1),	/* SDCARD_D0 */
	PAD_CFG_NF(GPIO_174, UP_20K, DEEP, NF1),	/* SDCARD_D1 */
	PAD_CFG_NF(GPIO_175, UP_20K, DEEP, NF1),	/* SDCARD_D2 */
	PAD_CFG_NF(GPIO_176, UP_20K, DEEP, NF1),	/* SDCARD_D3 */
	PAD_CFG_NF(GPIO_177, UP_20K, DEEP, NF1),	/* SDCARD_CD_N */
	PAD_CFG_NF(GPIO_178, UP_20K, DEEP, NF1),	/* SDCARD_CMD */
	/*
	 * SDCARD_CLK_FB - APL EDS Vol1 remarks:
	 * This is not a physical GPIO that can be used. This Signal is not Ball
	 * out on the SoC, only the buffer exists.
	*/
	PAD_CFG_NF(GPIO_179, DN_20K, DEEP, NF1),
	PAD_CFG_NF(GPIO_186, UP_20K, DEEP, NF1),	/* SDCARD_WP_1V8 */
	PAD_CFG_TERM_GPO(GPIO_183, 1, UP_20K, DEEP),	/* SD_PWR_EN_1V8 */

	/* West Community */

	PAD_CFG_GPI(GPIO_124, DN_20K, DEEP),		/* unused */
	PAD_CFG_GPI(GPIO_125, DN_20K, DEEP),		/* unused */
	PAD_CFG_GPI(GPIO_126, DN_20K, DEEP),		/* unused */
	PAD_CFG_GPI(GPIO_127, DN_20K, DEEP),		/* unused */

	PAD_CFG_NF(GPIO_128, NONE, DEEP, NF1),	/* provided LPSS_I2C2_SDA */
	PAD_CFG_NF(GPIO_129, NONE, DEEP, NF1),	/* provided LPSS_I2C2_SCL */

	PAD_CFG_GPI(GPIO_130, DN_20K, DEEP),		/* unused */
	PAD_CFG_GPI(GPIO_131, DN_20K, DEEP),		/* unused */

	PAD_CFG_NF(GPIO_132, NONE, DEEP, NF1),	/* provided LPSS_I2C4_SDA */
	PAD_CFG_NF(GPIO_133, NONE, DEEP, NF1),	/* provided LPSS_I2C4_SCL */

	/*
	 * Hint for USB enable power: some GPIOs are open drain outputs,
	 * to drive high -> Bit GPIO_TX_DIS has to be set in combination with PU
	 * PAD_CFG_GPO macro does not work. Refer to APL EDS Vol 4.
	 */
	PAD_CFG_GPI(GPIO_134, UP_20K, DEEP),		/* enable USB0 power */
	PAD_CFG_GPI(GPIO_135, UP_20K, DEEP),		/* unused */
	PAD_CFG_GPI(GPIO_136, UP_20K, DEEP),		/* enable USB7 power */
	PAD_CFG_GPI(GPIO_137, UP_20K, DEEP),		/* enable USB6 power */
	PAD_CFG_GPI(GPIO_138, UP_20K, DEEP),		/* enable USB2 power */
	PAD_CFG_GPI(GPIO_139, UP_20K, DEEP),		/* enable USB1 power */

	/* ISH_GPIO_[0:9] -- unused */
	PAD_CFG_GPI(GPIO_146, DN_20K, DEEP),		/* ISH_GPIO_0 */
	PAD_CFG_GPI(GPIO_147, DN_20K, DEEP),		/* ISH_GPIO_1 */
	PAD_CFG_GPI(GPIO_148, DN_20K, DEEP),		/* ISH_GPIO_2 */
	PAD_CFG_GPI(GPIO_149, DN_20K, DEEP),		/* ISH_GPIO_3 */
	PAD_CFG_GPI(GPIO_150, DN_20K, DEEP),		/* ISH_GPIO_4 */
	PAD_CFG_GPI(GPIO_151, DN_20K, DEEP),		/* ISH_GPIO_5 */
	PAD_CFG_GPI(GPIO_152, DN_20K, DEEP),		/* ISH_GPIO_6 */
	PAD_CFG_GPI(GPIO_153, DN_20K, DEEP),		/* ISH_GPIO_7 */
	PAD_CFG_GPI(GPIO_154, DN_20K, DEEP),		/* ISH_GPIO_8 */
	PAD_CFG_GPI(GPIO_155, DN_20K, DEEP),		/* ISH_GPIO_9 */

	/* PCIE_CLKREQ[0:3]_N */
	PAD_CFG_NF(GPIO_209, DN_20K, DEEP, NF1),
	PAD_CFG_NF(GPIO_210, DN_20K, DEEP, NF1),
	PAD_CFG_NF(GPIO_211, DN_20K, DEEP, NF1),
	PAD_CFG_NF(GPIO_212, DN_20K, DEEP, NF1),

	/* OSC_CLK_OUT_[0:4] - unused */
	PAD_CFG_GPI(OSC_CLK_OUT_0, DN_20K, DEEP),
	PAD_CFG_GPI(OSC_CLK_OUT_1, DN_20K, DEEP),
	PAD_CFG_GPI(OSC_CLK_OUT_2, DN_20K, DEEP),
	PAD_CFG_GPI(OSC_CLK_OUT_3, DN_20K, DEEP),
	PAD_CFG_GPI(OSC_CLK_OUT_4, DN_20K, DEEP),

	/* PMU Signals */
	PAD_CFG_NF(PMU_AC_PRESENT, UP_20K, DEEP, NF1),
	PAD_CFG_NF(PMU_BATLOW_B, UP_20K, DEEP, NF1),
	PAD_CFG_NF(PMU_PLTRST_B, NONE, DEEP, NF1),
	PAD_CFG_NF(PMU_PWRBTN_B, UP_20K, DEEP, NF1),
	PAD_CFG_NF(PMU_RESETBUTTON_B, NONE, DEEP, NF1),
	PAD_CFG_GPI(PMU_SLP_S0_B, UP_20K, DEEP),
	PAD_CFG_GPI(PMU_SLP_S3_B, UP_20K, DEEP),
	PAD_CFG_GPI(PMU_SLP_S4_B, UP_20K, DEEP),
	PAD_CFG_GPI(PMU_WAKE_B, DN_20K, DEEP),
	PAD_CFG_GPI(SUS_STAT_B, DN_20K, DEEP),
	PAD_CFG_GPI(SUSPWRDNACK, DN_20K, DEEP),

	/* Northwest Community */

	/* DDI0 SDA and SCL - Display-Port X24 */
	PAD_CFG_NF(GPIO_187, UP_20K, DEEP, NF1),	/* HV_DDI0_DDC_SDA */
	PAD_CFG_NF(GPIO_188, UP_20K, DEEP, NF1),	/* HV_DDI0_DDC_SCL */
	/* DDI1 SDA and SCL - unused */
	PAD_CFG_GPI(GPIO_189, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_190, DN_20K, DEEP),
	/* MIPI I2C - unused */
	PAD_CFG_GPI(GPIO_191, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_192, DN_20K, DEEP),

	/* Panel 0 control - unused */
	PAD_CFG_GPI(GPIO_193, DN_20K, DEEP),		/* PNL0_VDDEN */
	PAD_CFG_GPI(GPIO_194, DN_20K, DEEP),		/* PNL0_BKLTEN */
	PAD_CFG_GPI(GPIO_195, DN_20K, DEEP),		/* PNL0_BKLTCTL */

	/* Panel 1 control - unused */
	PAD_CFG_GPI(GPIO_196, DN_20K, DEEP),		/* PNL1_VDDEN */
	PAD_CFG_GPI(GPIO_197, DN_20K, DEEP),		/* PNL1_BKLTEN */
	PAD_CFG_GPI(GPIO_198, DN_20K, DEEP),		/* PNL1_BKLTCTL */

	/* DDI[0:1]_HPD */
	PAD_CFG_GPI(GPIO_199, UP_20K, DEEP),		/* DDI1_HPD unused */
	PAD_CFG_NF(GPIO_200, UP_20K, DEEP, NF2),	/* DDI0_HDP X24 */

	/* MDSI signals - unused */
	PAD_CFG_GPI(GPIO_201, DN_20K, DEEP),		/* MDSI_A_TE */
	PAD_CFG_GPI(GPIO_202, DN_20K, DEEP),		/* MDSI_C_TE */

	/* USB overcurrent pins. */
	PAD_CFG_NF(GPIO_203, UP_20K, DEEP, NF1),	/* USB_OC0_N */
	PAD_CFG_NF(GPIO_204, UP_20K, DEEP, NF1),	/* USB_OC1_N */

	/* PMC SPI */
	PAD_CFG_GPI(PMC_SPI_FS0, DN_20K, DEEP),		/* unused */
	PAD_CFG_NF(PMC_SPI_FS1, UP_20K, DEEP, NF2),	/* XHPD_EDP_APL */
	PAD_CFG_GPI(PMC_SPI_FS2, DN_20K, DEEP),		/* unused */
	PAD_CFG_GPI(PMC_SPI_RXD, DN_20K, DEEP),		/* unused */
	PAD_CFG_GPI(PMC_SPI_TXD, DN_20K, DEEP),		/* unused */
	PAD_CFG_GPI(PMC_SPI_CLK, DN_20K, DEEP),		/* unused */

	/* PMIC Signals unused signals related to an old PMIC interface. */
	PAD_CFG_NF(PMIC_PWRGOOD, DN_20K, DEEP, NF1),	/* PMIC_PWRGOOD */
	PAD_CFG_GPI(PMIC_RESET_B, DN_20K, DEEP),	/* PMIC_RESET_B */
	PAD_CFG_GPI(GPIO_213, UP_20K, DEEP),		/* PMIC_SDWN_B */
	PAD_CFG_GPI(GPIO_214, DN_20K, DEEP),		/* PMIC_BCUDISW2 */
	PAD_CFG_GPI(GPIO_215, DN_20K, DEEP),		/* PMIC_BCUDISCRIT */
	PAD_CFG_NF(PMIC_THERMTRIP_B, UP_20K, DEEP, NF1),/* THERMTRIP_N */
	PAD_CFG_GPI(PMIC_STDBY, DN_20K, DEEP),		/* PMIC_STDBY */
	PAD_CFG_NF(PROCHOT_B, UP_20K, DEEP, NF1),	/* PROCHOT_N */
	PAD_CFG_GPI(PMIC_I2C_SCL, DN_20K, DEEP),	/* unused */
	PAD_CFG_GPI(PMIC_I2C_SDA, DN_20K, DEEP),	/* unused */

	/* I2S1 - unused */
	PAD_CFG_GPI(GPIO_74, DN_20K, DEEP),		/* I2S1_MCLK */
	PAD_CFG_GPI(GPIO_75, DN_20K, DEEP),		/* I2S1_BCLK */
	PAD_CFG_GPI(GPIO_76, DN_20K, DEEP),		/* I2S1_WS_SYNC */
	PAD_CFG_GPI(GPIO_77, DN_20K, DEEP),		/* I2S1_SDI */
	PAD_CFG_GPI(GPIO_78, DN_20K, DEEP),		/* I2S1_SDO */

	/* DMIC or I2S4 - unused */
	PAD_CFG_GPI(GPIO_79, DN_20K, DEEP),		/* AVS_M_CLK_A1 */
	PAD_CFG_GPI(GPIO_80, DN_20K, DEEP),		/* AVS_M_CLK_B1 */
	PAD_CFG_GPI(GPIO_81, DN_20K, DEEP),		/* AVS_M_DATA_1 */
	PAD_CFG_GPI(GPIO_82, DN_20K, DEEP),		/* AVS_M_CLK_AB2 */
	PAD_CFG_GPI(GPIO_83, DN_20K, DEEP),		/* AVS_M_DATA_2 */

	/* I2S2 - unused */
	PAD_CFG_GPO(GPIO_84, 0, DEEP),			/* AVS_I2S2_MCLK */
	PAD_CFG_GPI(GPIO_85, DN_20K, DEEP),		/* AVS_I2S2_BCLK */
	PAD_CFG_GPI(GPIO_86, DN_20K, DEEP),		/* AVS_I2S2_WS_SYNC */
	PAD_CFG_GPI(GPIO_87, DN_20K, DEEP),		/* AVS_I2S2_SDI */
	PAD_CFG_GPI(GPIO_88, DN_20K, DEEP),		/* AVS_I2S2_SDO */

	/* I2S3 - unused */
	PAD_CFG_GPI(GPIO_89, DN_20K, DEEP),		/* AVS_I2S3_BCLK */
	PAD_CFG_GPI(GPIO_90, DN_20K, DEEP),		/* AVS_I2S3_WS_SYNC */
	PAD_CFG_GPI(GPIO_91, DN_20K, DEEP),		/* AVS_I2S3_SDI */
	PAD_CFG_GPI(GPIO_92, DN_20K, DEEP),		/* AVS_I2S3_SDO */

	/* Fast SPI for mainboard Flash and TPM on mainboard. */
	PAD_CFG_NF(GPIO_97, NATIVE, DEEP, NF1),	/* FST_SPI_CS0_B */
	PAD_CFG_GPI(GPIO_98, DN_20K, DEEP),	/* FST_SPI_CS1_B - unused */
	PAD_CFG_NF(GPIO_99, NATIVE, DEEP, NF1),	/* FST_SPI_MOSI_IO0 */
	PAD_CFG_NF(GPIO_100, NATIVE, DEEP, NF1),/* FST_SPI_MISO_IO1 */
	PAD_CFG_NF(GPIO_101, NATIVE, DEEP, NF1),/* FST_IO2 - MEM_CONFIG0 */
	PAD_CFG_NF(GPIO_102, NATIVE, DEEP, NF1),/* FST_IO3 - MEM_CONFIG1 */
	PAD_CFG_NF(GPIO_103, NATIVE, DEEP, NF1),/* FST_SPI_CLK */
	/* FST_SPI_CLK_FB - Pad not bonded, default register value is the same
	 * as here. Refer to Intel Doc APL EDS Vol 1 */
	PAD_CFG_NF(FST_SPI_CLK_FB, NONE, DEEP, NF1),

	/* SIO_SPI_0 for F-module on mainboard */
	PAD_CFG_NF(GPIO_104, DN_20K, DEEP, NF1),
	PAD_CFG_NF(GPIO_105, UP_20K, DEEP, NF1),
	/* GPIO_106 configured as XCS for TPM */
	PAD_CFG_NF(GPIO_106, UP_20K, DEEP, NF3),
	PAD_CFG_NF(GPIO_109, UP_20K, DEEP, NF1),
	PAD_CFG_NF(GPIO_110, UP_20K, DEEP, NF1),

	/* SIO_SPI_1 -- unused */
	PAD_CFG_GPI(GPIO_111, DN_20K, DEEP),		/* GP_SSP_1_CLK */
	PAD_CFG_GPI(GPIO_112, DN_20K, DEEP),		/* GP_SSP_1_FS0 */
	PAD_CFG_GPI(GPIO_113, DN_20K, DEEP),		/* GP_SSP_1_FS1 */
	PAD_CFG_GPI(GPIO_116, DN_20K, DEEP),		/* GP_SSP_1_RXD */
	PAD_CFG_GPI(GPIO_117, DN_20K, DEEP),		/* GP_SSP_1_TXD */

	/* SIO_SPI_2 -- unused */
	PAD_CFG_GPI(GPIO_118, UP_20K, DEEP),		/* GP_SSP_2_CLK */
	PAD_CFG_GPI(GPIO_119, DN_20K, DEEP),		/* GP_SSP_2_FS0 */
	PAD_CFG_GPI(GPIO_120, DN_20K, DEEP),		/* GP_SSP_2_FS1 */
	PAD_CFG_GPI(GPIO_121, DN_20K, DEEP),		/* GP_SSP_2_FS2 */
	PAD_CFG_GPI(GPIO_122, DN_20K, DEEP),		/* GP_SSP_2_RXD */
	PAD_CFG_GPI(GPIO_123, DN_20K, DEEP),		/* GP_SSP_2_TXD */

	/* North Community */

	PAD_CFG_GPI(GPIO_0, DN_20K, DEEP),
	/* GPIO_1 in early_gpio_table */
	PAD_CFG_GPI(GPIO_2, DN_20K, DEEP),
	/* GPIO_3,4 in early_gpio_table */
	PAD_CFG_GPI(GPIO_5, DN_20K, DEEP),		/* TRACE_0_DATA4_VNN */
	PAD_CFG_GPI(GPIO_6, DN_20K, DEEP),		/* TRACE_0_DATA5_VNN */
	PAD_CFG_GPO(GPIO_7, 0, DEEP),
	PAD_CFG_GPI(GPIO_8, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_9, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_10, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_11, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_12, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_13, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_14, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_15, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_16, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_17, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_18, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_19, DN_20K, DEEP),
	PAD_CFG_GPI(GPIO_20, DN_20K, DEEP),
	PAD_CFG_GPO(GPIO_21, 0, DEEP),	/* activate SMARC Ethernetmac 2 */
	PAD_CFG_GPO(GPIO_22, 0, DEEP),	/* activate SMARC Ethernetmac 1 */
	PAD_CFG_GPI(GPIO_23, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_24, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_25, DN_20K, DEEP),		/* pin open */
	PAD_CFG_NF(GPIO_26, UP_20K, DEEP, NF5),		/* SATA_LEDN */
	PAD_CFG_GPI(GPIO_27, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPO(GPIO_28, 1, DEEP),			/* disable SMARC HDMI */
	PAD_CFG_GPO(GPIO_29, 0, DEEP),
	PAD_CFG_GPI(GPIO_30, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_31, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_32, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_33, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_34, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_35, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_36, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_37, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_38, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_39, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_40, DN_20K, DEEP),		/* pin open */

	/* LPSS_UART[0:2] */
	PAD_CFG_GPI(GPIO_41, DN_20K, DEEP),		/* pin open */
	/* GPIO_42/43 are in early_gpio_table */
	PAD_CFG_GPI(GPIO_44, DN_20K, DEEP),	/* LPSS_UART1_RTS - unused */
	PAD_CFG_GPI(GPIO_45, DN_20K, DEEP),	/* LPSS_UART1_CTS - unused */
	/* GPIO_46/47 are in early_gpio_table */
	PAD_CFG_GPI(GPIO_48, DN_20K, DEEP),	/* LPSS_UART2_RTS - unused */
	PAD_CFG_GPI(GPIO_49, DN_20K, DEEP),	/* LPSS_UART2_CTS - unused */

	/* Camera interface -- completely unused. */
	PAD_CFG_GPI(GPIO_62, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_63, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_64, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_65, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_66, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPI(GPIO_67, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPO(GPIO_68, 0, DEEP),		/* APL7 Testpoint P403 */
	PAD_CFG_GPO(GPIO_69, 0, DEEP),		/* APL7 Testpoint P404 */
	PAD_CFG_GPI(GPIO_70, DN_20K, DEEP),		/* pin open */
	PAD_CFG_GPO(GPIO_71, 1, DEEP),		/* XCHC_UPD_REQ */
	PAD_CFG_GPO(GPIO_72, 0, DEEP),		/* APL7 Testpoint P402 */
	PAD_CFG_GPI(GPIO_73, DN_20K, DEEP),		/* pin open */

	/* no TAP controller pins available on SMARC of APL7 */
	PAD_CFG_NF(TCK, DN_20K, DEEP, NF1),		/* pin open */
	PAD_CFG_NF(TRST_B, DN_20K, DEEP, NF1),		/* pin open */
	PAD_CFG_NF(TMS, DN_20K, DEEP, NF1),		/* pin open */
	PAD_CFG_NF(TDI, DN_20K, DEEP, NF1),		/* pin open */

	PAD_CFG_NF(CX_PMODE, DN_20K, DEEP, NF1),	/* pin open */
	PAD_CFG_NF(CX_PREQ_B, DN_20K, DEEP, NF1),	/* pin open */
	PAD_CFG_NF(JTAGX, DN_20K, DEEP, NF1),		/* pin open */
	PAD_CFG_NF(CX_PRDY_B, DN_20K, DEEP, NF1),	/* pin open */
	PAD_CFG_NF(TDO, DN_20K, DEEP, NF1),		/* pin open */

	/* GPIO_[216:219] described into EDS Vol1. */
	PAD_CFG_GPO(CNV_BRI_DT, 0, DEEP),	/* Disable eDP to LVDS bridge */
	PAD_CFG_GPI(CNV_BRI_RSP, UP_20K, DEEP),
	PAD_CFG_GPI(CNV_RGI_DT, DN_20K, DEEP),		/* pin open */

	/* Writing to following GPIO registers leads to 0xFFFF FFFF in CFG0/1 */
	PAD_CFG_NF(CNV_RGI_RSP, DN_20K, DEEP, NF1),	/* pin open */

	/* Serial Voltage Identification */
	PAD_CFG_NF(SVID0_ALERT_B, NONE, DEEP, NF1),	/* SVID0_ALERT_B */
	PAD_CFG_NF(SVID0_DATA, UP_20K, DEEP, NF1),	/* SVID0_DATA */
	PAD_CFG_NF(SVID0_CLK, UP_20K, DEEP, NF1),	/* SVID0_CLK */
};

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

/* GPIOs needed prior to ramstage. */
static const struct pad_config early_gpio_table[] = {
	/* UART */
	PAD_CFG_NF(GPIO_46, NATIVE, DEEP, NF1),				/* LPSS_UART2_RXD */
	PAD_CFG_NF_IOSSTATE(GPIO_47, NATIVE, DEEP, NF1, Tx1RxDCRx0),	/* LPSS_UART2_TXD */

	/* Southwest Community */

	/* Multiplexed I2C7 */
	PAD_CFG_NF(SMB_ALERTB, UP_20K, DEEP, NF2),
	PAD_CFG_NF(SMB_CLK, UP_20K, DEEP, NF2),
	PAD_CFG_NF(SMB_DATA, UP_20K, DEEP, NF2),

	/* get LPC Bus early working */
	PAD_CFG_NF(LPC_ILB_SERIRQ, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_CLKOUT0, NONE, DEEP, NF1),
	PAD_CFG_NF(LPC_CLKOUT1, NONE, DEEP, NF1),
	PAD_CFG_NF(LPC_AD0, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_AD1, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_AD2, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_AD3, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_CLKRUNB, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_FRAMEB, UP_20K, DEEP, NF1),

	/* North Community */
	PAD_CFG_GPO(GPIO_1, 1, DEEP),			/* XPCIE_A_RST */
	PAD_CFG_GPO(GPIO_3, 1, DEEP),			/* XPCIE_B_RST */
	PAD_CFG_GPO(GPIO_4, 1, DEEP),			/* XPCIE_C_RST */

	/* UARTs for early coreboot output */
	PAD_CFG_NF(GPIO_42, UP_20K, DEEP, NF1),		/* LPSS_UART1_RXD */
	PAD_CFG_NF(GPIO_43, UP_20K, DEEP, NF1),		/* LPSS_UART1_TXD */
	PAD_CFG_NF(GPIO_46, UP_20K, DEEP, NF1),		/* LPSS_UART2_RXD */
	PAD_CFG_NF(GPIO_47, UP_20K, DEEP, NF1),		/* LPSS_UART2_TXD */

	PAD_CFG_NF(PMU_SUSCLK, NONE, DEEP, NF1),/* 32,78 kHz used on SMARC */
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}
