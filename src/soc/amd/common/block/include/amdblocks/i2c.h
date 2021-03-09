/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_COMMON_BLOCK_I2C_H
#define AMD_COMMON_BLOCK_I2C_H

#include <amdblocks/gpio_banks.h>
#include <types.h>

/**
 * Data structure to identify GPIO to be toggled to reset peripherals on an I2C bus.
 * @pin:	GPIO corresponding to I2C SCL that needs to be toggled/bit-banged.
 * @pin_mask:	Bit Mask of a single I2C bus that needs to be reset.
 */
struct soc_i2c_scl_pin {
	struct soc_amd_gpio pin;
	uint8_t pin_mask;
};

/**
 * Information about I2C peripherals that need to be reset.
 * @i2c_scl_reset_mask:	Bit mask of I2C buses that need to be reset based on the device tree
 *			configuration.
 * @i2c_scl:		SoC specific I2C SCL pins that need to be bit-banged as part of reset
 *			procedure.
 * @num_pins:		Number of pins defined in @i2c_scl.
 */
struct soc_i2c_peripheral_reset_info {
	uint8_t i2c_scl_reset_mask;
	const struct soc_i2c_scl_pin *i2c_scl;
	uint32_t num_pins;
};

/* Reset I2C peripherals. */
void sb_reset_i2c_peripherals(const struct soc_i2c_peripheral_reset_info *reset_info);

#endif /* AMD_COMMON_BLOCK_I2C_H */
