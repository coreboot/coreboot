/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>
#include <variant/gpio.h>

static const struct pad_config early_gpio_table[] = {
	PAD_CFG_GPO(GPP_C14, 1, RSMRST), // M.2_PLT_RST_CNTRL1#
	PAD_CFG_GPO(GPP_C15, 1, RSMRST), // M.2_PLT_RST_CNTRL2#
	PAD_CFG_GPO(GPP_F0, 1, RSMRST), // TBT_PERST_N
	PAD_CFG_GPO(GPP_F22, 0, DEEP), // DGPU_RST#_PCH
	PAD_CFG_GPO(GPP_F23, 0, DEEP), // DGPU_PWR_EN
	PAD_CFG_GPO(GPP_H16, 1, RSMRST), // TBT_RTD3_PWR_EN_R
	PAD_CFG_GPO(GPP_K8, 1, RSMRST), // SATA_M2_PWR_EN1
	PAD_CFG_GPO(GPP_K9, 1, RSMRST), // SATA_M2_PWR_EN2
	PAD_CFG_GPO(GPP_K11, 1, RSMRST), // GPIO_LANRTD3
};

void variant_configure_early_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
