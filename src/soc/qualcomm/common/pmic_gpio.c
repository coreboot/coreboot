// SPDX-License-Identifier: GPL-2.0-or-later

#include <arch/mmio.h>
#include <commonlib/bsd/stdlib.h>
#include <soc/pmic_gpio.h>
#include <soc/qcom_spmi.h>

void pmic_gpio_configure(uint8_t sid, uint8_t gpio_num,
				uint8_t source, uint8_t enable,
				uint8_t mode)
{
	if (gpio_num < PMIC_GPIO_NUMBER_MIN || gpio_num > PMIC_GPIO_NUMBER_MAX) {
		printk(BIOS_ERR, "Invalid PMIC GPIO (%d:%d)\n", sid, gpio_num);
		return;
	}

	uint32_t spmi_base = SPMI_ADDR(sid, PMIC_GPIO_BASE(gpio_num));
	if (!sid)
		spmi_base += PMIC_PMK_GPIO_OFFSET;

	spmi_write8(spmi_base + PMIC_GPIO_DIG_OUT_SOURCE_CTL, source);
	spmi_write8(spmi_base + PMIC_GPIO_EN_CTL, enable);
	spmi_write8(spmi_base + PMIC_GPIO_MODE_CTL, mode);
}

void pmic_gpio_output(uint8_t sid, uint8_t gpio_num, bool high)
{
	uint8_t state = high ? PMIC_GPIO_DIG_OUT_SOURCE_HIGH : PMIC_GPIO_DIG_OUT_SOURCE_LOW;
	pmic_gpio_configure(sid, gpio_num, state, PMIC_GPIO_EN_PERPH_EN, PMIC_GPIO_MODE_OUTPUT);
}
