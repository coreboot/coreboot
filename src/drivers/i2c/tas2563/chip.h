/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_I2C_TAS2563_CHIP_H__
#define __DRIVERS_I2C_TAS2563_CHIP_H__

#include <acpi/acpi_device.h>

#define TAS2563_MAX_AUDIO_SLOTS	8

struct drivers_i2c_tas2563_config {
	/* I2C Bus Speed */
	enum i2c_speed bus_speed;

	/* IRQ configuration */
	struct acpi_irq irq;
	struct acpi_gpio irq_gpio;

	/* Reset GPIO configuration */
	struct acpi_gpio reset_gpio;

	/* Audio slots configuration */
	uint64_t audio_slots[TAS2563_MAX_AUDIO_SLOTS];
};

#endif /* __DRIVERS_I2C_TAS2563_CHIP_H__ */
