/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

static const struct pad_config gpio_table[] = {
	/* A0  : GPP_A0 ==> NC */
	PAD_NC(GPP_A0, NONE),
	/* A1  : ESPI_IO0 */
	/* A2  : ESPI_IO1 */
	/* A3  : ESPI_IO2 */
	/* A4  : ESPI_IO3 */
	/* A5  : ESPI_CS# */
	/* A6  : GPP_A6 ==> NC */
	PAD_NC(GPP_A6, NONE),
	/* A7  : PP3300_SOC_A */
	PAD_NC(GPP_A7, NONE),
	/* A8  : GPP_A8 ==> NC */
	PAD_NC(GPP_A8, NONE),
	/* A9  : ESPI_CLK */
	/* A10  : GPP_A10 ==> NC */
	PAD_NC(GPP_A10, NONE),
	/* A11  : GPP_A11 ==> NC */
	PAD_NC(GPP_A11, NONE),
	/* A12  : GPP_A12 ==> NC */
	PAD_NC(GPP_A12, NONE),
	/* A13 : SUSWARN_L */
	PAD_CFG_NF(GPP_A13, NONE, DEEP, NF1),
	/* A14 : ESPI_RST_L */
	/* A15 : SUSACK_L */
	PAD_CFG_NF(GPP_A15, NONE, DEEP, NF1),
	/* A16 : SD_1P8_SEL => NC */
	PAD_NC(GPP_A16, NONE),
	/* A17 : EN_PP3300_SD_DX */
	PAD_CFG_NF(GPP_A17, NONE, DEEP, NF1),
	/* A18 : EN_PP3300_WWAN */
	PAD_CFG_GPO(GPP_A18, 1, DEEP),
	/* A19 : WWAN_RADIO_DISABLE_1V8_ODL */
	PAD_CFG_GPO(GPP_A19, 1, DEEP),
	/* A20 : WLAN_INT_L */
	PAD_CFG_GPI_APIC(GPP_A20, NONE, PLTRST, LEVEL, INVERT),
	/* A21 : TRACKPAD_INT_ODL */
	PAD_CFG_GPI_IRQ_WAKE(GPP_A21, NONE, DEEP, LEVEL, INVERT),
	/* A22 : FPMCU_PCH_BOOT0 */
	PAD_CFG_GPO(GPP_A22, 0, DEEP),
	/* A23 : FPMCU_PCH_INT_ODL */
	PAD_CFG_GPI_IRQ_WAKE(GPP_A23, NONE, PLTRST, LEVEL, INVERT),

	/* B0  : CORE_VID0 */
	PAD_CFG_NF(GPP_B0, NONE, DEEP, NF1),
	/* B1  : CORE_VID1 */
	PAD_CFG_NF(GPP_B1, NONE, DEEP, NF1),
	/* B2  : GPP_B2 ==> NC */
	PAD_NC(GPP_B2, NONE),
	/* B3  : GPP_B3 ==> NC */
	PAD_NC(GPP_B3, NONE),
	/* B4  : GPP_B4 ==> NC */
	PAD_NC(GPP_B4, NONE),
	/* B5  : GPP_B5 ==> NC */
	PAD_NC(GPP_B5, NONE),
	/* B6  : SRCCLKREQ1 */
	PAD_CFG_NF(GPP_B6, NONE, DEEP, NF1),
	/* B7  : GPP_B7 ==> NC */
	PAD_NC(GPP_B7, NONE),
	/* B8  : PCIE_14_WLAN_CLKREQ_ODL */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF1),
	/* B9  : GPP_B9 ==> NC */
	PAD_NC(GPP_B9, NONE),
	/* B10 : GPP_B10 ==> NC */
	PAD_NC(GPP_B10, NONE),
	/* B11 : EXT_PWR_GATE_L */
	PAD_CFG_NF(GPP_B11, NONE, DEEP, NF1),
	/* B12 : SLP_S0_L */
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
	/* B13 : PLT_RST_L */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	/* B14 : GPP_B14_STRAP */
	PAD_NC(GPP_B14, NONE),
	/* B15 : H1_SLAVE_SPI_CS_L */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),
	/* B16 : H1_SLAVE_SPI_CLK */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF1),
	/* B17 : H1_SLAVE_SPI_MISO_R */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF1),
	/* B18 : H1_SLAVE_SPI_MOSI_R */
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF1),
	/* B19 : Set to NF1 to match FSP setting it to NF1, i.e., GSPI1_CS0# */
	PAD_CFG_NF(GPP_B19, NONE, DEEP, NF1),
	/* B20 : PCH_SPI_FPMCU_CLK_R */
	PAD_CFG_NF(GPP_B20, NONE, DEEP, NF1),
	/* B21 : PCH_SPI_FPMCU_MISO */
	PAD_CFG_NF(GPP_B21, NONE, DEEP, NF1),
	/* B22 : PCH_SPI_FPMCU_MOSI */
	PAD_CFG_NF(GPP_B22, NONE, DEEP, NF1),
	/* B23 : GPP_B23_STRAP */
	PAD_NC(GPP_B23, NONE),

	/* C0  : GPP_C0 => NC */
	PAD_NC(GPP_C0, NONE),
	/* C1  : PCIE_14_WLAN_WAKE_ODL */
	PAD_CFG_GPI_SCI_LOW(GPP_C1, NONE, DEEP, EDGE_SINGLE),
	/* C2  : GPP_C2 => NC */
	PAD_NC(GPP_C2, NONE),
	/* C3  : WLAN_OFF_L */
	PAD_CFG_GPO(GPP_C3, 1, DEEP),
	/* C4  : TOUCHSCREEN_DIS_L */
	PAD_CFG_GPO(GPP_C4, 1, DEEP),
	/* C5  : GPP_C5 => NC */
	PAD_NC(GPP_C5, NONE),
	/* C6  : PEN_PDCT_OD_L */
	PAD_NC(GPP_C6, NONE),
	/* C7  : PEN_IRQ_OD_L */
	PAD_NC(GPP_C7, NONE),
	/* C8  : UART_PCH_RX_DEBUG_TX */
	PAD_CFG_NF(GPP_C8, NONE, DEEP, NF1),
	/* C9  : UART_PCH_TX_DEBUG_RX */
	PAD_CFG_NF(GPP_C9, NONE, DEEP, NF1),
	/* C10 : GPP_10 ==> GPP_C10_TP */
	PAD_NC(GPP_C10, NONE),
	/* C11 : GPP_11 ==> EN_FP_RAILS */
	PAD_CFG_GPO(GPP_C11, 0, DEEP),
	/* C12 : GPP_C12 ==> NC */
	PAD_NC(GPP_C12, NONE),
	/* C13 : EC_PCH_INT_L */
	PAD_CFG_GPI_APIC(GPP_C13, NONE, PLTRST, LEVEL, INVERT),
	/* C14 : BT_DISABLE_L */
	PAD_CFG_GPO(GPP_C14, 1, DEEP),
	/* C15 : NC */
	PAD_NC(GPP_C15, NONE),
	/* C16 : PCH_I2C_TRACKPAD_SDA */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	/* C17 : PCH_I2C_TRACKPAD_SCL */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	/* C18 : PCH_I2C_TOUCHSCREEN_SDA */
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),
	/* C19 : PCH_I2C_TOUCHSCREEN_SCL */
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),
	/* C20 : PCH_WP_OD */
	PAD_CFG_GPI(GPP_C20, NONE, DEEP),
	/* C21 : H1_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_C21, NONE, PLTRST, LEVEL, INVERT),
	/* C22 : EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_C22, NONE, DEEP),
	/* C23 : WLAN_PE_RST# */
	PAD_CFG_GPO(GPP_C23, 1, DEEP),

	/* D0  : TP31 */
	PAD_NC(GPP_D0, NONE),
	/* D1  : TP16 */
	PAD_NC(GPP_D1, NONE),
	/* D2  : TP26 */
	PAD_NC(GPP_D2, NONE),
	/* D3  : TP27 */
	PAD_NC(GPP_D3, NONE),
	/* D4  : TP40 */
	PAD_NC(GPP_D4, NONE),
	/* D5  : WWAN_CONFIG_0 */
	PAD_NC(GPP_D5, NONE),
	/* D6  : WWAN_CONFIG_1 */
	PAD_NC(GPP_D6, NONE),
	/* D7  : WWAN_CONFIG_2 */
	PAD_NC(GPP_D7, NONE),
	/* D8  : WWAN_CONFIG_3 */
	PAD_NC(GPP_D8, NONE),
	/* D9  : GPP_D9 ==> EN_PP3300_DX_TOUCHSCREEN */
	PAD_CFG_GPO(GPP_D9, 0, DEEP),
	/* D10 : GPP_D10 ==> NC */
	PAD_NC(GPP_D10, NONE),
	/* D11 : GPP_D11 ==> NC */
	PAD_NC(GPP_D11, NONE),
	/* D12 : GPP_D12 */
	PAD_NC(GPP_D12, NONE),
	/* D13 : ISH_UART_RX */
	PAD_NC(GPP_D13, NONE),
	/* D14 : ISH_UART_TX */
	PAD_NC(GPP_D14, NONE),
	/* D15 : TOUCHSCREEN_RST_L */
	PAD_CFG_GPO(GPP_D15, 0, DEEP),
	/* D16 : USI_INT */
	PAD_CFG_GPI_APIC(GPP_D16, NONE, PLTRST, LEVEL, NONE),
	/* D17 : PCH_HP_SDW_CLK */
	PAD_NC(GPP_D17, NONE),
	/* D18 : PCH_HP_SDW_DAT */
	PAD_NC(GPP_D18, NONE),
	/* D19 : DMIC_CLK_0_SNDW4_CLK */
	PAD_CFG_NF(GPP_D19, NONE, DEEP, NF1),
	/* D20 : DMIC_DATA_0_SNDW4_DATA */
	PAD_CFG_NF(GPP_D20, NONE, DEEP, NF1),
	/* D21 : GPP_D21 ==> NC */
	PAD_NC(GPP_D21, NONE),
	/* D22 : GPP_D22 ==> NC */
	PAD_NC(GPP_D22, NONE),
	/* D23 : SPP_MCLK */
	PAD_CFG_NF(GPP_D23, NONE, DEEP, NF1),

	/* E0  : GPP_E0 ==> NC */
	PAD_NC(GPP_E0, NONE),
	/* E1  : M2_SSD_PEDET */
	PAD_CFG_NF(GPP_E1, NONE, DEEP, NF1),
	/* E2  : GPP_E2 ==> NC */
	PAD_NC(GPP_E2, NONE),
	/* E3  : GPP_E3 ==> NC */
	PAD_NC(GPP_E3, NONE),
	/* E4  : M2_SSD_PE_WAKE_ODL */
	PAD_CFG_GPI(GPP_E4, NONE, DEEP),
	/* E5  : SATA_DEVSLP1 */
	PAD_CFG_NF(GPP_E5, NONE, PLTRST, NF1),
	/* E6  : M2_SSD_RST_L */
	PAD_NC(GPP_E6, NONE),
	/* E7  : GPP_E7 ==> NC */
	PAD_NC(GPP_E7, NONE),
	/* E8  : GPP_E8 ==> NC */
	PAD_NC(GPP_E8, NONE),
	/* E9  : GPP_E9 ==> NC */
	PAD_NC(GPP_E9, NONE),
	/* E10 : GPP_E10 ==> NC */
	PAD_NC(GPP_E10, NONE),
	/* E11 : USB_C_OC_OD USB_OC2 */
	PAD_CFG_NF(GPP_E11, NONE, DEEP, NF1),
	/* E12 : USB_A_OC_OD USB_OC3 */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF1),
	/* E13 : USB_C0_DP_HPD */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),
	/* E14 : DDI2_HPD_ODL */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
	/* E15 : DDPD_HPD2 =>  NC */
	PAD_NC(GPP_E15, NONE),
	/* E16 : DDPE_HPD2 => NC */
	PAD_NC(GPP_E16, NONE),
	/* E17 : EDP_HPD */
	PAD_CFG_NF(GPP_E17, NONE, DEEP, NF1),
	/* E18 : DDPB_CTRLCLK => NC */
	PAD_NC(GPP_E18, NONE),
	/* E19 : GPP_E19_STRAP */
	PAD_CFG_GPI(GPP_E19, NONE, DEEP),
	/* E20 : DDPC_CTRLCLK => NC */
	PAD_NC(GPP_E20, NONE),
	/* E21 : GPP_E21_STRAP */
	PAD_CFG_GPI(GPP_E21, NONE, DEEP),
	/* E22 : DDPD_CTRLCLK => NC */
	PAD_NC(GPP_E22, NONE),
	/* E23 : GPP_E23_STRAP */
	PAD_NC(GPP_E23, NONE),

	/* F0  : GPIO_WWAN_WLAN_COEX3 */
	PAD_CFG_NF(GPP_F0, NONE, DEEP, NF1),
	/* F1  : WWAN_RESET_1V8_ODL */
	PAD_CFG_GPO(GPP_F1, 1, DEEP),
	/* F2  : MEM_CH_SEL */
	PAD_CFG_GPI(GPP_F2, NONE, PLTRST),
	/* F3  : GPP_F3 ==> NC */
	PAD_NC(GPP_F3, NONE),
	/* F4  : CNV_BRI_DT */
	PAD_CFG_NF(GPP_F4, NONE, DEEP, NF1),
	/* F5  : CNV_BRI_RSP */
	PAD_CFG_NF(GPP_F5, NONE, DEEP, NF1),
	/* F6  : CNV_RGI_DT */
	PAD_CFG_NF(GPP_F6, NONE, DEEP, NF1),
	/* F7  : CNV_RGI_RSP */
	PAD_CFG_NF(GPP_F7, NONE, DEEP, NF1),
	/* F8  : UART_WWANTX_WLANRX_COEX1 */
	PAD_CFG_NF(GPP_F8, NONE, DEEP, NF1),
	/* F9  : UART_WWANRX_WLANTX_COEX2 */
	PAD_CFG_NF(GPP_F9, NONE, DEEP, NF1),
	/* F10 : GPP_F10 ==> NC */
	PAD_NC(GPP_F10, NONE),
	/* F11 : PCH_MEM_STRAP2 */
	PAD_CFG_GPI(GPP_F11, NONE, PLTRST),
	/* F12 : GPP_F12 ==> NC */
	PAD_NC(GPP_F12, NONE),
	/* F13 : GPP_F13 ==> NC */
	PAD_NC(GPP_F13, NONE),
	/* F14 : GPP_F14 ==> NC */
	PAD_NC(GPP_F14, NONE),
	/* F15 : GPP_F15 ==> NC */
	PAD_NC(GPP_F15, NONE),
	/* F16 : GPP_F16 ==> NC */
	PAD_NC(GPP_F16, NONE),
	/* F17 : GPP_F17 ==> NC */
	PAD_NC(GPP_F17, NONE),
	/* F18 : GPP_F18 ==> NC */
	PAD_NC(GPP_F18, NONE),
	/* F19 : GPP_F19 ==> NC */
	PAD_NC(GPP_F19, NONE),
	/* F20 : PCH_MEM_STRAP0 */
	PAD_CFG_GPI(GPP_F20, NONE, PLTRST),
	/* F21 : PCH_MEM_STRAP1 */
	PAD_CFG_GPI(GPP_F21, NONE, PLTRST),
	/* F22 : PCH_MEM_STRAP3 */
	PAD_CFG_GPI(GPP_F22, NONE, PLTRST),
	/* F23 : GPP_F23 ==> NC */
	PAD_NC(GPP_F23, NONE),

	/* G0  : SD_CMD */
	PAD_CFG_NF(GPP_G0, NATIVE, DEEP, NF1),
	/* G1  : SD_DATA0 */
	PAD_CFG_NF(GPP_G1, NATIVE, DEEP, NF1),
	/* G2  : SD_DATA1 */
	PAD_CFG_NF(GPP_G2, NATIVE, DEEP, NF1),
	/* G3  : SD_DATA2 */
	PAD_CFG_NF(GPP_G3, NATIVE, DEEP, NF1),
	/* G4  : SD_DATA3 */
	PAD_CFG_NF(GPP_G4, NATIVE, DEEP, NF1),
	/* G5  : SD_CD# */
	PAD_CFG_NF(GPP_G5, NONE, PLTRST, NF1),
	/* G6  : SD_CLK */
	PAD_CFG_NF(GPP_G6, NONE, DEEP, NF1),
	/* G7  : SD_WP
	 * As per schematics SD host controller SD_WP pin is not connected to
	 * uSD card connector. In order to overcome gpio default state, ensures
	 * to configure gpio pin as NF1 with internal 20K pull down.
	 */
	PAD_CFG_NF(GPP_G7, DN_20K, DEEP, NF1),
	/*
	 * H0  : HP_INT_L
	 */
	PAD_CFG_GPI_INT(GPP_H0, NONE, PLTRST, EDGE_BOTH),
	/* H1  : CNV_RF_RESET_L */
	PAD_CFG_NF(GPP_H1, NONE, DEEP, NF3),
	/* H2  : CNV_CLKREQ0 */
	PAD_CFG_NF(GPP_H2, NONE, DEEP, NF3),
	/* H3 : GPP_H3 ==> NC */
	PAD_NC(GPP_H3, NONE),
	/* H4  : PCH_I2C_PEN_SDA */
	PAD_NC(GPP_H4, NONE),
	/* H5  : PCH_I2C_PEN_SCL */
	PAD_NC(GPP_H5, NONE),
	/* H6  : PCH_I2C_SAR0_MST_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : PCH_I2C_SAR0_MST_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* H8  : PCH_I2C_M2_AUDIO_SAR1_SDA */
	PAD_CFG_NF(GPP_H8, NONE, DEEP, NF1),
	/* H9  : PCH_I2C_M2_AUDIO_SAR1_SCL */
	PAD_CFG_NF(GPP_H9, NONE, DEEP, NF1),
	/* H10 : PCH_I2C_TRACKPAD_SDA */
	PAD_NC(GPP_H10, NONE),
	/* H11 : PCH_I2C_TRACKPAD_SCL */
	PAD_NC(GPP_H11, NONE),
	/* H12 : GPP_H12 ==> NC */
	PAD_NC(GPP_H12, NONE),
	/* H13 : GPP_H13 ==> NC */
	PAD_NC(GPP_H13, NONE),
	/* H14 : GPP_H14 ==> NC */
	PAD_NC(GPP_H14, NONE),
	/* H15 : GPP_H15 ==> NC */
	PAD_NC(GPP_H15, NONE),
	/* H16 : GPP_H16 ==> NC */
	PAD_NC(GPP_H16, NONE),
	/* H17 : TP1 */
	PAD_NC(GPP_H17, NONE),
	/* H18 : CPU_C10_GATE_L */
	PAD_CFG_NF(GPP_H18, NONE, DEEP, NF1),
	/* H19 : GPP_H19 ==> NC */
	PAD_NC(GPP_H19, NONE),
	/* H20 : TP41 */
	PAD_NC(GPP_H20, NONE),
	/* H21 : XTAL_FREQ_SEL */
	PAD_NC(GPP_H21, NONE),
	/* H22 : GPP_H22 ==> NC */
	PAD_NC(GPP_H22, NONE),
	/* H23 : GPP_H23_STRAP */
	PAD_NC(GPP_H23, NONE),

	/* GPD2: LAN_WAKE# ==> EC_PCH_WAKE_ODL */
	PAD_CFG_NF(GPD2, NONE, DEEP, NF1),

	/* SD card detect VGPIO */
	PAD_CFG_GPI_GPIO_DRIVER(vSD3_CD_B, NONE, DEEP),

	/* CNV_WCEN  : Disable Wireless Charging */
	PAD_CFG_GPO(CNV_WCEN, 0, DEEP),
};

const struct pad_config *base_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

/*
 * Default GPIO settings before entering non-S5 sleep states.
 * Configure A12: FPMCU_RST_ODL as GPO before entering sleep.
 * This guarantees that A12's native3 function is disabled.
 * See https://review.coreboot.org/c/coreboot/+/32111 .
 */
static const struct pad_config default_sleep_gpio_table[] = {
	PAD_CFG_GPO(GPP_A12, 1, DEEP), /* FPMCU_RST_ODL */
};

/*
 * GPIO settings before entering S5, which are same as
 * default_sleep_gpio_table but also,
 * turn off EN_PP3300_WWAN and FPMCU.
 */
static const struct pad_config s5_sleep_gpio_table[] = {
	PAD_CFG_GPO(GPP_A12, 0, DEEP), /* FPMCU_RST_ODL */
	PAD_CFG_GPO(GPP_C11, 0, DEEP), /* PCH_FP_PWR_EN */
	PAD_CFG_GPO(GPP_A18, 0, DEEP), /* EN_PP3300_WWAN */
};

const struct pad_config *__weak variant_sleep_gpio_table(
	u8 slp_typ, size_t *num)
{
	if (slp_typ == ACPI_S5) {
		*num = ARRAY_SIZE(s5_sleep_gpio_table);
		return s5_sleep_gpio_table;
	}
	*num = ARRAY_SIZE(default_sleep_gpio_table);
	return default_sleep_gpio_table;
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(GPIO_PCH_WP, CROS_GPIO_DEVICE_NAME),
};

DECLARE_WEAK_CROS_GPIOS(cros_gpios);

/* Weak implementation of overrides */
const struct pad_config *__weak override_gpio_table(size_t *num)
{
	*num = 0;
	return NULL;
}

/* Weak implementation of early gpio */
const struct pad_config *__weak variant_early_gpio_table(size_t *num)
{
	*num = 0;
	return NULL;
}

/* Weak implementation of romstage gpio */
const struct pad_config *__weak variant_romstage_gpio_table(size_t *num)
{
	*num = 0;
	return NULL;
}
/* Weak implementation of finalize gpio */
const struct pad_config *__weak variant_finalize_gpio_table(size_t *num)
{
	*num = 0;
	return NULL;
}
