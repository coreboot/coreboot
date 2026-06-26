/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

/*
 * DFR1142 lite carrier pad overrides: the pads from the original mainboard
 * gpio table that are routed to this carrier via the edge connector (HDMI
 * DDI control/HPD, SATA activity LED, SMBus, and the SML1 link).
 */
static const struct pad_config override_gpio_table[] = {
	PAD_CFG_GPO(GPP_B14, 0, PLTRST),

	/* H15 : DDPB_CTRLCLK ==> DDIB_HDMI_CTRLCLK */
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF1),
	/* H17 : DDPB_CTRLDATA ==> DDIB_HDMI_CTRLDATA */
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),
	/* DDIB_DP_HDMI_ALS_HDP */
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),

	/* SMB_CLK */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
	/* SMB_DATA */
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),
	/* SMB_ALERT_N */
	PAD_CFG_NF(GPP_C2, NONE, DEEP, NF1),
	/* SML1_CLK */
	PAD_CFG_NF(GPP_C6, NONE, RSMRST, NF1),
	/* SML1_DATA */
	PAD_CFG_NF(GPP_C7, NONE, RSMRST, NF1),
};

/* GPIOs configured before console is up (LPSS UART console only). */
static const struct pad_config early_uart_gpio_table[] = {
	/* UART0 RX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* UART0 TX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* UART1 RX */
	PAD_CFG_NF(GPP_D17, NONE, DEEP, NF1),
	/* UART1 TX */
	PAD_CFG_NF(GPP_D18, NONE, DEEP, NF1),
	/* UART2 RX */
	PAD_CFG_NF(GPP_F1, NONE, DEEP, NF2),
	/* UART2 TX */
	PAD_CFG_NF(GPP_F2, NONE, DEEP, NF2),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(override_gpio_table);
	return override_gpio_table;
}

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	if (!CONFIG(INTEL_LPSS_UART_FOR_CONSOLE)) {
		*num = 0;
		return NULL;
	}

	*num = ARRAY_SIZE(early_uart_gpio_table);
	return early_uart_gpio_table;
}
