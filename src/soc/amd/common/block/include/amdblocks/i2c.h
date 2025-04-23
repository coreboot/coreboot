/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_COMMON_BLOCK_I2C_H
#define AMD_COMMON_BLOCK_I2C_H

#include <amdblocks/gpio.h>
#include <device/i2c.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <types.h>

/* Enum to identify in which mode the I2C controller is operating. */
enum i2c_ctrlr_mode {
	I2C_MASTER_MODE,
	I2C_PERIPHERAL_MODE,
};

/**
 * Data structure to hold SoC I2C controller information
 * @bar:	MMIO base address for the I2C bus.
 * @acpi_name:	ACPI Name corresponding to the I2C bus.
 */
struct soc_i2c_ctrlr_info {
	enum i2c_ctrlr_mode mode;
	uintptr_t bar;
	const char *acpi_name;
};

/**
 * Data structure to identify GPIO to be toggled to reset peripherals on an I2C bus.
 * @pin:	GPIO corresponding to I2C SCL that needs to be toggled/bit-banged.
 * @pin_mask:	Bit Mask of a single I2C bus that needs to be reset.
 */
struct soc_i2c_scl_pin {
	struct soc_amd_gpio pin;
	uint8_t pin_mask;
};

/* Macro to populate the elements of the array of soc_i2c_scl_pin in the SoC code */
#define I2C_RESET_SCL_PIN(pin_name, pin_mask_value)					\
{											\
	.pin = PAD_CFG_STRUCT(pin_name, pin_name ## _IOMUX_GPIOxx, PAD_OUTPUT(HIGH)),	\
	.pin_mask = pin_mask_value,							\
}

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
	size_t num_pins;
};

enum i2c_pad_rx_level {
	I2C_PAD_RX_NO_CHANGE,
	I2C_PAD_RX_OFF,
	I2C_PAD_RX_3_3V,
	I2C_PAD_RX_1_8V,
	I2C_PAD_RX_1_1V,
	I3C_PAD_RX_NO_CHANGE,
	I3C_PAD_RX_1_8V,
	I3C_PAD_RX_1_1V,
};

struct i2c_pad_control {
	enum i2c_pad_rx_level rx_level;
};

struct soc_i3c_ctrlr_info {
	uintptr_t bar;
	const char *acpi_name;
};

void fch_i2c_pad_init(unsigned int bus,
		      enum i2c_speed speed,
		      const struct i2c_pad_control *ctrl);

void fch_i23c_pad_init(unsigned int bus,
		       enum i2c_speed speed,
		       const struct i2c_pad_control *ctrl);

/* Helper function to perform misc I2C configuration specific to SoC. */
void soc_i2c_misc_init(unsigned int bus, const struct dw_i2c_bus_config *cfg);

/* Getter function to get the SoC I2C Controller Information. */
const struct soc_i2c_ctrlr_info *soc_get_i2c_ctrlr_info(size_t *num_ctrlrs);

/* Getter function to get the SoC I2C bus configuration. */
const struct dw_i2c_bus_config *soc_get_i2c_bus_config(size_t *num_buses);

/* Initialize all the i2c buses that are marked with early init. */
void i2c_soc_early_init(void);

/* Initialize all the i2c buses that are not marked with early init. */
void i2c_soc_init(void);

/* Reset I2C peripherals. */
void sb_reset_i2c_peripherals(const struct soc_i2c_peripheral_reset_info *reset_info);

/* Getter function to get the SoC I3C controller information. */
const struct soc_i3c_ctrlr_info *soc_get_i3c_ctrlr_info(size_t *num_ctrlrs);

#endif /* AMD_COMMON_BLOCK_I2C_H */
