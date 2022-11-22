/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <ec/intel/board_id.h>
#include <soc/soc_info.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Early pad configuration in bootblock */
static const struct pad_config mtl_p_rvp_early_gpio_table[] = {
	/* Audio */
	PAD_NC(GPP_D14, NONE), /* M.2_BT_I2S2_PCMCLK/NONE */
	PAD_NC(GPP_D15, NONE), /* M.2_BT_I2S2_PCMFRM/NONE */
	PAD_CFG_GPO(GPP_S04, 1, DEEP), /* EN_SPKR_PA */
	PAD_CFG_GPI_INT(GPP_S05, NONE, PLTRST, EDGE_BOTH),

	/* SMBUS */
	PAD_CFG_NF(GPP_C00, NONE, DEEP, NF1), /* GPP_C0_SMBCLK  */
	PAD_CFG_NF(GPP_C01, NONE, DEEP, NF1), /* GPP_C1_SMBDATA */

	/* SSD */
	PAD_CFG_GPO(GPP_B15, 0, DEEP), /* M.2_CPU_SSD3_PWREN */
	PAD_CFG_GPO(GPP_D06, 0, DEEP), /* M.2_CPU_SSD4_PWREN */
};

static const struct pad_config early_uart_gpio_table[] = {
	/* UART0 */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1), /* UART0_RXD */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1), /* UART0_TXD */
};

static const struct pad_config early_wwan_on_gpio_table[] = {
	/* M.2 WWAN */
	PAD_CFG_GPO(GPP_B17, 1, DEEP), /* WWAN_PWREN */
	PAD_CFG_GPO(GPP_E07, 1, DEEP), /* M.2_WWAN_FCP_OFF_N */
	PAD_CFG_GPO(GPP_A15, 0, DEEP), /* M.2_WWAN_RST_N */
	PAD_CFG_GPO(GPP_C05, 0, DEEP), /* M.2_WWAN_PERST_GPIO_N */
	PAD_CFG_GPI_SCI(GPP_F10, NONE, DEEP, LEVEL, INVERT), /* M.2_WWAN_WAKE_GPIO_N */
};

void configure_early_gpio_pads(void)
{
	uint8_t board_id = get_rvp_board_id();

	if (CONFIG(INTEL_LPSS_UART_FOR_CONSOLE))
		gpio_configure_pads(early_uart_gpio_table, ARRAY_SIZE(early_uart_gpio_table));

	switch (board_id) {
	case MTLP_DDR5_RVP:
	case MTLP_LP5_T3_RVP:
	case MTLP_LP5_T4_RVP:
		gpio_configure_pads(mtl_p_rvp_early_gpio_table,
				    ARRAY_SIZE(mtl_p_rvp_early_gpio_table));
		gpio_configure_pads(early_wwan_on_gpio_table,
				    ARRAY_SIZE(early_wwan_on_gpio_table));
		break;
	default:
		printk(BIOS_WARNING, "Invalid board_id 0x%x."
		       "Skipping early gpio configuration\n", board_id);
		break;
	}
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
};
DECLARE_CROS_GPIOS(cros_gpios);
