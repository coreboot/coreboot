/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <drivers/tpm/cr50.h>
#include <gpio.h>

__weak gpio_t cr50_override_gpio(gpio_t irq)
{
	return irq;
}

int cr50_plat_irq_status(void)
{
	gpio_t irq_gpio = CONFIG_GSC_IRQ_GPIO;

	irq_gpio = cr50_override_gpio(irq_gpio);

	return gpio_interrupt_status(irq_gpio);
}
