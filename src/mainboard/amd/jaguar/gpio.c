/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include "gpio.h"
#include "board_config.h"

/*
 * As a rule of thumb, GPIO pins used by coreboot should be initialized at
 * bootblock while GPIO pins used only by the OS should be initialized at
 * ramstage.
 */
static const struct soc_amd_gpio gpio_set_stage_ram[] = {
	/* PWR_BTN_L*/
	PAD_NF(GPIO_0, PWR_BTN_L, PULL_UP),
	/* SYS_RESET_L */
	PAD_NF(GPIO_1, SYS_RESET_L, PULL_UP),
	/* WAKE_L */
	PAD_NF_SCI(GPIO_2, WAKE_L, PULL_NONE, EDGE_LOW),
	/* INT_CLKREQ#_S5_R */
	PAD_SCI(GPIO_3, PULL_NONE, EDGE_LOW),
	/* UART_WAKE_L_M2_APU */
	PAD_SCI(GPIO_4, PULL_UP, EDGE_LOW),
	/* MPM_EVENT_L, input or OD output */
	PAD_GPI(GPIO_5, PULL_UP),
	/* EC SCI */
	PAD_SCI(GPIO_6, PULL_UP, EDGE_LOW),
	/* VDD_MEM_LP# */
	PAD_GPO(GPIO_7, HIGH),
	/* TPAD_INT_L */
	PAD_SCI(GPIO_8, PULL_UP, EDGE_LOW),
	/* CPLD_TDO_SOC_1V8 / MDIO2_SCL */
	PAD_GPO(GPIO_9, LOW),
	/* GPIO_10: VDD_MEM_VID0 - Controlled by firmware outside of coreboot (ABL) */
	/* HP_MIC_DET_L */
	PAD_GPI(GPIO_11, PULL_UP),
	/* ALIGN_FLAG_MU_L */
	PAD_GPO(GPIO_12, HIGH),
	/* GPIO_13 - GPIO_15: Not available */
	/* USB_OC0_L */
	PAD_NF(GPIO_16, USB_OC0_L, PULL_NONE),
	/* USB_OC1_L */
	PAD_NF(GPIO_17, USB_OC1_L, PULL_NONE),
	/* PCIE_WLAN_WAKE_L */
	PAD_SCI(GPIO_18, PULL_UP, EDGE_LOW),
	/* KBRST_L */
	PAD_NF(GPIO_21, KBRST_L, PULL_NONE),
	/* ESPI_ALERT_L */
	PAD_NF(GPIO_22, ESPI_ALERT_D1, PULL_NONE),
	/* AC_PRES */
	PAD_NF(GPIO_23, AC_PRES, PULL_UP),
	/* CPLD_TMS_SOC / USB_OSC3_L */
	PAD_GPO(GPIO_24, LOW),
	/* GPIO_25: Not available */
	/* PCIE_RST0_L */
	PAD_NFO(GPIO_26, PCIE_RST0_L, HIGH),
	/* PCIE_RST1_L */
	PAD_NFO(GPIO_27, PCIE_RST1_L, HIGH),
	/* GPIO_28: Not available */
	/* TPM CS */
	PAD_NF(GPIO_29, SPI_TPM_CS_L, PULL_UP),
	/* ESPI_CS_L */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_UP),
	/* SPI_CS3_L */
	PAD_NF(GPIO_31, SPI_CS3_L, PULL_UP),
	/* LPC_RST_L */
	PAD_NF(GPIO_32, LPC_RST_L, PULL_DOWN),
	/* GPIO_33 - GPIO_37: Not available */
	/* CLK_REQ5_L */
	PAD_NF(GPIO_38, CLK_REQ5_L, PULL_UP),
	/* CLK_REQ6_L */
	PAD_NF(GPIO_39, CLK_REQ6_L, PULL_UP),
	/* USB2_HDR_P0/1_SMI */
	PAD_SCI(GPIO_40, PULL_UP, EDGE_LOW),
	/* GPIO_41: Not available */
	/* GPIO_42: VDD_MEM_VID1 - Controlled by firmware outside of coreboot (ABL) */
	/* GPIO_43 - GPIO_66: Not available */
	/* SPI_ROM_REQ */
	PAD_NF(GPIO_67, SPI_ROM_REQ, PULL_NONE),
	/* ESPI_DATA2 */
	PAD_NF(GPIO_68, SPI1_DAT2, PULL_NONE),
	/* ESPI_DATA3 */
	PAD_NF(GPIO_69, SPI1_DAT3, PULL_NONE),
	/* SPI2_CLK */
	PAD_NF(GPIO_70, SPI2_CLK, PULL_NONE),
	/* GPIO_71 - GPIO_73: Not available */
	/* CPLD_TCK_SOC */
	PAD_NF(GPIO_74, GPIOxx, PULL_DOWN),
	/* SPI2_CS1_L */
	PAD_NF(GPIO_75, SPI2_CS1_L, PULL_NONE),
	/* SPI_ROM_GNT */
	PAD_NF(GPIO_76, SPI_ROM_GNT, PULL_NONE),
	/* ESPI_SOC_CLK */
	PAD_NF(GPIO_77, SPI1_CLK, PULL_NONE),
	/* M2_SSD0_RST_L */
	PAD_GPO(GPIO_78, HIGH),
	/* CPLD_TDI_SOC */
	PAD_NF(GPIO_79, GPIOxx, PULL_DOWN),
	/* ESPI_DATA1 */
	PAD_NF(GPIO_80, SPI1_DAT1, PULL_NONE),
	/* ESPI_DATA0 */
	PAD_NF(GPIO_81, SPI1_DAT0, PULL_NONE),
	/* GPIO_82 - GPIO_83: Not available */
	/* FANIN0 */
	PAD_NF(GPIO_84, FANIN0, PULL_NONE),
	/* FANOUT0 */
	PAD_NF(GPIO_85, FANOUT0, PULL_NONE),
	/* GPIO_86 - GPIO_88: Not available */
	/* DP_HPD_DIG# */
	PAD_SCI(GPIO_89, PULL_UP, EDGE_LOW),
	/* ALERT_L_M2_SSD0 */
	PAD_SCI(GPIO_90, PULL_UP, EDGE_LOW),
	/* PC BEEP - DNI */
	PAD_NFO(GPIO_91, SPKR, LOW),
	/* CLK_REQ0_L */
	PAD_NF(GPIO_92, CLK_REQ0_L, PULL_UP),
	/* GPIO_93 - GPIO_103: Not available */
	/* SPI2_DAT0 */
	PAD_NF(GPIO_104, SPI2_DAT0, PULL_NONE),
	/* SPI2_DAT1 */
	PAD_NF(GPIO_105, SPI2_DAT1, PULL_NONE),
	/* SPI2_DAT2 */
	PAD_NF(GPIO_106, SPI2_DAT2, PULL_NONE),
	/* SPI2_DAT3 */
	PAD_NF(GPIO_107, SPI2_DAT3, PULL_NONE),
	/* GPIO_108 - GPIO_112: Not available */
	/* CLK_REQ1_L */
	PAD_NF(GPIO_115, CLK_REQ1_L, PULL_UP),
	/* CLK_REQ2_L */
	PAD_NF(GPIO_116, CLK_REQ2_L, PULL_UP),
	/* GPIO_117 - GPIO_129: Not available */
	/* TPM IRQ */
	PAD_INT(GPIO_130, PULL_UP, EDGE_LOW, STATUS_DELIVERY),
	/* CLK_REQ3_L */
	PAD_NF(GPIO_131, CLK_REQ3_L, PULL_UP),
	/* CLK_REQ4_L */
	PAD_NF(GPIO_132, CLK_REQ4_L, PULL_UP),
	/* GPIO_133 - GPIO_134: Not available */
	/* UART0_RXD */
	PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
	/* UART0_TXD */
	PAD_NF(GPIO_143, UART0_TXD, PULL_NONE),
	/* I2S_CODEC_INT */
	PAD_SCI(GPIO_139, PULL_UP, EDGE_LOW),
	/* SFP_IOEXP_INT0_L */
	PAD_GPI(GPIO_144, PULL_UP),
	/* GPIO_149 - GPIO_152: Not available */
};

static const struct soc_amd_gpio gpio_i2c[] = {
	/* I2C0 SCL */
	PAD_NF(GPIO_145, I2C0_SCL, PULL_NONE),
	/* I2C0 SDA */
	PAD_NF(GPIO_146, I2C0_SDA, PULL_NONE),
	/* I2C1 SCL */
	PAD_NF(GPIO_147, I2C1_SCL, PULL_NONE),
	/* I2C1 SDA */
	PAD_NF(GPIO_148, I2C1_SDA, PULL_NONE),
	/* I2C2_SCL */
	PAD_NF(GPIO_113, I2C2_SCL, PULL_NONE),
	/* I2C2_SDA */
	PAD_NF(GPIO_114, I2C2_SDA, PULL_NONE),
	/* I2C3_SCL */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* I2C3_SDA */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
};

static const struct soc_amd_gpio gpio_i3c[] = {
	/* I3C0 SCL */
	PAD_NF(GPIO_145, I3C0_SCL, PULL_NONE),
	/* I3C0 SDA */
	PAD_NF(GPIO_146, I3C0_SDA, PULL_NONE),
	/* I3C1 SCL */
	PAD_NF(GPIO_147, I3C1_SCL, PULL_NONE),
	/* I3C1 SDA */
	PAD_NF(GPIO_148, I3C1_SDA, PULL_NONE),
	/* I3C2_SCL */
	PAD_NF(GPIO_113, I3C2_SCL, PULL_NONE),
	/* I3C2_SDA */
	PAD_NF(GPIO_114, I3C2_SDA, PULL_NONE),
	/* I3C3_SCL */
	PAD_NF(GPIO_19, I3C3_SCL, PULL_NONE),
	/* I3C3_SDA */
	PAD_NF(GPIO_20, I3C3_SDA, PULL_NONE),
};

static const struct soc_amd_gpio gpio_uart1[] = {
	/* UART1_TXD */
	PAD_NF(GPIO_140, UART1_TXD, PULL_NONE),
	/* UART1_RXD */
	PAD_NF(GPIO_142, UART1_RXD, PULL_NONE),
};

static const struct soc_amd_gpio gpio_uart2[] = {
	/* UART2_RXD */
	PAD_NF(GPIO_136, UART2_RXD, PULL_NONE),
	/* UART2_TXD */
	PAD_NF(GPIO_138, UART2_TXD, PULL_NONE),
};

static const struct soc_amd_gpio gpio_uart3[] = {
	/* UART3_TXD */
	PAD_NF(GPIO_135, UART3_TXD, PULL_NONE),
	/* UART3_RXD */
	PAD_NF(GPIO_137, UART3_RXD, PULL_NONE),
};

static const struct soc_amd_gpio gpio_uart4[] = {
	/* UART4_CTS_L */
	PAD_NF(GPIO_153, UART4_CTS_L, PULL_NONE),
	/* UART4_RTS_L */
	PAD_NF(GPIO_154, UART4_RTS_L, PULL_NONE),
	/* UART4_RXD */
	PAD_NF(GPIO_155, UART4_RXD, PULL_NONE),
	/* UART4_TXD */
	PAD_NF(GPIO_156, UART4_TXD, PULL_NONE),
	/* UART4_INTR - DNI */
	PAD_GPI(GPIO_157, PULL_NONE),
};

static const struct soc_amd_gpio gpio_uart0_uart2_rtscts[] = {
	/* UART2_CTS_L */
	PAD_NF(GPIO_135, UART2_CTS_L, PULL_NONE),
	/* UART2_RTS_L */
	PAD_NF(GPIO_137, UART2_RTS_L, PULL_NONE),
	/* UART0_CTS_L */
	PAD_NF(GPIO_140, UART0_CTS_L, PULL_NONE),
	/* UART0_RTS_L */
	PAD_NF(GPIO_142, UART0_RTS_L, PULL_NONE),
};

static const struct soc_amd_gpio gpio_xgbe_led[] = {
	/* xGBE LED0- Port 0 */
	PAD_NF(GPIO_136, XGBE_LED0, PULL_NONE),
	/* xGBE LED1- Port 0 */
	PAD_NF(GPIO_138, XGBE_LED1, PULL_NONE),
	/* xGBE LED2 - Port 0 */
	PAD_NF(GPIO_139, XGBE_LED2, PULL_NONE),
	/* xGBE LED3- Port 0 */
	PAD_NF(GPIO_153, XGBE_LED3, PULL_NONE),
	/* xGBE LED4- Port 1 */
	PAD_NF(GPIO_154, XGBE_LED4, PULL_NONE),
	/* xGBE LED5- Port 1 */
	PAD_NF(GPIO_155, XGBE_LED5, PULL_NONE),
	/* xGBE LED6- Port 1 */
	PAD_NF(GPIO_156, XGBE_LED6, PULL_NONE),
	/* xGBE LED7- Port 1 */
	PAD_NF(GPIO_157, XGBE_LED7, PULL_NONE),
};

void mainboard_program_gpios(void)
{
	gpio_configure_pads(gpio_set_stage_ram, ARRAY_SIZE(gpio_set_stage_ram));

	if (mb_cfg_xgbe_leds())
		gpio_configure_pads(gpio_xgbe_led, ARRAY_SIZE(gpio_xgbe_led));
	if (!mb_cfg_uart1_disabled())
		gpio_configure_pads(gpio_uart1, ARRAY_SIZE(gpio_uart1));
	if (!mb_cfg_uart2_disabled())
		gpio_configure_pads(gpio_uart2, ARRAY_SIZE(gpio_uart2));
	if (!mb_cfg_uart3_disabled())
		gpio_configure_pads(gpio_uart3, ARRAY_SIZE(gpio_uart3));
	if (!mb_cfg_uart4_disabled())
		gpio_configure_pads(gpio_uart4, ARRAY_SIZE(gpio_uart4));
	if (mb_cfg_uart024_4_wire())
		gpio_configure_pads(gpio_uart0_uart2_rtscts, ARRAY_SIZE(gpio_uart0_uart2_rtscts));
	if (mb_cfg_i2c_enabled())
		gpio_configure_pads(gpio_i2c, ARRAY_SIZE(gpio_i2c));
	else
		gpio_configure_pads(gpio_i3c, ARRAY_SIZE(gpio_i3c));
}
