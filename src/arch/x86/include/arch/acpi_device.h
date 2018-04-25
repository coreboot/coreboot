/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#ifndef __ACPI_DEVICE_H
#define __ACPI_DEVICE_H

#include <device/i2c.h>
#include <stdint.h>
#include <spi-generic.h>

enum acpi_dp_type {
	ACPI_DP_TYPE_UNKNOWN,
	ACPI_DP_TYPE_INTEGER,
	ACPI_DP_TYPE_STRING,
	ACPI_DP_TYPE_REFERENCE,
	ACPI_DP_TYPE_TABLE,
	ACPI_DP_TYPE_ARRAY,
	ACPI_DP_TYPE_CHILD,
};

struct acpi_dp {
	enum acpi_dp_type type;
	const char *name;
	struct acpi_dp *next;
	union {
		struct acpi_dp *child;
		struct acpi_dp *array;
	};
	union {
		uint64_t integer;
		const char *string;
	};
};

#define ACPI_DESCRIPTOR_LARGE		(1 << 7)
#define ACPI_DESCRIPTOR_INTERRUPT	(ACPI_DESCRIPTOR_LARGE | 9)
#define ACPI_DESCRIPTOR_GPIO		(ACPI_DESCRIPTOR_LARGE | 12)
#define ACPI_DESCRIPTOR_SERIAL_BUS	(ACPI_DESCRIPTOR_LARGE | 14)

/*
 * PRP0001 is a special DT namespace link device ID. It provides a means to use
 * existing DT-compatible device identification in ACPI. When this _HID is used
 * by an ACPI device, the ACPI subsystem in OS looks up "compatible" property in
 * device object's _DSD and will use the value of that property to identify the
 * corresponding device in analogy with the original DT device identification
 * algorithm.
 * More details can be found in Linux kernel documentation:
 * Documentation/acpi/enumeration.txt
 */
#define ACPI_DT_NAMESPACE_HID		"PRP0001"

struct device;
const char *acpi_device_name(struct device *dev);
const char *acpi_device_path(struct device *dev);
const char *acpi_device_scope(struct device *dev);
const char *acpi_device_path_join(struct device *dev, const char *name);

/*
 * ACPI Descriptor for extended Interrupt()
 */

enum acpi_irq_mode {
	ACPI_IRQ_EDGE_TRIGGERED,
	ACPI_IRQ_LEVEL_TRIGGERED
};

enum acpi_irq_polarity {
	ACPI_IRQ_ACTIVE_LOW,
	ACPI_IRQ_ACTIVE_HIGH,
	ACPI_IRQ_ACTIVE_BOTH
};

enum acpi_irq_shared {
	ACPI_IRQ_EXCLUSIVE,
	ACPI_IRQ_SHARED
};

enum acpi_irq_wake {
	ACPI_IRQ_NO_WAKE,
	ACPI_IRQ_WAKE
};

struct acpi_irq {
	unsigned int pin;
	enum acpi_irq_mode mode;
	enum acpi_irq_polarity polarity;
	enum acpi_irq_shared shared;
	enum acpi_irq_wake wake;
};

#define ACPI_IRQ_EDGE_LOW(x) { \
	.pin = (x), \
	.mode = ACPI_IRQ_EDGE_TRIGGERED, \
	.polarity = ACPI_IRQ_ACTIVE_LOW, \
	.shared = ACPI_IRQ_EXCLUSIVE, \
	.wake = ACPI_IRQ_NO_WAKE }

#define ACPI_IRQ_EDGE_HIGH(x) { \
	.pin = (x), \
	.mode = ACPI_IRQ_EDGE_TRIGGERED, \
	.polarity = ACPI_IRQ_ACTIVE_HIGH, \
	.shared = ACPI_IRQ_EXCLUSIVE, \
	.wake = ACPI_IRQ_NO_WAKE }

#define ACPI_IRQ_LEVEL_LOW(x) { \
	.pin = (x), \
	.mode = ACPI_IRQ_LEVEL_TRIGGERED, \
	.polarity = ACPI_IRQ_ACTIVE_LOW, \
	.shared = ACPI_IRQ_SHARED, \
	.wake = ACPI_IRQ_NO_WAKE }

#define ACPI_IRQ_LEVEL_HIGH(x) { \
	.pin = (x), \
	.mode = ACPI_IRQ_LEVEL_TRIGGERED, \
	.polarity = ACPI_IRQ_ACTIVE_HIGH, \
	.shared = ACPI_IRQ_SHARED, \
	.wake = ACPI_IRQ_NO_WAKE }

/* Write extended Interrupt() descriptor to SSDT AML output */
void acpi_device_write_interrupt(const struct acpi_irq *irq);

/*
 * ACPI Descriptors for GpioIo() and GpioInterrupt()
 */

enum acpi_gpio_type {
	ACPI_GPIO_TYPE_INTERRUPT,
	ACPI_GPIO_TYPE_IO
};

enum acpi_gpio_pull {
	ACPI_GPIO_PULL_DEFAULT,
	ACPI_GPIO_PULL_UP,
	ACPI_GPIO_PULL_DOWN,
	ACPI_GPIO_PULL_NONE
};

enum acpi_gpio_io_restrict {
	ACPI_GPIO_IO_RESTRICT_NONE,
	ACPI_GPIO_IO_RESTRICT_INPUT,
	ACPI_GPIO_IO_RESTRICT_OUTPUT,
	ACPI_GPIO_IO_RESTRICT_PRESERVE
};

enum acpi_gpio_polarity {
	ACPI_GPIO_ACTIVE_HIGH = 0,
	ACPI_GPIO_ACTIVE_LOW = 1,
};

#define ACPI_GPIO_REVISION_ID		1
#define ACPI_GPIO_MAX_PINS		8

struct acpi_gpio {
	int pin_count;
	uint16_t pins[ACPI_GPIO_MAX_PINS];

	enum acpi_gpio_type type;
	enum acpi_gpio_pull pull;
	const char *resource;

	/* GpioInt */
	uint16_t interrupt_debounce_timeout;	/* 1/100 ms */
	struct acpi_irq irq;

	/* GpioIo */
	uint16_t output_drive_strength;		/* 1/100 mA */
	int io_shared;
	enum acpi_gpio_io_restrict io_restrict;
	enum acpi_gpio_polarity polarity;
};

/* Basic output GPIO with default pull settings */
#define ACPI_GPIO_OUTPUT_ACTIVE_HIGH(gpio) { \
	.type = ACPI_GPIO_TYPE_IO, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.io_restrict = ACPI_GPIO_IO_RESTRICT_OUTPUT, \
	.polarity = ACPI_GPIO_ACTIVE_HIGH,     \
	.pin_count = 1, \
	.pins = { (gpio) } }

#define ACPI_GPIO_OUTPUT_ACTIVE_LOW(gpio) { \
	.type = ACPI_GPIO_TYPE_IO, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.io_restrict = ACPI_GPIO_IO_RESTRICT_OUTPUT, \
	.polarity = ACPI_GPIO_ACTIVE_LOW,     \
	.pin_count = 1, \
	.pins = { (gpio) } }

/* Basic input GPIO with default pull settings */
#define ACPI_GPIO_INPUT_ACTIVE_HIGH(gpio) {   \
	.type = ACPI_GPIO_TYPE_IO, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.io_restrict = ACPI_GPIO_IO_RESTRICT_INPUT, \
	.polarity = ACPI_GPIO_ACTIVE_HIGH,     \
	.pin_count = 1, \
	.pins = { (gpio) } }

#define ACPI_GPIO_INPUT_ACTIVE_LOW(gpio) {   \
	.type = ACPI_GPIO_TYPE_IO, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.io_restrict = ACPI_GPIO_IO_RESTRICT_INPUT, \
	.polarity = ACPI_GPIO_ACTIVE_LOW,     \
	.pin_count = 1, \
	.pins = { (gpio) } }

/* Edge Triggered Active High GPIO interrupt */
#define ACPI_GPIO_IRQ_EDGE_HIGH(gpio) { \
	.type = ACPI_GPIO_TYPE_INTERRUPT, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.irq.mode = ACPI_IRQ_EDGE_TRIGGERED, \
	.irq.polarity = ACPI_IRQ_ACTIVE_HIGH, \
	.pin_count = 1, \
	.pins = { (gpio) } }

/* Edge Triggered Active Low GPIO interrupt */
#define ACPI_GPIO_IRQ_EDGE_LOW(gpio) { \
	.type = ACPI_GPIO_TYPE_INTERRUPT, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.irq.mode = ACPI_IRQ_EDGE_TRIGGERED, \
	.irq.polarity = ACPI_IRQ_ACTIVE_LOW, \
	.pin_count = 1, \
	.pins = { (gpio) } }

/* Edge Triggered Active Both GPIO interrupt */
#define ACPI_GPIO_IRQ_EDGE_BOTH(gpio) { \
	.type = ACPI_GPIO_TYPE_INTERRUPT, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.irq.mode = ACPI_IRQ_EDGE_TRIGGERED, \
	.irq.polarity = ACPI_IRQ_ACTIVE_BOTH, \
	.pin_count = 1, \
	.pins = { (gpio) } }

/* Edge Triggered Active High GPIO interrupt with wake */
#define ACPI_GPIO_IRQ_EDGE_HIGH_WAKE(gpio) { \
	.type = ACPI_GPIO_TYPE_INTERRUPT, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.irq.mode = ACPI_IRQ_EDGE_TRIGGERED, \
	.irq.polarity = ACPI_IRQ_ACTIVE_HIGH, \
	.irq.wake = ACPI_IRQ_WAKE, \
	.pin_count = 1, \
	.pins = { (gpio) } }

/* Edge Triggered Active Low GPIO interrupt with wake */
#define ACPI_GPIO_IRQ_EDGE_LOW_WAKE(gpio) { \
	.type = ACPI_GPIO_TYPE_INTERRUPT, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.irq.mode = ACPI_IRQ_EDGE_TRIGGERED, \
	.irq.polarity = ACPI_IRQ_ACTIVE_LOW, \
	.irq.wake = ACPI_IRQ_WAKE, \
	.pin_count = 1, \
	.pins = { (gpio) } }

/* Edge Triggered Active Both GPIO interrupt with wake */
#define ACPI_GPIO_IRQ_EDGE_BOTH_WAKE(gpio) { \
	.type = ACPI_GPIO_TYPE_INTERRUPT, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.irq.mode = ACPI_IRQ_EDGE_TRIGGERED, \
	.irq.polarity = ACPI_IRQ_ACTIVE_BOTH, \
	.irq.wake = ACPI_IRQ_WAKE, \
	.pin_count = 1, \
	.pins = { (gpio) } }

/* Level Triggered Active High GPIO interrupt */
#define ACPI_GPIO_IRQ_LEVEL_HIGH(gpio) { \
	.type = ACPI_GPIO_TYPE_INTERRUPT, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.irq.mode = ACPI_IRQ_LEVEL_TRIGGERED, \
	.irq.polarity = ACPI_IRQ_ACTIVE_HIGH, \
	.pin_count = 1, \
	.pins = { (gpio) } }

/* Level Triggered Active Low GPIO interrupt */
#define ACPI_GPIO_IRQ_LEVEL_LOW(gpio) { \
	.type = ACPI_GPIO_TYPE_INTERRUPT, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.irq.mode = ACPI_IRQ_LEVEL_TRIGGERED, \
	.irq.polarity = ACPI_IRQ_ACTIVE_LOW, \
	.pin_count = 1, \
	.pins = { (gpio) } }

/* Write GpioIo() or GpioInt() descriptor to SSDT AML output */
void acpi_device_write_gpio(const struct acpi_gpio *gpio);

/*
 * ACPI Descriptors for Serial Bus interfaces
 */

#define ACPI_SERIAL_BUS_TYPE_I2C	1
#define ACPI_SERIAL_BUS_TYPE_SPI	2
#define ACPI_SERIAL_BUS_REVISION_ID	1

/*
 * ACPI I2C Bus
 */

struct acpi_i2c {
	/* I2C Address */
	uint16_t address;
	/* 7 or 10 bit Address Mode */
	enum i2c_address_mode mode_10bit;
	/* I2C Bus Speed in Hz */
	enum i2c_speed speed;
	/* Reference to I2C controller */
	const char *resource;
};

/* Write I2cSerialBus() descriptor to SSDT AML output */
void acpi_device_write_i2c(const struct acpi_i2c *i2c);

/*
 * ACPI SPI Bus
 */

struct acpi_spi {
	/* Device selection */
	uint16_t device_select;
	/* Device selection line is active high or low */
	enum spi_polarity device_select_polarity;
	/* 3 or 4 wire SPI connection */
	enum spi_wire_mode wire_mode;
	/* Connection speed in HZ */
	unsigned int speed;
	/* Size in bits of smallest transfer unit */
	u8 data_bit_length;
	/* Phase of clock pulse on which to capture data */
	enum spi_clock_phase clock_phase;
	/* Indicate if clock is high or low during first phase */
	enum spi_polarity clock_polarity;
	/* Reference to SPI controller */
	const char *resource;
};

/* Write SPI Bus descriptor to SSDT AML output */
void acpi_device_write_spi(const struct acpi_spi *spi);

/* GPIO/timing information for the power on/off sequences */
struct acpi_power_res_params {
	/* GPIO used to take device out of reset or to put it into reset. */
	struct acpi_gpio *reset_gpio;
	/* Delay to be inserted after device is taken out of reset.
	 * (_ON method delay)
	 */
	unsigned int reset_delay_ms;
	/* Delay to be inserted after device is put into reset.
	 * (_OFF method delay)
	 */
	unsigned int reset_off_delay_ms;
	/* GPIO used to enable device. */
	struct acpi_gpio *enable_gpio;
	/* Delay to be inserted after device is enabled.
	 * (_ON method delay)
	 */
	unsigned int enable_delay_ms;
	/* Delay to be inserted after device is disabled.
	 * (_OFF method delay)
	 */
	unsigned int enable_off_delay_ms;
	/* GPIO used to stop operation of device. */
	struct acpi_gpio *stop_gpio;
	/* Delay to be inserted after disabling stop.
	 * (_ON method delay)
	 */
	unsigned int stop_delay_ms;
	/* Delay to be inserted after enabling stop.
	 * (_OFF method delay)
	 */
	unsigned int stop_off_delay_ms;
};

/*
 * Add a basic PowerResource block for a device that includes
 * GPIOs to control enable, reset and stop operation of the device. Each
 * GPIO is optional, but at least one must be provided.
 *
 * Reset - Put the device into / take the device out of reset.
 * Enable - Enable / disable power to device.
 * Stop - Stop / start operation of device.
 */
void acpi_device_add_power_res(const struct acpi_power_res_params *params);

/*
 * Writing Device Properties objects via _DSD
 *
 * http://uefi.org/sites/default/files/resources/_DSD-device-properties-UUID.pdf
 * http://uefi.org/sites/default/files/resources/_DSD-hierarchical-data-extension-UUID-v1.pdf
 *
 * The Device Property Hierarchy can be multiple levels deep with multiple
 * children possible in each level.  In order to support this flexibility
 * the device property hierarchy must be built up before being written out.
 *
 * For example:
 *
 * // Child table with string and integer
 * struct acpi_dp *child = acpi_dp_new_table("CHLD");
 * acpi_dp_add_string(child, "childstring", "CHILD");
 * acpi_dp_add_integer(child, "childint", 100);
 *
 * // _DSD table with integer and gpio and child pointer
 * struct acpi_dp *dsd = acpi_dp_new_table("_DSD");
 * acpi_dp_add_integer(dsd, "number1", 1);
 * acpi_dp_add_gpio(dsd, "gpio", "\_SB.PCI0.GPIO", 0, 0, 1);
 * acpi_dp_add_child(dsd, "child", child);
 *
 * // Write entries into SSDT and clean up resources
 * acpi_dp_write(dsd);
 *
 * Name(_DSD, Package() {
 *   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301")
 *   Package() {
 *     Package() { "gpio", Package() { \_SB.PCI0.GPIO, 0, 0, 0 } }
 *     Package() { "number1", 1 }
 *   }
 *   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b")
 *   Package() {
 *     Package() { "child", CHLD }
 *   }
 * }
 * Name(CHLD, Package() {
 *   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301")
 *   Package() {
 *     Package() { "childstring", "CHILD" }
 *     Package() { "childint", 100 }
 *   }
 * }
 */

/* Start a new Device Property table with provided ACPI reference */
struct acpi_dp *acpi_dp_new_table(const char *ref);

/* Add integer Device Property */
struct acpi_dp *acpi_dp_add_integer(struct acpi_dp *dp, const char *name,
				    uint64_t value);

/* Add string Device Property */
struct acpi_dp *acpi_dp_add_string(struct acpi_dp *dp, const char *name,
				   const char *string);

/* Add ACPI reference Device Property */
struct acpi_dp *acpi_dp_add_reference(struct acpi_dp *dp, const char *name,
				      const char *reference);

/* Add an array of Device Properties */
struct acpi_dp *acpi_dp_add_array(struct acpi_dp *dp, struct acpi_dp *array);

/* Add an array of integers Device Property */
struct acpi_dp *acpi_dp_add_integer_array(struct acpi_dp *dp, const char *name,
					  uint64_t *array, int len);

/* Add a GPIO binding Device Property */
struct acpi_dp *acpi_dp_add_gpio(struct acpi_dp *dp, const char *name,
				 const char *ref, int index, int pin,
				 int active_low);

/* Add a child table of Device Properties */
struct acpi_dp *acpi_dp_add_child(struct acpi_dp *dp, const char *name,
				  struct acpi_dp *child);

/* Add a list of Device Properties, returns the number of properties added */
size_t acpi_dp_add_property_list(struct acpi_dp *dp,
				 const struct acpi_dp *property_list,
				 size_t property_count);

/* Write Device Property hierarchy and clean up resources */
void acpi_dp_write(struct acpi_dp *table);

#endif
