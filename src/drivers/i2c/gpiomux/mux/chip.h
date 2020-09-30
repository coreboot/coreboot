/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __I2C_GPIOMUX_MUX_CHIP_H__
#define __I2C_GPIOMUX_MUX_CHIP_H__

#include <acpi/acpi_device.h>
#include <types.h>

#define MAX_NUM_MUX_GPIOS 4

struct drivers_i2c_gpiomux_mux_config {
	/* GPIOs used to select the mux lines */
	uint32_t mux_gpio_count;
	struct acpi_gpio mux_gpio[MAX_NUM_MUX_GPIOS];
};

#endif /* __I2C_GPIOMUX_MUX_CHIP_H__ */
