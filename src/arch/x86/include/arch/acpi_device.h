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

#define ACPI_DESCRIPTOR_LARGE		(1 << 7)
#define ACPI_DESCRIPTOR_INTERRUPT	(ACPI_DESCRIPTOR_LARGE | 9)
#define ACPI_DESCRIPTOR_GPIO		(ACPI_DESCRIPTOR_LARGE | 12)
#define ACPI_DESCRIPTOR_SERIAL_BUS	(ACPI_DESCRIPTOR_LARGE | 14)

struct device;
const char *acpi_device_name(struct device *dev);
const char *acpi_device_path(struct device *dev);
const char *acpi_device_scope(struct device *dev);
const char *acpi_device_path_join(struct device *dev, const char *name);

/*
 * ACPI Descriptor for extended Interrupt()
 */

enum irq_mode {
	IRQ_EDGE_TRIGGERED,
	IRQ_LEVEL_TRIGGERED
};

enum irq_polarity {
	IRQ_ACTIVE_LOW,
	IRQ_ACTIVE_HIGH,
	IRQ_ACTIVE_BOTH
};

enum irq_shared {
	IRQ_EXCLUSIVE,
	IRQ_SHARED
};

enum irq_wake {
	IRQ_NO_WAKE,
	IRQ_WAKE
};

struct acpi_irq {
	unsigned int pin;
	enum irq_mode mode;
	enum irq_polarity polarity;
	enum irq_shared shared;
	enum irq_wake wake;
};

#define IRQ_EDGE_LOW(x) { \
	.pin = (x), \
	.mode = IRQ_EDGE_TRIGGERED, \
	.polarity = IRQ_ACTIVE_LOW, \
	.shared = IRQ_EXCLUSIVE, \
	.wake = IRQ_NO_WAKE }

#define IRQ_EDGE_HIGH(x) { \
	.pin = (x), \
	.mode = IRQ_EDGE_TRIGGERED, \
	.polarity = IRQ_ACTIVE_HIGH, \
	.shared = IRQ_EXCLUSIVE, \
	.wake = IRQ_NO_WAKE }

#define IRQ_LEVEL_LOW(x) { \
	.pin = (x), \
	.mode = IRQ_LEVEL_TRIGGERED, \
	.polarity = IRQ_ACTIVE_LOW, \
	.shared = IRQ_SHARED, \
	.wake = IRQ_NO_WAKE }

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

#define ACPI_GPIO_REVISION_ID		1
#define ACPI_GPIO_MAX_PINS 		8

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
};

/* Basic output GPIO with default pull settings */
#define ACPI_GPIO_OUTPUT(gpio) { \
	.type = ACPI_GPIO_TYPE_IO, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.io_restrict = ACPI_GPIO_IO_RESTRICT_OUTPUT, \
	.pin_count = 1, \
	.pins = { (gpio) } }

/* Basic input GPIO with default pull settings */
#define ACPI_GPIO_INPUT(gpio) { \
	.type = ACPI_GPIO_TYPE_IO, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.io_restrict = ACPI_GPIO_IO_RESTRICT_INPUT, \
	.pin_count = 1, \
	.pins = { (gpio) } }

/* Basic interrupt GPIO with default pull settings */
#define ACPI_GPIO_INTERRUPT(gpio,mode,polarity) { \
	.type = ACPI_GPIO_TYPE_INTERRUPT, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.irq.mode = (mode), \
	.irq.polarity = (polarity), \
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

enum spi_clock_phase {
	SPI_CLOCK_PHASE_FIRST,
	SPI_CLOCK_PHASE_SECOND
};

/* SPI Flags bit 0 */
enum spi_wire_mode {
	SPI_4_WIRE_MODE,
	SPI_3_WIRE_MODE
};

/* SPI Flags bit 1 */
enum spi_polarity {
	SPI_POLARITY_LOW,
	SPI_POLARITY_HIGH
};

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

/*
 * Device Properties with _DSD
 * http://uefi.org/sites/default/files/resources/_DSD-device-properties-UUID.pdf
 */

#define ACPI_DP_UUID "daffd814-6eba-4d8c-8a91-bc9bbf4aa301"

enum acpi_dp_type {
	ACPI_DP_TYPE_INTEGER,
	ACPI_DP_TYPE_STRING,
	ACPI_DP_TYPE_REFERENCE,
};

struct acpi_dp {
	enum acpi_dp_type type;
	union {
		uint64_t integer;
		const char *string;
	};
};

#define ACPI_DP_INTEGER(x)   { .type = ACPI_DP_TYPE_INTEGER, .integer = x }
#define ACPI_DP_STRING(x)    { .type = ACPI_DP_TYPE_STRING, .string = x }
#define ACPI_DP_REFERENCE(x) { .type = ACPI_DP_TYPE_REFERENCE, .string = x }

/*
 * Writing Device Properties objects via _DSD
 */

/* Start a set of Device Properties with _DSD and UUID */
void acpi_dp_write_header(void);

/* End the Device Properties set and fill in length values */
void acpi_dp_write_footer(void);

/* Write a Device Property value, but not the key */
void acpi_dp_write_value(const struct acpi_dp *prop);

/* Write a Device Property, both key and value */
void acpi_dp_write_keyval(const char *key, const struct acpi_dp *prop);

/* Write an integer as a Device Property */
void acpi_dp_write_integer(const char *key, uint64_t value);

/* Write a string as a Device Property */
void acpi_dp_write_string(const char *key, const char *value);

/* Write an ACPI reference as a Device Property */
void acpi_dp_write_reference(const char *key, const char *value);

/* Write an array of Device Properties */
void acpi_dp_write_array(const char *key, const struct acpi_dp *array, int len);

/* Write an array of integers as Device Properties */
void acpi_dp_write_integer_array(const char *key, uint64_t *array, int len);

/* Write a GPIO binding Device Property */
void acpi_dp_write_gpio(const char *key, const char *ref, int index,
			int pin, int active_low);

#endif
