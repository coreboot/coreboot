/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <ec/google/chromeec/ec.h>
#include <baseboard/variants.h>
#include <delay.h>
#include <gpio.h>

struct gpio_with_delay {
	gpio_t gpio;
	unsigned int delay_msecs;
};

static void power_off_lte_module(u8 slp_typ)
{
	const struct gpio_with_delay lte_power_off_gpios[] = {
		{
			GPIO_161, /* AVS_I2S1_MCLK -- PLT_RST_LTE_L */
			30,
		},
		{
			GPIO_117, /* PCIE_WAKE1_B -- FULL_CARD_POWER_OFF */
			100
		},
		{
			GPIO_67, /* UART2-CTS_B -- EN_PP3300_DX_LTE_SOC */
			0
		}
	};

	for (int i = 0; i < ARRAY_SIZE(lte_power_off_gpios); i++) {
		gpio_output(lte_power_off_gpios[i].gpio, 0);
		mdelay(lte_power_off_gpios[i].delay_msecs);
	}
}

void variant_smi_sleep(u8 slp_typ)
{
	/* Currently use cases here all target to S5 therefore we do early return
	 * here for saving one transaction to the EC for getting SKU ID. */
	if (slp_typ != ACPI_S5)
		return;

	power_off_lte_module(slp_typ);
}
