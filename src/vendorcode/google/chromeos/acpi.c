/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpigen.h>
#if CONFIG(GENERIC_GPIO_LIB)
#include <gpio.h>
#endif
#include "chromeos.h"

void chromeos_acpi_gpio_generate(const struct cros_gpio *gpios, size_t num)
{
	size_t i;
	int gpio_num;

	acpigen_write_scope("\\");
	acpigen_write_name("OIPG");

	acpigen_write_package(num);
	for (i = 0; i < num; i++) {
		acpigen_write_package(4);
		acpigen_write_integer(gpios[i].type);
		acpigen_write_integer(gpios[i].polarity);
		gpio_num = gpios[i].gpio_num;
#if CONFIG(GENERIC_GPIO_LIB)
		/* Get ACPI pin from GPIO library if available */
		if (gpios[i].gpio_num != CROS_GPIO_VIRTUAL)
			gpio_num = gpio_acpi_pin(gpio_num);
#endif
		acpigen_write_integer(gpio_num);
		acpigen_write_string(gpios[i].device);
		acpigen_pop_len();
	}
	acpigen_pop_len();

	acpigen_pop_len();
}

void chromeos_dsdt_generator(struct device *dev)
{
	mainboard_chromeos_acpi_generate();
}
